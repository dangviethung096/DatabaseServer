/* 
    File: db_core_funcs.h
    Author: Dang Viet Hung
    Description: 

    
 */

/* 
    Function: find_nearest_prime_number
    Params: input,
            
    Description: find nearest prime number is greater than input
        
 */
static int is_prime(long int input)
{
    int index;
    for(index = 2; index < input; index++)
    {
        if(!(input % index))
        {
            return 0;
        }
    }
    return 1;
}

/* 
    Function: find_nearest_prime_number
    Params: input,
            
    Description: find nearest prime number is greater than input
        
 */
long int db_find_nearest_prime_number(long int input)
{
    long int output = input;
    // Check number is greater or equal than input
    while(!is_prime(output))
        output++;
    
    return output;
}