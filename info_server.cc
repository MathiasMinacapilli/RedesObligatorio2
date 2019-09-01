// info_server.cc
#include <string>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //close
#include <stdlib.h>
#include <stdio.h>

//Clase Account
#include "Account.h"

#define PORT 8890
#define MY_IP "127.0.0.1"
#define MAX_MSG_SIZE 1024

using namespace std;

//Carga de datos en la variable accounts
map<string, Account*> accounts = {
    {"matias", new Account("matias", "saitam", "192.168.56.102")}
};
// = {
//accounts.insert(pair<string, Account*>("matias", new Account("matias", "saitam", "192.168.56.102")));
/*
    {"martin", new Account("martin", "nitram", "192.168.56.102")},
    {"eduardo", new Account("eduardo", "odraude", "192.168.56.102")},
    {"ariel", new Account("ariel", "leira", "192.168.56.102")},
    {"lucia", new Account("lucia", "aicul", "192.168.56.103")},
    {"sara", new Account("sara", "aras", "192.168.56.103")},
    {"clara", new Account("clara", "aralc", "192.168.56.103")},
    {"marta", new Account("marta", "atram", "192.168.56.103")}
};*/

/* 
IMAP_1 – 192.168.56.102
IMAP_2 – 192.168.56.103
*/
int main(void) {
    //primitiva SOCKET
    int server_socket = socket(PF_INET, SOCK_DGRAM, 0);
    
    //primitiva BIND
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(MY_IP);
    bind(
        server_socket, 
        (struct sockaddr*)&server_addr, server_addr_size
    );
    
    while (1) {
        //primitiva RECEIVE
        struct sockaddr_in remote_addr;
        socklen_t remote_addr_size = sizeof remote_addr;
        char* data = malloc(MAX_MSG_SIZE);
        int data_size = MAX_MSG_SIZE;
        int received_data_size = recvfrom(
            server_socket, 
            data, data_size, 0,
            (struct sockaddr*)&remote_addr, &remote_addr_size
        );
        
        printf("Recibido del cliente (%d bytes): %s\n", received_data_size, data);
        
        int i;
        for (i = 0; i < received_data_size; i++) {
            data[i] = toupper(data[i]);
        }
        
        //primitiva SEND
        int sent_data_size = sendto(
            server_socket, 
            data, received_data_size, 0,
            (struct sockaddr*)&  remote_addr, remote_addr_size
        );
        printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
        
    }

    //primitiva CLOSE
    close(server_socket);
}
