#ifndef SERVER_DEF_H

#define SERVER_DEF_H

#ifdef SERVER_ENABLE_TRACE

#define SERVER_TRACE(str)  printf str

#else

#define SERVER_TRACE(str)

#endif

/* START: Using for both server and client */
#define BUFFER_SIZE 1024

#define EPOLL_SIZE 15
#define EPOLL_MAX_EVENT EPOLL_SIZE
#define MAX_CHILDREN_PROCESS EPOLL_SIZE

#define FAILURE 0
#define SUCCESS 1

#define STRING_END_CONNECT "exit"
#define LENGTH_END_CONNECT strlen(STRING_END_CONNECT)


#define MSG_END             0

/* API code */


#define SEARCH_CODE         1
#define INSERT_CODE         2
#define UPDATE_CODE         3 
#define DELETE_CODE         4

#define RET_SEARCH_CODE     5
#define RET_INSERT_CODE     6
#define RET_UPDATE_CODE     7
#define RET_DELETE_CODE     8

#define COMMAND_CODE        10

#define ERROR_CODE          15

#define SEARCH_STR              "SELECT"
#define SEARCH_TABLE_STR        "FROM"
#define SEARCH_COND_STR         "WHERE"


#define INSERT_STR          "INSERT"
#define INSERT_VALUES_STR   "VALUES"
#define RET_INSERT_STR      "Insert success!"


#define UPDATE_STR          "UPDATE"
#define DELETE_STR          "DELETE"

#define MAX_LENGTH_MSG      BUFFER_SIZE

/* END: Using for both server and client */



#endif