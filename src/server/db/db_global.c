/* 
    File: db_global.c
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"


/* Start: define some values in error */
enum db_error_no db_error_no;

const char *db_error_str[] = {
    "DB_NO_ERROR",      //0
    "DB_READ_WRONG",    //1
    "DB_WRITE_WRONG",   //2
    "DB_SEEK_FD_FAIL",  //3
    "DB_NULL_VALUE",    //4
    "DB_NOT_EXIST",     //5
    "DB_EXISTED",       //6
    "DB_ERROR_FULL",            //7
    "DB_ERROR_NOT_ENOUGH",      //8
    "DB_ERROR_WRONG_PARAM",     //9
    "",                 //10
    "DB_OUT_OF_BOUNDS", //11
};
/* End: define some values in error */