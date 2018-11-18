/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include <stdlib.h>

void db_close(DATABASE db)
{
    DB_TRACE(("Close db!\n"));
    free(db);
}