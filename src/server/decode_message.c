#include "lib/server_struct.h"
#include "lib/server_def.h"
#include "db/db_def.h"
#include "db/db_api.h"
#include <stdlib.h>
#include "server_global.h"



void decode_msg_search(U8bit * buffer)
{
    int i;
    db_search_ret_t * ret;
    /* Parse message for search in db */
    int pos = 0;
    /* Read table */
    int length_table_name = *((int *) (buffer + pos));
    pos += DB_INT_SIZE;

    char * table_name = (char *) calloc(DB_MAX_LENGTH_TABLE_NAME, DB_U_8_BIT_SIZE);
    

    memcpy(table_name, buffer + pos, length_table_name);
    pos += length_table_name;
    table_name[length_table_name] = '\0';
    /* Read field */
    int num_field = *((int *) (buffer + pos));
    U8bit ** field_names;
    field_names = (U8bit **) calloc(num_field, sizeof(U8bit *));

    pos += DB_INT_SIZE;
    for(i = 0; i < num_field; i++)
    {
        int length_field_name = *((int *) (buffer + pos));
        pos += DB_INT_SIZE;
        
        memcpy(table_name, buffer + pos, length_field_name);
        pos += length_field_name;
    }

    /* Read condition */
    db_condition_t cond;
    int num_cond = *((int *) (buffer + pos));
    pos +=  DB_INT_SIZE;
    cond.num_cond = num_cond;

    for(i = 0; i < num_cond; i++)
    {
        int length_field_cond_name = *((int *) (buffer + pos));
        pos += DB_INT_SIZE;
        cond.field_conditions[i] = (U8bit *) calloc(DB_MAX_LENGTH_FIELD_NAME, DB_U_8_BIT_SIZE);
        
        memcpy(cond.field_conditions[i], buffer + pos, length_field_cond_name);
        pos += length_field_cond_name;

        int length_val_cond = *((int *) (buffer + pos));
        pos += DB_INT_SIZE;

        cond.val_conditions[i] = (U8bit *) calloc(DB_MAX_SIZE_IN_VALUE, DB_U_8_BIT_SIZE);
        memcpy(cond.val_conditions[i], buffer + pos, length_val_cond);
        pos += length_val_cond;

    }

    printf("SERVER:search:\n");
    for(i = 0; i < ret->num_ret; i++)
    {
        SERVER_TRACE(("%s = %s\n", ret->field_names[i], ret->values[i]));
    }
    
}

void decode_msg_delete(U8bit * buffer)
{
    /* Parse message for delete in db */
}

void decode_msg_update(U8bit * buffer)
{
    /* Parse message for update in db */
}

void decode_msg_insert(U8bit * buffer)
{
    int i;
    /* Parse message for insert in db */
    int pos = 0;
    /* Read table */
    int length_table_name = *((int *) (buffer + pos));
    pos += DB_INT_SIZE;

    char * table_name = (char *) calloc(DB_MAX_LENGTH_TABLE_NAME, DB_U_8_BIT_SIZE);
    

    memcpy(table_name, buffer + pos, length_table_name);
    pos += length_table_name;
    table_name[length_table_name] = '\0';
    /* Read field */
    int num_field = *((int *) (buffer + pos));
    U8bit ** field_names;
    field_names = (U8bit **) calloc(num_field, sizeof(U8bit *));

    pos += DB_INT_SIZE;
    for(i = 0; i < num_field; i++)
    {
        int length_field_name = *((int *) (buffer + pos));
        pos += DB_INT_SIZE;
        
        memcpy(table_name, buffer + pos, length_field_name);
        pos += length_field_name;
    }

    int num_value = *((int *) (buffer + pos));
    pos += DB_INT_SIZE;
    U8bit ** values = (U8bit **) calloc(num_value, sizeof(U8bit *));

    for(i = 0; i < num_value; i++)
    {
        int length_value = *((int *) (buffer + pos));
        pos += DB_INT_SIZE;
        values[i] = (U8bit *) calloc(DB_MAX_SIZE_IN_VALUE, sizeof(U8bit));
        memcpy(values[i], buffer + pos, length_value);
        values[length_value] = '\0' ;
        pos += length_value;
    }

    if(db_insert(db, table_name, field_names, values, num_value) == DB_FAILURE)
    {
        return;
    }

    
}

void decode_message(U8bit * buffer)
{
    /* Parse message for db */
    command_code_t code = *((command_code_t *) buffer);
    SERVER_TRACE(("SERVER:decode_message: code = %d\n", (int) code));
    switch(code)
    {
        case SEARCH_CODE:
            decode_msg_search(buffer + COMMAND_CODE_SIZE);
            break; 
        case INSERT_CODE:
            decode_msg_insert(buffer + COMMAND_CODE_SIZE);
            break;
        case UPDATE_CODE:
            decode_msg_update(buffer + COMMAND_CODE_SIZE);
            break;
        case DELETE_CODE:
            decode_msg_delete(buffer + COMMAND_CODE_SIZE);
            break;
        default:
            SERVER_TRACE(("SERVER:decode_message:ERROR: cannot detect code\n"));
    }
}