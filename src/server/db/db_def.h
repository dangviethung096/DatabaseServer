/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_DEF_H

#define DB_DEF_H


/* Start: size of struct or type in db */
#define DB_OFF_T_SIZE                   sizeof(off_t)
#define DB_FLAG_T_SIZE                  sizeof(db_flag_t)
#define DB_INDEX_T_SIZE                 sizeof(db_index_t)
#define DB_TABLE_INFO_TYPE_SIZE         sizeof(db_table_info)
#define DB_U_8_BIT_SIZE                 sizeof(U8bit)
#define DB_U_16_BIT_SIZE                sizeof(U16bit)
#define DB_U_32_BIT_SIZE                sizeof(U32bit)
#define DB_TABLE_INFO_SIZE              sizeof(db_table_info)
#define DB_INT_SIZE                     sizeof(int)
#define DB_FIELD_INFO_SIZE              sizeof(db_field_t)         

#define DB_FIELD_IN_TABLE_SIZE          sizeof(struct db_field_in_table_data)
/* End: size of struct or type in db */



/* START: define max length, size, vv..v */
#define DB_MAX_LENGTH_DATABASE_PATH    256
#define DB_MAX_ROWS_IN_BUCKET          7         // It must be a prime number because algorithm hashing
#define DB_MAX_FIELDS_IN_TABLE         3         // It must be a prime number because algorithm hashing
#define DB_MAX_INDEX_FIELDS            4
#define DB_MAX_SIZE_IN_VALUE           32

#define DB_MAX_LENGTH_DB_NAME          32
#define DB_MAX_LENGTH_TABLE_NAME       32
#define DB_MAX_LENGTH_FIELD_NAME       32

#define DB_MAX_TABLE_IN_DATABASE       16

#define DB_MAX_CONDITION               5
/* END: define max length, size, vv..v */

/* START: define size */
#define DB_UNIT_SIZE_IN_FIELDS_BUCKET  sizeof(struct db_value)
#define DB_UNIT_SIZE_IN_ROWS_BUCKET    DB_OFF_T_SIZE

#define DB_ROW_SIZE_IN_ROW_BUCKET      sizeof(struct db_row_data)
#define DB_FIELD_SIZE_IN_FIELD_BUCKET  sizeof(struct db_field_data)

#define DB_ROWS_BUCKET_SIZE            sizeof(struct db_rows_bucket_data)
#define DB_FIELDS_BUCKET_SIZE          sizeof(struct db_fields_bucket_data)
#define DB_TABLE_INFO_DATA_SIZE        sizeof(struct db_table_info_data)

#define DB_SINGLE_TABLE_SIZE           DB_ROWS_BUCKET_SIZE + DB_FIELDS_BUCKET_SIZE + DB_TABLE_INFO_DATA_SIZE
/* END: define size */
            


#define DB_OPEN     0
#define DB_CREATE   1
#define DB_REMOVE   2


#define DB_SIZE_DATABASE_TYPE sizeof(struct db_file_info)
#define DB_SCHEMA_SIZE sizeof(db_schema);

#define DB_BEGIN_FD         SEEK_SET
#define DB_CURRENT_FD       SEEK_CUR
#define DB_END_FD           SEEK_END

#define DB_NULL ((void *)0)



/* Define for boolean type */
#define DB_FALSE            0
#define DB_TRUE             1

#define DB_FAILURE          0
#define DB_SUCCESS          1
/* Start: Trace in database */
#ifdef DB_TRACE_ENABLE

#define DB_TRACE(str)  printf  str

#else

#define DB_TRACE(str) 

#endif
/* End: Trace in database */

/* START: define position in database data*/
#define DB_POS_NAME_DATABASE            0
#define DB_POS_NUMBER_TABLE             DB_POS_NAME_DATABASE + DB_MAX_LENGTH_DB_NAME
#define DB_POS_FIRST_TABLE_POS          DB_POS_NUMBER_TABLE + DB_INT_SIZE
#define DB_POS_LAST_POSITION            DB_POS_FIRST_TABLE_POS + DB_MAX_TABLE_IN_DATABASE * DB_OFF_T_SIZE

/* END: define position in database info */


/* START: define position in table data */
#define DB_POS_ID_TABLE                  0
#define DB_POS_NAME_TABLE                DB_POS_ID_TABLE + DB_INT_SIZE
#define DB_POS_NUM_FIELD_IN_TABLE        DB_POS_NAME_TABLE   +   DB_MAX_LENGTH_TABLE_NAME
#define DB_POS_FIELDS_IN_TABLE           DB_POS_NUM_FIELD_IN_TABLE + DB_U_8_BIT_SIZE

#define DB_POS_TABLE_INFO_IN_TABLE      0
#define DB_POS_ROWS_BUCKET_IN_TABLE     DB_POS_TABLE_INFO_IN_TABLE + DB_TABLE_INFO_DATA_SIZE
#define DB_POS_FIELDS_BUCKET_IN_TABLE   DB_POS_ROWS_BUCKET_IN_TABLE + DB_ROWS_BUCKET_SIZE


// #define DB_POS_FIELD_ID_IN_FIELD_INFO   0
// #define DB_POS_NAME_IN_FIELD_INFO       DB_POS_FIELD_ID_IN_FIELD_INFO + DB_U_8_BIT_SIZE
// #define DB_POS_INDEX_IN_FIELD_INFO      DB_POS_NAME_IN_FIELD_INFO + DB_MAX_LENGTH_FIELD_NAME

#define DB_POS_FLAG_IN_FIELDS_BUCKET     0


#define DB_POS_FLAG_IN_ROWS_BUCKET       0

/* END: define position in table data */

/* START: define flag in db_field_in_table_data */
#define DB_FLAG_USED 0x01
#define DB_FLAG_NOT_USED 0x00

/* END: define flag in db_field_in_table_data */


/* Include struct */
#include "db_struct.h"
#include <unistd.h>
#include <sys/types.h>

/* End DB_DEF_H */
#endif