/* 
    File: db_hash_function.h
    Author: Dang Viet Hung
    Description: 

    
 */



#ifndef DB_HASH_FUNCTION_H
#define  DB_HASH_FUNCTION_H

db_first_hash_ret_t db_first_hash(db_key_t key);
db_second_hash_ret_t db_second_hash(int hval2, int table_size, int index);

#endif



