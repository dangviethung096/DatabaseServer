#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "lib/server_def.h"
#include "lib/server_global.h"
#include "db/db_api.h"
#include "lib/process_message.h"




/* 
    FUNCTION : start_new_process
    DESCRIPTION:
    PARAMS:     
    RETURN VALUE:

 */

int start_new_process(char * command, int client_fd)
{
    // using fork to make a new process
    pid_t child_proc = fork();

    if(child_proc == 0)         //child process
    {
        // char real_command[BUFFER_SIZE + 10];
        // sprintf(real_command, "%s > log_%d.txt", command, client_fd);
        // printf("Client input command: %s", real_command);
        // system(real_command);
        execl("/bin/sh", "sh", "-c", command, NULL);
        exit(EXIT_SUCCESS);
    }else{                      //parent process
        printf("%d: a new process have been init\n", child_proc);
        waitpid(child_proc, NULL, WNOHANG);
        printf("%d: process have terminated\n", child_proc);
    }
    return 1;
}
/* 
    FUNCTION : thread_function
    DESCRIPTION:
    PARAMS:     
    RETURN VALUE:

 */
void * thread_function(void *arg)
{
    printf("Start wait event from sub_thread!\n");
    int number_event;
    int num_rev, num_send;
    while(!isExit)
    {
        //wait client
        number_event = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENT, -1);
        if(number_event == -1)
        {
            perror("can't wait in epoll");
        }
        
        // process request
        int count;
        for(count = 0; count < number_event; count++)
        {
            // check event
            if(events[count].events & EPOLLIN)
            {
                // receive request from client
                num_rev = recv(events[count].data.fd, msg, BUFFER_SIZE, 0);
                
                if(num_rev <= 0 || msg[0] == END_CODE)
                {
                    // Delete socket out epoll_fd
                    printf("SERVER:Delete %d out epoll_fd\n",events[count].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[count].data.fd, NULL);
                    printf("SERVER:Disconnection %d\n",events[count].data.fd);
                    close(events[count].data.fd);
                }else 
                {
                    // Call an api will decode message and call api in search
                    int ret_pos = process_message(msg, ret_msg);
                    if(ret_pos <= 0)
                    {
                        ret_msg[0] = ERROR_CODE;
                        ret_pos = 1;
                    }
                    // Return buffer
                    num_send = send(events[count].data.fd, ret_msg, ret_pos, 0);
                    printf("SERVER:fd=%d, num_send=%d\n", events[count].data.fd, num_send);
                }
            }
        }
    }
    // end function
    pthread_exit(NULL);
}
/* 
    FUNCTION : listen_clients
    DESCRIPTION:
    PARAMS: 
    RETURN VALUE:

 */
int listen_clients(){
    // start a new thread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, thread_function, NULL);
    // accept client
    while(!isExit)
    {
        int accept_fd = accept(server_fd, NULL, NULL);
        printf("Have a new client, fd = %d!\n", accept_fd);
        struct epoll_event ev;
        // set up to epoll listen event
        ev.data.fd = accept_fd;
        ev.events = EPOLLIN;
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev) == -1)
        {
            perror("Cannot add new fd to epoll_fds\n");
        }
        
    }

    printf("Wait to subthread end!");
    pthread_join(thread_id, NULL);

    return 1;
}
/* 
    FUNCTION : start_server
    DESCRIPTION:
    PARAMS: 
    RETURN VALUE:

 */
int start_server()
{
    //declare socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        perror("make socket!");
        exit(EXIT_FAILURE);
    }
    //assign address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(6969);
    //reuse address
    int enable = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) )
    {
        perror("setsockopt fail!");
        exit(EXIT_FAILURE);
    }
    
    //bind address to socket
    if(bind(server_fd,(struct sockaddr *) &server_address, sizeof(server_address)) )
    {
        perror("bind fail!");
        exit(EXIT_FAILURE);
    }
    //listen socket
    if(listen(server_fd, 10) ){
        perror("listen fail!");
        exit(EXIT_FAILURE);
    }
    
    epoll_fd =  epoll_create(EPOLL_SIZE);
    return 1;
}

/* 
    FUNCTION : main
    DESCRIPTION:
    PARAMS: 
    RETURN VALUE:

 */

int main()
{
    /* Open database */
    db = db_open("test_database", "../data/", DB_OPEN);
    if(db == DB_NULL)
    {
        SERVER_TRACE(("SERVER:main: cannot open db, %s\n", db_error_str[db_error_no]));
        return 1;
    }

    if(start_server()){
        printf("server started!\n");
    }
    listen_clients();

    return 1;
}