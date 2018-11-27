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
    // End data
    off_t last_position;
};


typedef struct db_file_info * DATABASE;

typedef U32bit db_return_t;


/* Define struct data in hard drive */
struct db_field_in_table_data
{
    char field[DB_MAX_LENGTH_FIELD_NAME];
};

struct db_table_data
{
    U32bit id_table;
    char table_name[DB_MAX_LENGTH_TABLE_NAME];
    db_field fields[DB_MAX_FIELDS_IN_TABLE];  
};

struct db_database_data
{
    char database_name[DB_MAX_LENGTH_DB_NAME];
};

/* End DB_STRUCT_H */
#endif
