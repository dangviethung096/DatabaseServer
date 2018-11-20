#include <stdio.h>
#include "../server/db/db_def.h"
#include "../server/db/db_open.c"
#include "../server/db/db_close.c"

int main()
{
    DATABASE db = db_open("hello", DB_CREATE);
    if(db != NULL)
    {
        printf("Success!\n");
    }else{
        printf("Fail!\n");
    }
    db_close(db);
}