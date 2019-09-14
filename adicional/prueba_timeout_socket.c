
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
#include <stddef.h>
#include <regex.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 3499
#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define DEBUG 1
#define IP_INFO_SERVER "localhost"
#define PUERTO_INFO_SERVER "12000"
#define PORT_IMAP "143"
#define IP_IMAP1 "192.168.56.102"

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

void main()
{  
    //primitiva SOCKET
    int welcome_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    //primitiva BIND
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(MY_IP);  
    bind(welcome_socket, (struct sockaddr*)&server_addr, server_addr_size);

    //primitiva LISTEN
    listen(welcome_socket, MAX_QUEUE);

    while (1) {
        // primitiva ACCEPT
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int socket_to_client = accept(welcome_socket, (struct sockaddr *)&client_addr, &client_addr_size);

        // timeout en el socket_to_client
        struct timeval timeout;      
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        /* 
            setsockopt(int s, int level, int optname, const void *optval,
            socklen_t optlen);
        
            SO_SNDTIMEO	      set timeout value	for output
            SO_RCVTIMEO	      set timeout value	for input
        */
        if (setsockopt (socket_to_client, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        error("setsockopt failed\n");

        if (setsockopt (socket_to_client, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        error("setsockopt failed\n");

        // recv data
        char* data = malloc(MAX_MSG_SIZE);
        int data_size = MAX_MSG_SIZE;
        int received_data_size;
        received_data_size = recv(socket_to_client, data, data_size, 0);
        if(received_data_size == -1) {
            fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
            close(socket_to_client);
        }


    }
    //CLOSE del socket que espera conexiones
    close(welcome_socket);
}


