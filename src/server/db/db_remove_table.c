/* 
    File: db_remove_table.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_def.h"
#include "db_struct.h"
#include "db_core_funcs.h"
#include "db_global.h"
#include "db_error.h"
/* 
        Function: db_remove_table
        Params: db,
                table_name
        Description: remove a table out hardisk
        
 */
db_boolean_t db_remove_table(DATABASE db, char * table_name)
{
    int old_num_table = db->num_table;
    db_flag_t flag = DB_FLAG_NOT_USED;
    int new_last_position;
    ssize_t io_ret_val;
    int i;
    for(i = 0; i < old_num_table; i++)
    {
        if(db_strncmp(db->tables[i].table_name, table_name, db_strlen(table_name)) == 0)
        {
            // Remove table
            new_last_position = db->tables[i].position_table;
            flag = DB_FLAG_USED;
            break;
        }
    }

    if(flag == DB_FLAG_NOT_USED)
    {
        DB_SET_ERROR(DB_NOT_EXIST);
        return DB_FAILURE;
    }

    for(; i < (old_num_table - 1); i++)
    {
        // Get index i + 1
        off_t pos = db_get_position_index_table(db->fd, i+1);
        if(pos == -1)
        {
            return DB_FAILURE;
        }
        // Insert into index i
        if(db_set_position_index_table(db->fd, i, pos) == DB_FAILURE)
        {
            return DB_FAILURE;
        }
    }
    
    db->num_table--;
    db->last_position = new_last_position;
    return DB_SUCCESS;
}