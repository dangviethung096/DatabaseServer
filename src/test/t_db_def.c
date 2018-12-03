#include <stdio.h>
#include "../server/db/db_def.h"

int main()
{
    printf("Size in a single table: %ld\n", DB_SINGLE_TABLE_SIZE);
    printf("Size in a table info data: %ld\n", DB_TABLE_DATA_SIZE);
    printf("Size in a fields bucket %ld\n", DB_FIELDS_BUCKET_SIZE);
    printf("Size in a rows bucket %ld\n", DB_ROWS_BUCKET_SIZE);
}