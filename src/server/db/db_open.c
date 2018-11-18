/* 
    File: db_def.h
    Author: Dang Viet Hung
    Description: 

    
 */
#include "db_struct.h"
#include "db_def.h"
#include "db_alloc.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

/* 
        Function: db_open_file
        Params: file,
                flag,
                mode
        Description: Open a database
        
 */
static inline int db_open_file(char *file,int flag, int mode)
{
    DB_TRACE(("file = %s\nflag = %d\nmode = %d\n", file, flag, mode));
    return open(file, flag, mode);
}
/* 
        Function: db_open
        Params: db_name,
                db_path,
                flag
        Description: Open a database
        
 */
DATABASE db_open(char *db_file, int flag)
{
    // Declare variable
    DATABASE db;
    int mode = S_IRWXU | S_IRWXG | S_IRWXO;
    int open_flag;
    int db_fd;

    // open database
    switch(flag)
    {
        case DB_OPEN:
            open_flag = O_RDWR;
            break;
        case DB_CREATE:
            open_flag = O_RDWR | O_CREAT ;
            break;
        default:
            open_flag = -1;
    }

    // Check flag param
    if(open_flag == -1)
    {
        DB_TRACE(("flag param is wrong!"));
        return NULL;
    }
    // Get descriptor in database
    db_fd = db_open_file(db_file, open_flag, mode);
    if(db_fd == -1)
    {
        DB_TRACE(("open file error!\n"));
        return NULL;
    }

    db = (DATABASE) db_alloc(DB_SIZE_DATABASE_TYPE);

    return db;
}