#include <stdio.h>
#include "t_db_def.h"
#include <math.h>
#include <time.h>

char rand_num()
{
    return '0' + rand() % 10;
}

char rand_letter()
{
    return 'a' + rand() % 26;
}

int main()
{
    srand(time(NULL));
    // Declare variable
    int i, j, k;
    FILE * file = fopen("data.txt","wb");
    char * field[N_FIELD] = {"id", "name", "age", "class", "hobby"};
    
    // Write field
    for(i = 0; i < N_FIELD; i++)
    {
        fprintf(file, "%s ", field[i]);
    }
    fprintf(file, "\n");
    // Gen data
    for(i = 0; i < N_RECORD; i++)
    {
        
        for(j = 0; j < N_FIELD; j++)
        {   
            char value[MAX_VALUE];
            int length = rand() % (MAX_VALUE - MIN_VALUE) + MIN_VALUE;
            for(k = 0; k < length; k++)
            {
                if(j == 0)
                    value[k] = rand_num();
                else
                    value[k] = rand_letter();
            }
            value[length] = '\0';
            fprintf(file, "%s ", value);
        }

        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}