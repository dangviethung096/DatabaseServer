/* 
    File: db_core_funcs.h
    Author: Dang Viet Hung
    Description: 

    
 */

#ifndef DB_CORE_FUNCS_H

#define DB_CORE_FUNCS_H

#include <unistd.h>
#include <sys/types.h>
#include <string.h>


#define db_strlen(str)                      \
            strlen((const char *) str)

#define db_length_str(str)                  \
            (strlen((const char *) str) + 1)

long int db_find_nearest_prime_number(long int input);
#define DB_FIND_INT_PRIME_NUMBER(input)            \
            (int) db_find_nearest_prime_number((long int) input)

db_boolean_t db_is_row_in_rows_bucket_used(int fd, off_t table_pos, db_index_t index);
off_t db_point_to_rows_bucket_by_index(int fd, off_t table_pos, db_index_t index);
db_boolean_t db_get_flag_in_rows_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t * flag);
db_boolean_t db_set_flag_in_rows_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t flag);

db_boolean_t db_is_field_in_fields_bucket_used(int fd, off_t table_pos, db_index_t index);
off_t db_point_to_fields_bucket_by_index(int fd, off_t table_pos, db_index_t index);
db_boolean_t db_get_flag_in_fields_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t *flag);
db_boolean_t db_set_flag_in_fields_bucket(int fd, off_t table_pos, db_index_t index, db_flag_t flag);

off_t db_point_to_table_info_in_db_by_index(int fd, int index);
off_t db_get_position_index_table(int fd, int index);
db_boolean_t db_set_position_index_table(int fd, int index, off_t pos);

off_t db_get_last_position(int fd);
db_boolean_t db_set_last_position(int fd, off_t last_position);

unsigned int db_get_num_table(int fd);
db_boolean_t db_set_num_table(int fd, unsigned int num_table);

db_boolean_t db_set_database_name(int fd, char *database_name);
db_boolean_t db_get_database_name(int fd, char *database_name);

db_field_t db_get_field_info_in_table_by_index(int fd, off_t table_pos, int index);
db_boolean_t db_set_index_field_info_in_table(int fd, off_t table_pos, int index, db_field_t field);

int db_get_index_field_in_fields_bucket_by_field_name(int fd, db_table_info *table, U8bit * field_name);
off_t db_point_to_fields_bucket_by_field_name(int fd, db_table_info *table, U8bit * field_name);
int db_get_index_table_from_table_name(DATABASE db, U8bit * table_name);

off_t db_point_to_fields_bucket_by_value_index(int fd, off_t fields_pos, db_index_t value_index);
db_boolean_t db_get_value_in_fields_bucket(int fd, off_t field_pos, db_index_t value_index, db_value_field_t *value);
db_boolean_t db_is_value_in_field_bucket_used(int fd, off_t field_pos, db_index_t value_index);
db_boolean_t db_set_value_in_fields_bucket(int fd, off_t field_pos, db_index_t value_index, struct db_value value);

int db_get_empty_index_field_in_fields_bucket_by_field_name(int fd, off_t table_pos, U8bit *field_name);

off_t db_get_value_pos_from_rows_bucket_by_field_index(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index);
db_boolean_t db_set_value_pos_in_rows_bucket_by_field_index(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index, off_t val_pos);

U32bit db_get_num_row_in_table(int fd, off_t table_pos);
db_boolean_t db_set_num_row_in_table(int fd, off_t table_pos, U32bit num_row);

db_boolean_t db_get_value_in_fields_bucket_by_value_pos(int fd, off_t val_pos, db_value_field_t *value);

db_boolean_t db_is_value_in_row_bucket_used(int fd, off_t table_pos, int row_index, int field_index);
db_boolean_t db_is_value_in_field_bucket_used_with_field_index(int fd, off_t table_pos, int field_index, db_index_t value_index);

db_boolean_t db_get_value_in_fields_bucket_with_field_index(int fd, off_t table_pos, db_index_t field_index, db_index_t value_index, db_value_field_t *value);

db_boolean_t db_search_row_with_equal_condition(int fd, off_t table_pos, int field_index, U8bit * value, int * row_ids, int * num_row);

db_boolean_t db_set_value_in_fields_bucket_by_field_index(int fd, off_t table_pos, db_index_t field_index, db_index_t value_index, db_value_field_t value);

int db_get_empty_value_in_field_of_fields_bucket(int fd, off_t table_pos, int field_index, U8bit * value);

db_boolean_t db_remove_value_in_field_bucket(int fd, off_t table_pos, int field_index, int value_index);
db_boolean_t db_remove_value_pos_in_rows_bucket(int fd, off_t table_pos, db_index_t row_index, db_index_t field_index);

db_boolean_t db_remove_row_in_rows_bucket(int fd, off_t table_pos, db_index_t row_index);
int db_get_value_index_from_rows_bucket(int fd, int table_pos, int row_index, int field_index);

db_boolean_t db_rehash_field_bucket_by_index(int fd, off_t table_pos, int field_index);

/* START: function with string */
#define db_strncmp(str1, str2, num)                         \
            strncmp((const char *) str1,(const char *) str2,(size_t) num)

/* END: function with string */

/* Start: Functions with file descriptor */
#define db_seek(fd, off_set, whence)        \
            lseek(fd, off_set, whence)

#define db_write(fd, buffer, num)           \
            write(fd,(const void *) buffer,(size_t) num)

#define db_read(fd, buffer, num)           \
            read(fd,(void *) buffer,(size_t) num)

/* End : Functions with file descriptor */

/* End DB_CORE_FUNCS.H */
#endif