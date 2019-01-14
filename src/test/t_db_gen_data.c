#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "t_db_const.h"

char gen_num()
{
    return (rand() % 10) + '0';
}

char gen_lower_case()
{
    return (rand()%26 ) + 'a';
}

char gen_upper_case()
{
    return (rand()%26) + 'A';
}
int main()
{
    srand(time(NULL));
    int i, j, k;
    FILE * file = fopen("../data/data.txt", "wb");
    char * field_name[NUM_COLS] = 
            {"id", "name", "age", "class", "school", "status", "favorite", "detail", "home_land", "address"};
    char * data[NUM_COLS];
    printf("malloc\n");
    for(i = 0; i < NUM_COLS; i++)
    {
        data[i] = (char *) malloc(MAX_VALUE);
        // Insert field_name
        fprintf(file, "%s ", field_name[i]);
    }
    fprintf(file, "\n");
    printf("Gen data\n");
    for(i = 0; i < NUM_ROWS; i++)
    {
        for(j = 0; j < NUM_COLS; j++)
        {
            // Gen rows
            int size = (rand() % (MAX_VALUE - 16)) + 1;
            for(k = 0; k < size; k++)
            {
                if(j == 0)
                {
                    data[j][k] = gen_num();
                }else
                {
                    data[j][k] = gen_lower_case();
                }
            }

            printf("i = %d, j = %d, k = %d, size = %d\n", i, j, k, size);
            data[j][size] = '\0';
            fprintf(file, "%s ", data[j]);
            
        }
        fprintf(file, "\n");
    }
    printf("Close\n");
    fclose(file);
    return 1;
}