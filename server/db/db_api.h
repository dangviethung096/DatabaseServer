/* 
    File: db_api.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DP_API_H
#define DB_API_H


#include "db_struct.h"

DATABASE db_open(char *db_name, char *db_path);
void db_close(DATABASE db);

void db_search(DATABASE db, db_key_t key);
void db_insert(DATABASE db, db_key_t key, db_value_t value);
void db_modify(DATABASE db, db_key_t key, db_value_t value);
void db_delete(DATABASE db, db_key_t key);

#endif