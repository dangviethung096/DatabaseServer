
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