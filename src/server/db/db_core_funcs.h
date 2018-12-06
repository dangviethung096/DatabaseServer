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

long int db_find_nearest_prime_number(long int input);
#define DB_FIND_INT_PRIME_NUMBER(input)            \
            (int) db_find_nearest_prime_number((long int) input)

db_boolean_t db_is_row_in_rows_bucket_used(int fd, off_t pos_rows_bucket, db_index_t index);
off_t db_point_to_index_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index);
db_boolean_t db_get_flag_in_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index, db_flag_t * flag);
db_boolean_t db_set_flag_in_rows_bucket(int fd, off_t pos_rows_bucket, db_index_t index, db_flag_t flag);

db_boolean_t db_is_field_in_fields_bucket_used(int fd, off_t pos_fields_bucket, db_index_t index);
off_t db_point_to_index_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index);
db_boolean_t db_get_flag_in_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index, db_flag_t * flag);
db_boolean_t db_set_flag_in_fields_bucket(int fd, off_t pos_fields_bucket, db_index_t index, db_flag_t flag);

off_t db_point_to_index_table_info_in_db(int fd, int index);
off_t db_get_position_index_table(int fd, int index);
db_boolean_t db_set_position_index_table(int fd, int index, off_t pos);

off_t db_get_last_position(int fd);
db_boolean_t db_set_last_position(int fd, off_t last_position);

unsigned int db_get_num_table(int fd);
db_boolean_t db_set_num_table(int fd, unsigned int num_table);

db_boolean_t db_set_database_name(int fd, char *database_name);
db_boolean_t db_get_database_name(int fd, char *database_name);

db_field_t db_get_index_field_info_in_table(int fd, off_t table_pos, int index);
db_boolean_t db_set_index_field_info_in_table(int fd, off_t table_pos, int index, db_field_t field);

/* START: function with string */
#define db_strncmp(str1, str2, num)                         \
            strncmp(str1, str2,(size_t) num)

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