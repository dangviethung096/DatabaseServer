#include "lib/server_struct.h"
#include "lib/server_def.h"
#include "db/db_def.h"
#include "db/db_api.h"
#include "lib/server_global.h"
#include <string.h>
#include <stdlib.h>

static inline void read_message(U8bit * message, int * pos, U8bit * val, int size)
{
    memcpy(val, message + *pos, size);
    SERVER_TRACE(("SERVER:read_message:pos=%d, size=%d\n", *pos, size));
    *pos += size;
}

static inline void write_message(U8bit * message, int * pos, U8bit * val, int size)
{
    memcpy(message + *pos, val, size);
    SERVER_TRACE(("SERVER:write_message:pos=%d, size=%d\n", *pos, size));
    *pos += size;
}

static inline int process_msg_condition(U8bit * message, int pos, db_condition_t * cond)
{
    /* Declare variable */
    int i;
    U8bit num_cond;
    U8bit length_field, length_val;
    /* Read number condition */
    read_message(message, &pos, &num_cond, DB_U_8_BIT_SIZE);
    cond->num_cond = num_cond;
    SERVER_TRACE(("SERVER:process_msg_condition:num_cond=%d\n", cond->num_cond));
    /* Read condition */
    for(i = 0; i < num_cond; i++)
    {
        // Read length of field
        read_message(message, &pos, &length_field, DB_U_8_BIT_SIZE);
        // Read field condition
        cond->field_conditions[i] = (U8bit *) malloc(DB_U_8_BIT_SIZE * (length_field + 1));
        read_message(message, &pos, cond->field_conditions[i], length_field);
        cond->field_conditions[i][length_field] = '\0';
        SERVER_TRACE(("SERVER:process_msg_condition:field_condition[%d]=%s\n", i, cond->field_conditions[i]));
        // Read length of value
        read_message(message, &pos, &length_val, DB_U_8_BIT_SIZE);
        // Read value condition
        cond->val_conditions[i] = (U8bit *) malloc(DB_U_8_BIT_SIZE * (length_val + 1));
        read_message(message, &pos, cond->val_conditions[i], length_val);
        cond->val_conditions[i][length_val] = '\0';
        SERVER_TRACE(("SERVER:process_msg_condition:val_condition[%d]=%s\n", i, cond->val_conditions[i]));
        cond->operator_conditions[i] = DB_COND_EQUAL;
    }

    return pos;
}

