#ifndef DB_ALLOC_H

#define DB_ALLOC_H

#include <stdlib.h>

#define db_alloc(size)      \
            malloc((size_t) size)

#define db_free(ptr)        \
            free((void *) ptr)

#endif