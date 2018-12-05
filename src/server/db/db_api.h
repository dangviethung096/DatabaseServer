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
db_table_info * db_create_table(DATABASE db, char *table_name, db_field * fields, int num_field);
void db_close(DATABASE db);
/* API design */
// Remove table
// Insert data
// 

#endif