int process_msg_search(U8bit * message, U8bit * ret_message)
{
    // Declare variable
    int i, j;
    U8bit table_name[DB_MAX_LENGTH_TABLE_NAME];
    U8bit * columns[DB_MAX_FIELDS_IN_TABLE];
    db_condition_t cond;

    // Read table name
    int pos = 0;
    U8bit length_table_name;
    read_message(message, &pos, &length_table_name, DB_U_8_BIT_SIZE);

    read_message(message, &pos, table_name, length_table_name);
    table_name[length_table_name] = '\0';

    SERVER_TRACE(("SERVER:process_msg_search:pos=%d, length=%d, table_name=%s\n", pos, length_table_name, table_name));
    
    // Read columns
    U8bit num_col;
    read_message(message, &pos, &num_col, DB_U_8_BIT_SIZE);
    SERVER_TRACE(("SERVER:process_msg_search:pos=%d, num_col=%d\n", pos, num_col));
    
    for(i = 0; i < num_col; i++)
    {
        columns[i] = (U8bit *) malloc(DB_U_8_BIT_SIZE * DB_MAX_LENGTH_FIELD_NAME);
        // Read col_length
        U8bit col_length;
        read_message(message, &pos, &col_length, DB_U_8_BIT_SIZE);
        
        // String column
        read_message(message, &pos, columns[i], col_length);
        columns[i][col_length] = '\0';
        SERVER_TRACE(("SERVER:process_msg_search:pos=%d, columns[%d]=%s\n", pos, i, columns[i]));
    }

    /* Read condition */
    pos = process_msg_condition(message, pos, &cond);
    if(pos == -1)
    {
        SERVER_TRACE(("SERVER:process_msg_search:pos=%d\n", pos));
        return -1;
    }
    /* Search data in table */
    db_search_ret_t * ret = NULL;
    if(db_search(db, table_name, columns, num_col, &cond, &ret) == DB_FAILURE)
    {
        SERVER_TRACE(("SERVER:process_msg_search:search fail!\n"));
        return -1;
    }
    

    // Process return value
    int ret_pos = 0;
    U8bit num_ret, num_field; 
    U8bit ret_code = RET_SEARCH_CODE;
    
    if(ret == NULL)
    {
        // Process when not value return
        num_ret = 0;
        num_field = 0;
    }else
    {
        num_ret = ret[0].num_ret;
        num_field = ret[0].num_field;
    }
    write_message(ret_message, &ret_pos, &ret_code, DB_U_8_BIT_SIZE);
    write_message(ret_message, &ret_pos, &num_ret, DB_U_8_BIT_SIZE);
    write_message(ret_message, &ret_pos, &num_field, DB_U_8_BIT_SIZE);

    for(i = 0; i < num_ret; i++)
    {
        for(j = 0; j < num_field; j++)
        {
            // Write length_field
            U8bit length_field = strlen(ret[i].field_names[j]);
            write_message(ret_message, &ret_pos, &length_field, DB_U_8_BIT_SIZE);
            // Write field
            write_message(ret_message, &ret_pos, ret[i].field_names[j], length_field);
            // Write length_val
            U8bit length_val = strlen(ret[i].values[j]);
            write_message(ret_message, &ret_pos, &length_val, DB_U_8_BIT_SIZE);
            // Write val
            write_message(ret_message, &ret_pos, ret[i].values[j], length_val);
            free(ret[i].field_names[j]);
            free(ret[i].values[j]);
        }

        free(ret[i].field_names);
        free(ret[i].values);
    }
    
    U8bit end = '\0';
    write_message(ret_message, &ret_pos, &end, DB_U_8_BIT_SIZE);
    
    // Free mem
    for(i = 0; i < cond.num_cond; i++)
    {
        SERVER_TRACE(("SERVER:process_msg_search:Free field_condition[%d]!\n", i));
        free(cond.field_conditions[i]);
        SERVER_TRACE(("SERVER:process_msg_search:Free val_condition[%d]!\n", i));
        free(cond.val_conditions[i]);
    }

    for(i = 0; i < num_field; i++)
    {
        free(columns[i]);
    }
    free(ret);

    return ret_pos;
}

int process_msg_delete(U8bit * msg, U8bit * ret_msg)
{
    /* Declare variable */
    SERVER_TRACE(("SERVER:process_msg_delete:msg=%s\n", msg));
    int ret_pos = 0;
    int pos = 0;
    U8bit table_name[DB_MAX_LENGTH_TABLE_NAME];
    U8bit length_table_name = 0;
    db_condition_t cond;
    /* Read table name */
    // Read length of table name
    read_message(msg, &pos, &length_table_name, DB_U_8_BIT_SIZE);
    // Read table name
    read_message(msg, &pos, table_name, length_table_name);
    table_name[length_table_name] = '\0';
    SERVER_TRACE(("SERVER:process_msg_delete:table_name=%s\n", table_name));
    /* Read condition: using a different function */
    pos = process_msg_condition(msg, pos, &cond);
    if(pos == -1)
    {
        return -1;
    }

    if(db_delete(db, table_name, &cond) == DB_FAILURE)
    {
        SERVER_TRACE(("SERVER:process_msg_delete:Delete fail!"));
    }

    // Write return code
    U8bit ret_code = RET_DELETE_CODE;
    write_message(ret_msg, &ret_pos, &ret_code, DB_U_8_BIT_SIZE);
    U8bit * ret_info = "Delete successfull";
    U8bit length_ret_info = strlen(ret_info);
    // Write length of return info
    write_message(ret_msg, &ret_pos, &length_ret_info, DB_U_8_BIT_SIZE);
    // Write return info
    write_message(ret_msg, &ret_pos, ret_info, length_ret_info);
    return ret_pos;
}

