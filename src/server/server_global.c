#include "db/db_struct.h"
#include "lib/server_def.h"
#include "lib/server_struct.h"
#include <sys/epoll.h>



/* Global variables */
DATABASE db;
U8bit msg[MAX_LENGTH_MSG];
U8bit ret_msg[MAX_LENGTH_MSG];

struct epoll_event events[EPOLL_MAX_EVENT];
int server_fd, epoll_fd;
int isExit = 0;