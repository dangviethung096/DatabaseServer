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

/* 
    Function: db_hash_function
    Params: value,
            hval,
            table_size,
            num_hash,
            index
    Description: search new memory for store value in fields bucket
    Return value: DB_FALSE if value is not used
                  DB_TRUE if value is used
    Caution: this function change position of fd. 
             So after call this function, seek to old position
*/
void db_hash_function(char *value, db_first_hash_ret_t *hval, int table_size, int *num_hash, int *index)
{
    if (*num_hash == 0)
    {
        db_key_t key;
        key.val = value;
        key.size = db_strlen(value);
        *hval = db_first_hash(key);
        *index = (*hval) % table_size + 1;
        num_hash++;
        DB_TRACE(("DB:db_hash_function:first_hash: hval = %ld, index = %d\n", hval, *index));
    }
    else
    {
        unsigned int hval2 = 1 + (*hval) % (table_size - 2);
        *index = db_second_hash(hval2, table_size, *index);
        DB_TRACE(("DB:db_hash_function:second_hash: hval2 = %u, index = %d\n", hval2, *index));
    }
    return;
}