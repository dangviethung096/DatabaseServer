#include <stdio.h>
#include "../server/db/db_def.h"

int main()
{
    printf("Max rows : %d\nMax fields: %d\n", DB_MAX_ROWS_IN_BUCKET, DB_MAX_FIELDS_IN_TABLE);
}