/* 
    File: db_struct.h
    Author: Dang Viet Hung
    Description: 


 */
#ifndef DB_STRUCT_H

#define DB_STRUCT_H

#include <stdio.h>
#include "db_def.h"

typedef unsigned int    U32bit;
typedef unsigned char   U8bit;
typedef unsigned short  U16bit;
typedef unsigned char   db_flag_t;
typedef unsigned int    db_index_t;
typedef unsigned char   db_boolean_t;

typedef long int        db_first_hash_ret_t;
typedef int             db_second_hash_ret_t;

/* db_key_t: for hashing function */
typedef struct {
    U8bit * val;
    int size;
} db_key_t;


struct db_value
{
    // struct store data
    U8bit       size;
    U8bit       flag;
    U32bit      row_id;
    U8bit       value[26];                 //Using 26 bytes store data
};

typedef struct db_value db_value_field_t;

typedef struct {
    U8bit  field_id;
    char field_name[DB_MAX_LENGTH_FIELD_NAME];
    // If field is not using, index = -1
    int index;
} db_field_t;


typedef struct {
    U32bit id_table;
    char table_name[DB_MAX_LENGTH_TABLE_NAME];
    U8bit num_fields;
    db_field_t * fields;
    U32bit num_rows;
    off_t position_table;
} db_table_info;

struct db_file_info {
    // File descriptor connect to database in harddisk
    int fd;
    // Name of database
    char database_name[DB_MAX_LENGTH_DB_NAME];
    // Number table
    unsigned int num_table;
    unsigned int select_table;
    // Schema in tables
    db_table_info * tables;
    // This field will show position that can add new single table
    off_t last_position;
};


typedef struct db_file_info * DATABASE;

typedef U32bit db_return_t;

/* START:Define ERROR */
enum db_error_no
{
    DB_NO_ERROR = 0,    //0
    DB_READ_WRONG = 1,
    DB_WRITE_WRONG,     //2
    DB_SEEK_FD_FAIL,    //3
    DB_NULL_VALUE,      //4
    DB_NOT_EXIST,       //5
    DB_EXISTED,         //6
    DB_OUT_OF_BOUNDS = 11
};
/* END:Define ERROR */

/* Never using this structs in program
    This structs use for reference to data struct in disk */
/* Define struct data in hard drive */



struct db_field_in_table_data
{
    db_flag_t flag;
    U8bit field_name[DB_MAX_LENGTH_FIELD_NAME];
    db_index_t index;
};

/* size of db_first_row_data is sizeof(off_t) = 8 */
struct db_first_row_data
{
    db_flag_t   flag;
    db_index_t  row_id;
    U8bit       reserve[3];                 // 3 bytes for reserve
};
/* size of db_first_field_data is sizeof(off_t) = 8 */
struct db_first_field_data
{
    db_flag_t       flag;
    U8bit           field_id;
};

struct db_row_data
{
    off_t fields[DB_MAX_FIELDS_IN_TABLE];
};

struct db_field_data
{
    struct db_value value[DB_MAX_ROWS_IN_BUCKET + 1];
};


struct db_fields_bucket_data
{
    struct db_field_data fields[DB_MAX_FIELDS_IN_TABLE + 1];
};

struct db_rows_bucket_data
{
    struct db_row_data rows[DB_MAX_ROWS_IN_BUCKET];
};

struct db_table_info_data
{
    U32bit id_table;
    U8bit table_name[DB_MAX_LENGTH_TABLE_NAME];
    U8bit num_field;
    db_field_t fields[DB_MAX_FIELDS_IN_TABLE];
};

struct db_table_data
{
    struct db_table_info_data           table_info;
    struct db_rows_bucket_data          rows_bucket;  
    struct db_fields_bucket_data        fields_bucket;
};

struct db_database_data
{
    U8bit   database_name[DB_MAX_LENGTH_DB_NAME];
    U32bit  num_table;
    off_t   tables[DB_MAX_TABLE_IN_DATABASE];
    off_t   last_position;
};



/* End DB_STRUCT_H */
#endif
