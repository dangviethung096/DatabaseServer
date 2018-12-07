/* 
    File: db_insert.c
    Author: Dang Viet Hung
    Description: 

    
 */

#include "db_def.h"
#include "db_struct.h"
#include "db_core_funcs.h"

db_boolean_t db_insert_field(char * field_name, db_value_field_t value)
{
    // Hash field
    db_key_t field_key;
    field_key.val = (U8bit *) field_name;
    field_key.size = db_strlen(field_name);
    
}