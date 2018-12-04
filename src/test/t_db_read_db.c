#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include "../server/db/db_api.h"

int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_OPEN);
    
    if(db != NULL)
    {
        printf("Success!\n");
    }else{
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }

    printf("Database name: %s\n", db->database_name);
    printf("FD: %d\n", db->fd);
    printf("Last position: %ld\n", db->last_position);
    printf("================== List tables =================\n");
    int i,j;
    for(i = 0; i < db->num_table; i++)
    {
        printf("Table %d: %ld\n", i, db->tables[i].position_table);
        for(j = 0; j < db->tables[i].num_fields; j++)
        {
            
        }
    }
    /* Show num_table */
    db_close(db);
    return 1;
}