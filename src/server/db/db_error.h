/* 
    File: db_error.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_ERROR_H
#define DB_ERROR_H

#include "db_def.h"
#include "db_struct.h"

/* Start: define some values in error */
enum db_error_no {
    DB_READ_WRONG       = 1,
    DB_WRITE_WRONG,             //2
    DB_SEEK_FD_FAIL,            //3
    DB_OUT_OF_BOUNDS    =11
} db_error_no;


const char * db_error_str[] = {
    "",                         //0
    "DB_READ_WRONG",            //1
    "DB_WRITE_WRONG",           //2
    "DB_SEEK_FD_FAIL",          //3
    "",                         //4
    "",                         //5
    "",                         //6
    "",                         //7
    "",                         //8
    "",                         //9
    "",                         //10
    "DB_OUT_OF_BOUNDS",         //11
};
/* End: define some values in error */

/* Start: Define error code */
#define DB_NO_ERROR     0

/* End: Define error code */

/* Start: Error trace in database */
#ifdef  DB_ERROR_TRACE_ENABLE

#define DB_SET_ERROR(error_no)          \
            db_error_no = error_no
#define DB_RESET_ERROR()                 \
            db_error_no = DB_NO_ERROR 
#else

#define DB_RESET_ERROR()    
#define DB_SET_ERROR(error_no)

#endif
/* End: Error trace in database */




#endif