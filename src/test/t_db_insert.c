#include <stdio.h>
#include "../server/db/db_api.h"
#include "../server/db/db_struct.h"
#include "../server/db/db_global.h"
#include <string.h>
#include <stdlib.h>
#include "t_db_const.h"



int main()
{
    DATABASE db = db_open("test_database", "../data/", DB_OPEN);

    if (db != NULL)
    {
        printf("Success!\n");
    }
    else
    {
        DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
        printf("Fail!\n");
        return 0;
    }

    // Insert field to database
    {
        U8bit * field_name[NUM_COLS];
        U8bit ** value[NUM_ROWS];
        FILE * file = fopen("../data/data.txt", "rb");
        int i, j;
        for(i = 0; i < NUM_COLS; i++)
        {
            field_name[i] = (U8bit *) malloc(MAX_VALUE * DB_U_8_BIT_SIZE);
            fscanf(file, "%s ", field_name[i]);
            printf("field_name = %s\n", field_name[i]);
        }
        fscanf(file, "\n");

        for(i = 0; i < NUM_ROWS; i++)
        {
            value[i] = (U8bit **) malloc(sizeof(U8bit *) * NUM_COLS);
            for(j = 0; j < NUM_COLS; j++)
            {
                value[i][j] = (U8bit *) malloc(DB_U_8_BIT_SIZE * MAX_VALUE);
                fscanf(file, "%s ", value[i][j]);
            }
            fscanf(file, "\n");
        }
        fclose(file);
        for(i = 0; i < NUM_ROWS; i++)
        {
            printf("Insert %d\n", i);
            if(db_insert(db, "student", field_name, value[i], NUM_COLS) == DB_SUCCESS)
            {
                printf("Success!\n");
            }else
            {
                printf("Fail\n");
                DB_TRACE(("Error_no: %s\n", db_error_str[db_error_no]));
            }
        }
        
    }

    db_close(db);
    return 1;
}