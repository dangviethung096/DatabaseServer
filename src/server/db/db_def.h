/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_DEF_H
#define DB_DEF_H

#include "db_def.h"

#define DB_MAX_LENGTH_DATABASE_PATH 50

#define DB_OPEN 0
#define DB_CREATE 1
#define DB_REMOVE 2

#define DB_SIZE_DATABASE_TYPE sizeof(struct db_file_info)

/* Start: Trace in database */
#ifdef DB_TRACE_ENABLE

#define DB_TRACE(str)  printf  str

#else

#define DB_TRACE(str) 

#endif
/* End: Trace in database */

/* Start: Define error code */
#define DB_NO_ERROR     0

/* End: Define error code */


/* Start: Error trace in database */
#ifdef  DB_ERROR_TRACE_ENABLE

#define DB_SET_ERROR(error_no) /
        {
            db_error_no = error_no  /
        }
#define DB_RESET_ERROR          /
        {                       /
            db_error_no = DB_NO_ERROR      /
        }           

#else

#define DB_RESET_ERROR
#define DB_SET_ERROR(error_no)

#endif
/* End: Error trace in database */

/* End DB_DEF_H */
#endif