#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../lib/constants.h"
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


char buffer[BUFFER_SIZE];

struct epoll_event events[EPOLL_MAX_EVENT];
pid_t children_process[MAX_CHILDREN_PROCESS];
int server_fd, epoll_fd;
int isExit = 0;


/* 
    FUNCTION : start_new_process
    DESCRIPTION:
    PARAMS:     
    RETURN VALUE:

 */
int start_new_process(char *command, int client_fd)
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
                // int num_rev = 
                recv(events[count].data.fd, buffer, BUFFER_SIZE, 0);
                if(strncmp(buffer, STRING_END_CONNECT, LENGTH_END_CONNECT) == 0)
                {
                   // Delete socket out epoll_fd
                    printf("Delete %d out epoll_fd\n",events[count].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[count].data.fd, NULL);
                }else 
                {
                    // buffer[num_rev - 1] = '\0';
                    start_new_process(buffer, events[count].data.fd);
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
        printf("Have a new client!\n");
        struct epoll_event ev;
        // set up to epoll listen event
        ev.data.fd = accept_fd;
        ev.events = EPOLLIN;
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev))
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
    if(start_server()){
        printf("server started!\n");
    }
    listen_clients();

    return 1;
}