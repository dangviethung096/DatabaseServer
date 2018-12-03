/* 
    File: db_error.h
    Author: Dang Viet Hung
    Description: 

    
 */
#ifndef DB_ERROR_H
#define DB_ERROR_H

#include "db_def.h"
#include "db_struct.h"
#include "db_global.h"


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