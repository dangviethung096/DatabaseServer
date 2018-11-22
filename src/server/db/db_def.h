/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_DEF_H
#define DB_DEF_H

#include <unistd.h>
#include <sys/types.h>
#include "db_struct.h"

#define DB_OFF_T_SIZE sizeof(off_t)
#define DB_TABLE_INFO_SIZE sizeof(db_table_info)
#define DB_INT_SIZE sizeof(int)

#define DB_MAX_LENGTH_DATABASE_PATH 256
#define MAX_ROWS_IN_TABLE 4096
#define MAX_FIELDS_IN_TABLE 1024
#define MAX_INDEX_FIELDS 4

#define MAX_LENGTH_DB_NAME 32

#define DB_OPEN 0
#define DB_CREATE 1
#define DB_REMOVE 2


#define DB_SIZE_DATABASE_TYPE sizeof(struct db_file_info)
#define DB_SCHEMA_SIZE sizeof(db_schema);

#define DB_BEGIN_FD SEEK_SET
#define DB_CURRENT_FD SEEK_CUR
#define DB_END_FD           SEEK_END

#define DB_NULL ((void *)0)

/* Start: Trace in database */
#ifdef DB_TRACE_ENABLE

#define DB_TRACE(str)  printf  str

#else

#define DB_TRACE(str) 

#endif
/* End: Trace in database */

/* Start: define position in database */
#define DB_POS_NAME_DATABASE 0
#define DB_POS_NUMBER_TABLE         \
            DB_POS_NAME_DATABASE + MAX_LENGTH_DB_NAME
#define DB_POS_TABLE_POS            \
            DB_POS_NUMBER_TABLE + DB_INT_SIZE            
/* End: define position in database  */

/* End DB_DEF_H */
#endif