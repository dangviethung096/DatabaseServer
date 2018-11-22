/* 
    File: db_struct.h
    Author: Dang Viet Hung
    Description: 


 */
#ifndef DB_STRUCT_H
#define DB_STRUCT_H

#include <stdio.h>
#include "db_def.h"

typedef unsigned int U32bit;
typedef unsigned char U8bit;

typedef struct {
    char *val;
    int size;
} db_key_t;


typedef db_key_t db_value_t;

typedef struct {
    char * field;
    int index;
}db_field;

typedef struct {
    db_value_t * fields;
}hash_row;


typedef struct {
    U32bit id_table;
    char * table_name;
    db_field fields;
    U32bit num_rows;
    off_t position_table;
} db_table_info;

struct db_file_info {
    // File descriptor connect to database in harddisk
    int fd;
    // Name of database
    char *database_name;
    // Number table
    unsigned int num_table;
    // Schema in tables
    db_table_info * tables;
};


typedef struct db_file_info * DATABASE;

typedef U32bit db_return_t;

#endif
