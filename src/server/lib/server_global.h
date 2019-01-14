#ifndef SERVER_GLOBAL_H

#define SERVER_GLOBAL_H
#include "../db/db_struct.h"
#include <sys/epoll.h>

extern DATABASE db;
extern U8bit msg[MAX_LENGTH_MSG];
extern U8bit ret_msg[MAX_LENGTH_MSG];

extern struct epoll_event events[EPOLL_MAX_EVENT];
extern int server_fd, epoll_fd;
extern int isExit;
#endif