#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> 
#include <stddef.h>
#include <regex.h>

#define PORT 3499
#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define DEBUG 1
#define IP_INFO_SERVER "localhost"
#define PUERTO_INFO_SERVER "12000"

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

struct arg_struct {
    int socket_to_client;
    pthread_t thr;
};

char* enviarAInfoServer(mensaje) {
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
            
    struct addrinfo hints, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(IP_INFO_SERVER, PUERTO_INFO_SERVER, &hints, &res);
    
    char* udp_mensaje = mensaje;
    int sent_msg_size = sendto(udp_socket, udp_mensaje, strlen(udp_mensaje)+1, 0, res->ai_addr, res->ai_addrlen);
    
    char* udp_respuesta = malloc(MAX_MSG_SIZE);
    int udp_tamanio_recibido = recv(udp_socket, udp_respuesta, MAX_MSG_SIZE, 0);
    return udp_respuesta;
}

/* Funcion auxiliar para manejar un hilo con un socket particular */
void *aux(struct arg_struct *args) 
{ 
    int is_closed = 0;
    while(is_closed == 0){ 
        //primitiva RECEIVE
        char* data = malloc(MAX_MSG_SIZE);
        int data_size = MAX_MSG_SIZE;
        int received_data_size = recv(args->socket_to_client, data, data_size, 0);
        
        if(received_data_size == 0){ //el socket está cerrado cuando es 0
            is_closed = 1;
        }
        // Ejecutar procesos si no fue cerrado el socket
        if(is_closed == 0) {   //sino se quedaria bloqueado en el send que es bloqueante
            printf("Recibido del cliente (%d bytes): %s\n", received_data_size, data);
            
            // Procesar lo recibido del cliente
            int encontre_espacio = 0;
            int index = 0;
            while(!encontre_espacio && index < 16) { // SEteamos en 16 para que eventualmente no tenga que recorrer una tira larga sin encontrar un espacio
                if(data[index] == ' ') {
                    encontre_espacio = 1;
                }
                index++;
            }
            
            // Crear socket UDP y consultar por el usuario
            char* mensaje = "GET_USER-2\nmatias mathias\n";
            char* respuesta = enviarAInfoServer(mensaje);
            printf(respuesta);
            
            int i;
            for (i = 0; i < received_data_size; i++) {
                data[i] = toupper(data[i]);
            }
            
            //primitiva SEND

            int sent_data_size = send(args->socket_to_client, data, received_data_size, 0);
            printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
        }
        // Libero la memoria del data
        free(data);
    }
    
    printf("Socket cerrado\n");
    pthread_cancel(args->thr);
} 


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
        //primitiva ACCEPT
        pthread_t thr;
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int socket_to_client = accept(welcome_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        
        struct arg_struct args;
        args.socket_to_client = socket_to_client;
        args.thr = thr;

        struct arg_struct *args_aster = &args;
        
        char* mensaje = "Bienvenido!\n";        
        int sent_data_size = send(socket_to_client, mensaje, strlen(mensaje)+1, 0);
        pthread_create(&thr, NULL, (void*) aux, args_aster);
        
        //primitiva CLOSE
        // close(socket_to_client);
    }

   //CLOSE del socket que espera conexiones
   close(welcome_socket);
}



