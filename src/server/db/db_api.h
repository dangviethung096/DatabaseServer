/* 
    File: db_api.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DP_API_H
#define DB_API_H


#include "db_struct.h"
#include "db_def.h"

DATABASE db_open(char *db_name, char *db_path, int flag);
db_table_info * db_create_table(DATABASE db, char *table_name, db_field_t * fields, int num_field);
db_boolean_t db_remove_table(DATABASE db, char *table_name);
db_boolean_t db_insert(DATABASE db, U8bit * table_name, U8bit * field_name[], U8bit * value[], int num_value);
void db_close(DATABASE db);
/* API design */
// 

#endif