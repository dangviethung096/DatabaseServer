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
    int i, j;
    for(i = 0; i < db->num_table; i++)
    {
        DB_TRACE(("DB:db_close: Free table name %d!\n", i));
        db_free(db->tables[i].table_name);
        // Free fields
        for(j = 0; j < DB_MAX_FIELDS_IN_TABLE; j++)
        {
            if (db->tables[i].fields[j].index != -1)
            {
                DB_TRACE(("DB:db_close: Free field name index %d in table %d!\n", j, i));
                db_free(db->tables[i].fields[j].field_name);
            }
                
        }
    }
    DB_TRACE(("DB:db_close: Free tables!\n"));
    db_free(db->tables);
    db_free(db->database_name);
    // Close file descriptor
    close(db->fd);
    // Mem in database info
    db_free(db);
}