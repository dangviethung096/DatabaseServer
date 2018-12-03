/* 
    File: hash_function.c
    Author: Dang Viet Hung
    Description: This file contains all hash function in database
    In this database, it uses double hashing for hash function

    
 */
#include "db_def.h"
#include "db_struct.h"
/* 
    Function: db_first_hash
    Params: key,
            
    Description: this is first hash function in double hasing
        
 */
db_first_hash_ret_t db_first_hash(db_key_t key)
{
    long int hval = key.size;
    
    int i;
    for(i = key.size; i > 0; i--)
    {
        hval <<= 4;
        hval += key.val[i];
    }

    if(hval == 0)
        hval += 1;

    DB_TRACE(("DB:db_first_hash: hval = %ld\n", hval));
    return hval;
}
/* 
    Function: db_second_hash
    Params: hval2: hval2 = 1 + hval % (table_size - 2),
            table_size,
            index
            
    Description: this is second has in hash function
        
 */
db_second_hash_ret_t db_second_hash(int hval2, int table_size, int index)
{
    if(index <= hval2) 
        index = table_size + index - hval2;
    else
        index -= hval2;

    DB_TRACE(("DB:db_second_hash: index = %d\n", index));
    return index;
}