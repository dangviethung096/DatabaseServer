/* 
    File: db_search.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_struct.h"
#include "db_def.h"
#include "db_core_funcs.h"

/* 
    Function: db_search
    Params: 
    Description: search by using field_name with some condition or not
                 if field_name == NULL then search all field in table
    Return value: DB_FAILURE if error
                  DB_SUCCESS if success
    Caution: this function change position of fd. 
             So after call this function, seek to old position
 */
db_boolean_t db_search(DATABASE db, U8bit * table_name, U8bit * field_name[], int num_field, db_condition_t * cond, db_search_ret_t * ret)
{
    int table_index = db_get_index_table_from_table_name(db, table_name);
    int field_index;
    int count_value = 0;
    db_table_info * table = &(db->tables[table_index]);
    /* Search field_name */
    int i, j;
    
    /* Search all value */
    ret = (db_search_ret_t *) malloc(table->num_rows);
    for(i = 0; i < table->num_rows; i++)
    {
        if(db_is_row_in_rows_bucket_used(db->fd, table->position_table, i) == DB_TRUE)
        {
            
        }
    }
    


    /* Choose value from condition */

}
