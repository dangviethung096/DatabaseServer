/* 
    File: db_struct.h
    Author: Dang Viet Hung
    Description: 


 */
#ifndef DB_STRUCT_H
#define DB_STRUCT_H
#include <stdio.h>

typedef struct {
    
}hash_table;

struct db_file_info {
    char *name;
    int fd;
    hash_table *table;
};

typedef struct db_file_info * DATABASE;

typedef struct {
    char *val;
    int size;
} db_key_t;


typedef db_key_t db_value_t;

typedef unsigned int U32bit;
typedef unsigned char U8bit;

typedef U32bit db_return_t;

#endif
