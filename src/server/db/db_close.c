/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include "db_alloc.h"
#include "db_global.h"
#include <stdlib.h>

void db_close(DATABASE db)
{
    DB_TRACE(("DB:db_close: Close db!\n"));
    // Mem in database_name
    
    DB_TRACE(("DB:db_close: Free tables!\n"));
    db_free(db->tables);
    // Close file descriptor
    close(db->fd);
    // Mem in database info
    DB_TRACE(("DB:db_close: Free db!\n"));
    db_free(db);
}