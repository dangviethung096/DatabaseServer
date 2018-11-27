/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_DEF_H

#define DB_DEF_H


/* Start: size of struct or type in db */
#define DB_OFF_T_SIZE sizeof(off_t)
#define DB_TABLE_INFO_TYPE_SIZE sizeof(db_table_info)
#define DB_INT_SIZE sizeof(int)
#define DB_FIELD_IN_TABLE_SIZE sizeof(struct db_field_in_table_data)
/* End: size of struct or type in db */

/* START: define param type */

/* END: define param type */

/* Start: define return type */
#define DB_HASH_RETURN  long int
#define DB_HASH_PARAM

/* End: define return type */

#define DB_MAX_LENGTH_DATABASE_PATH    256
#define DB_MAX_ROWS_IN_BUCKET          4099         // It must be a prime number because algorithm hashing
#define DB_MAX_FIELDS_IN_TABLE         1031         // It must be a prime number because algorithm hashing
#define DB_MAX_INDEX_FIELDS            4
#define DB_MAX_SIZE_IN_VALUE           32

#define DB_ROWS_BUCKET_SIZE            DB_MAX_ROWS_IN_BUCKET * DB_MAX_FIELDS_IN_TABLE * DB_OFF_T_SIZE
#define DB_FIELDS_BUCKET_SIZE          DB_MAX_ROWS_IN_BUCKET * DB_MAX_FIELDS_IN_TABLE * DB_MAX_SIZE_IN_VALUE

#define DB_TABLE_INFO_SIZE             DB_TABLE_INFO_TYPE_SIZE - DB_OFF_T_SIZE

#define DB_SINGLE_TABLE_SIZE           DB_ROWS_BUCKET_SIZE + DB_FIELDS_BUCKET_SIZE + DB_TABLE_INFO_SIZE

            

#define DB_MAX_LENGTH_DB_NAME          32
#define DB_MAX_LENGTH_TABLE_NAME       32
#define DB_MAX_LENGTH_FIELD_NAME       32

#define DB_MAX_TABLE_IN_DATABASE       64

#define DB_OPEN     0
#define DB_CREATE   1
#define DB_REMOVE   2


#define DB_SIZE_DATABASE_TYPE sizeof(struct db_file_info)
#define DB_SCHEMA_SIZE sizeof(db_schema);

#define DB_BEGIN_FD         SEEK_SET
#define DB_CURRENT_FD       SEEK_CUR
#define DB_END_FD           SEEK_END

#define DB_NULL ((void *)0)

/* Start: Trace in database */
#ifdef DB_TRACE_ENABLE

#define DB_TRACE(str)  printf  str

#else

#define DB_TRACE(str) 

#endif
/* End: Trace in database */

/* Start: define position in database info*/
#define DB_POS_NAME_DATABASE    0
#define DB_POS_NUMBER_TABLE     DB_POS_NAME_DATABASE + DB_MAX_LENGTH_DB_NAME
#define DB_POS_FIRST_TABLE_POS        DB_POS_NUMBER_TABLE + DB_INT_SIZE            
/* End: define position in database info */

/* Start: define position in table info */
#define DB_POS_ID_TABLE         0
#define DB_POS_NAME_TABLE       DB_POS_ID_TABLE + DB_INT_SIZE
#define DB_POS_FIELDS_IN_TABLE  DB_POS_NAME_TABLE   +   DB_MAX_LENGTH_TABLE_NAME


/* End: define position in table info */

#include "db_struct.h"
#include <unistd.h>
#include <sys/types.h>

/* End DB_DEF_H */
#endif