int process_msg_update(U8bit * msg, U8bit * ret_msg)
{
    /* Parse message for update in db */
    int ret_pos = 0;


    return ret_pos;
}

int process_msg_insert(U8bit * msg, U8bit * ret_msg)
{
    /* Declare variable */
    int i;
    int ret_pos = 0;
    int pos = 0;
    U8bit table_name[DB_MAX_LENGTH_TABLE_NAME];
    U8bit * columns[DB_MAX_FIELDS_IN_TABLE];
    U8bit * values[DB_MAX_FIELDS_IN_TABLE];

    /* Get table name */
    // Read length of table name
    U8bit length_table_name;
    read_message(msg, &pos, &length_table_name, DB_U_8_BIT_SIZE);
    // Read table name
    read_message(msg, &pos, table_name, length_table_name);
    /* Columns */
    // Read num columns
    U8bit num_col;
    read_message(msg, &pos, &num_col, DB_U_8_BIT_SIZE);
    SERVER_TRACE(("SERVER:process_msg_insert:num_col=%d\n", num_col));
    for(i = 0; i < num_col; i++)
    {
        // Read length of column name
        U8bit length_col;
        read_message(msg, &pos, &length_col, DB_U_8_BIT_SIZE);
        // Read column name
        columns[i] = (U8bit *) malloc(DB_U_8_BIT_SIZE * (length_col + 1));
        read_message(msg, &pos, columns[i], length_col);
        columns[length_col] = '\0';
        SERVER_TRACE(("SERVER:process_msg_insert:columns[%d]=%s\n", i, columns[i]));
    }
    /* Get value */
    for(i = 0; i < num_col; i++)
    {
        // Read length of value
        U8bit length_val;
        read_message(msg, &pos, &length_val, DB_U_8_BIT_SIZE);
        // Read value
        values[i] = (U8bit *) malloc(DB_U_8_BIT_SIZE * (length_val + 1));
        read_message(msg, &pos, values[i], length_val);
        values[length_val] = '\0';
        SERVER_TRACE(("SERVER:process_msg_insert:values[%d]=%s\n", i, values[i]));
    }
    
    if(db_insert(db, table_name, columns, values, num_col) == DB_FAILURE)
    {
        SERVER_TRACE(("SERVER:process_msg_insert:Error insert!\n"));
        return -1;
    }
    // Free memory
    for(i = 0; i < num_col; i++)
    {
        free(columns[i]);
        free(values[i]);
    }
    // Write return value
    U8bit ret_code = RET_INSERT_CODE;
    write_message(ret_msg, &ret_pos, &ret_code, DB_U_8_BIT_SIZE);
    // Write message return
    U8bit length_ret_msg = strlen(RET_INSERT_STR);
    write_message(ret_msg, &ret_pos, &length_ret_msg, DB_U_8_BIT_SIZE);
    write_message(ret_msg, &ret_pos, RET_INSERT_STR, length_ret_msg);

    return ret_pos;
}

/* 
    FUNCTION : process_message
    DESCRIPTION:this function will process message
    PARAMS:     
    RETURN VALUE:
        0 if function fails
        1 if function successes

 */
int process_message(U8bit * message, U8bit * ret_buffer)
{
    /* Parse message for db */
    U8bit code = *(message);
    SERVER_TRACE(("SERVER:process_message: code = %d\n", (int) code));
    int ret_pos = 0;
    switch(code)
    {
        case SEARCH_CODE:
            ret_pos = process_msg_search(message + DB_U_8_BIT_SIZE, ret_buffer);
            break; 
        case INSERT_CODE:
            ret_pos = process_msg_insert(message + DB_U_8_BIT_SIZE, ret_buffer);
            break;
        case UPDATE_CODE:
            ret_pos = process_msg_update(message + DB_U_8_BIT_SIZE, ret_buffer);
            break;
        case DELETE_CODE:
            ret_pos = process_msg_delete(message + DB_U_8_BIT_SIZE, ret_buffer);
            break;
        default:
            ret_pos = -1;
            SERVER_TRACE(("SERVER:process_message:ERROR:cannot detect code\n"));
    }
    return ret_pos;
}