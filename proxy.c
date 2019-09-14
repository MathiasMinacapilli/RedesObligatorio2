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
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>

#define DEBUG 1

#define PORT 3490
#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define DEBUG 1
#define IP_INFO_SERVER "localhost"
#define PUERTO_INFO_SERVER "12000"
#define PORT_IMAP "143"
#define IP_IMAP1 "192.168.56.101"

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

struct arg_struct {
    int socket_to_client;
    pthread_t thr;
};

struct sock{
    int client_socket;
    int socket_IMAP;
};


char* get_usuario_to_info_server(char* message){
    char *source = message;
    int length = (int)strlen(source);
    int iter = 0;
    int encontre_usuario = 0;
    int cant_espacios = 0;
    while(iter < length && encontre_usuario == 0){
        if(source[iter] == ' '){
            cant_espacios++;
        }
        if(source[iter] == ' ' && cant_espacios == 2){
            encontre_usuario = 1;
        }
        iter++;
    }
    int j = iter;
    while(source[j] != ' '){
        j++;
    }
    int largo = j-iter;
    char* usuario = malloc(largo);
    for(int k=iter; k<j; k++){
        usuario[k-iter] = source[k];
    }
    char *mensaje = malloc(largo+10);
    char *string_get_user = malloc(9);
    string_get_user = "GET_USER ";
    for(int x=0; x<9; x++){
        mensaje[x] = string_get_user[x];
    }
    for(int y=0; y<largo; y++){
        mensaje[y+9] = usuario[y];
    }
    mensaje[largo+9] = '\n';
    if(encontre_usuario==0){
        return (mensaje = "GET_USER error\n");
    }
    else {
        return mensaje;
    }
}


int conectarse_IMAP(char* IP_IMAP){
    
    int socket_IMAP = socket(AF_INET, SOCK_STREAM,0);
    
    struct addrinfo hints, *res;
    memset(&hints, 0 , sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(IP_IMAP, PORT_IMAP, &hints, &res); // DEberia pasar IP_IMAP (la pasada por parametro) el primer parametro de la llamada
    
    //me conecto al IMAP 
    
    connect(socket_IMAP, res->ai_addr, res->ai_addrlen);
    
    //me chupo el bienvenida del imap
    
    char* data = malloc(MAX_MSG_SIZE);
    int data_size = MAX_MSG_SIZE;
    int received_data_size = recv(socket_IMAP, data, data_size, 0);                //Chequear que alcance con un recv?
    printf("Bienvenida del IMAP (%d bytes): %s\n", received_data_size, data);
    return(socket_IMAP);
}

void IMAP_cliente(struct sock* sockets){
    int socket_IMAP = sockets->socket_IMAP;
    int client_socket = sockets->client_socket;
    char* data = malloc(MAX_MSG_SIZE);
    int data_size = MAX_MSG_SIZE;
    int received_data_size;
    int encontre_bye = 0;
    while(encontre_bye == 0){
        if(DEBUG) printf("[IMAP_cliente-%d] Recibiendo datos del IMAP...\n", client_socket);
        received_data_size = recv(socket_IMAP, data, data_size, 0);
        if(DEBUG) printf("[IMAP_cliente-%d] Recibidos datos del IMAP...\n", client_socket);
        
        printf("Recibido del IMAP (%d bytes): %s\n", received_data_size, data);
        
        if(DEBUG) printf("[IMAP_cliente-%d] Enviando datos del IMAP al cliente...\n", client_socket);
        send(client_socket, data, received_data_size, 0);
        if(DEBUG) printf("[IMAP_cliente-%d] Enviados datos del IMAP al cliente...\n", client_socket);
        
        printf("Enviado al cliente (%d bytes): %s\n", received_data_size, data);
        
        if(strstr(data, "* BYE") != NULL){
            if(DEBUG) printf("[IMAP_cliente-%d] IMAP cerró la conexión...\n", client_socket);
            encontre_bye = 1;
            void * algo;
            pthread_exit(algo);
        }
    }
}

void Cliente_IMAP(struct sock* sockets){
    int socket_IMAP = sockets->socket_IMAP;
    int client_socket = sockets->client_socket;
    char* data = malloc(MAX_MSG_SIZE);
    int data_size = MAX_MSG_SIZE;
    int received_data_size;
    while(/*no se cerro conexion?*/1){
        if(DEBUG) printf("[Cliente_IMAP-%d] Recibiendo datos del cliente...\n", client_socket);
        received_data_size = recv(client_socket, data, data_size, 0);
        if(DEBUG) printf("[Cliente_IMAP-%d] Recibidos datos del cliente...\n", client_socket);
        
        printf("Recibido del cliente (%d bytes): %s\n", received_data_size, data);
        
        if(DEBUG) printf("[Cliente_IMAP-%d] Enviando datos del cliente al IMAP...\n", client_socket);
        send(socket_IMAP, data, received_data_size,0);
        if(DEBUG) printf("[Cliente_IMAP-%d] Enviados datos del cliente al IMAP...\n", client_socket);
        
        printf("Enviado al IMAP (%d bytes): %s\n", received_data_size, data);
    }  
}

char* getIP(char* udp_respuesta){
    char * source = udp_respuesta;
    int length = (int)strlen(source);
    int i = 3;
    int encontre_IP = 0;
    int cant_espacios = 1;
    
    int j = i;
    while((j<length) && (source[j] != ' ') && (source[j] != '\n')){
        j++;
    }
    
    int largo = j-i;
    char* ip = malloc(largo);
    
    for(int k=i; k<j; k++){
        ip[k-i] = source[k];        
    }
    
    if(DEBUG) printf("[getIP] Obteniendo la IP de la respuesta del info_server...\n");
    
    char* devolver = malloc(17);
    for(int iter=0; iter<17; iter++){
        devolver[iter] = ip[iter];
    }
    
    if(DEBUG) printf("[getIP] IP obtenida...\n");
    return (devolver);
}

/* Funcion auxiliar para manejar un hilo con un socket particular */
void *aux(struct arg_struct *args){ 
    int is_closed = 0;
    int estoy_conectado_a_IMAP = 0;
    int estoy_logueado = 0;
    int socket_IMAP;
    int cantidad_pqts_enviado;
    while(is_closed == 0 && estoy_logueado == 0){
        cantidad_pqts_enviado = 0;
        //Recibo un mensaje del cliente        
        char* data = malloc(MAX_MSG_SIZE);
        int data_size = MAX_MSG_SIZE;
        
        // timeout en el udp_socket
        struct timeval timeout_client;      
        timeout_client.tv_sec = 180;
        timeout_client.tv_usec = 0;

        //SO_RCVTIMEO	      set timeout value	for input
        if (setsockopt (args->socket_to_client, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_client,
                sizeof(timeout_client)) < 0)
        error("setsockopt failed\n");

        //SO_SNDTIMEO	      set timeout value	for output
        if (setsockopt (args->socket_to_client, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout_client,
                sizeof(timeout_client)) < 0)
        error("setsockopt failed\n");
        
        if(DEBUG) printf("[aux-%d] Recibiendo datos del cliente...\n", args->socket_to_client);
        
        int received_data_size = recv(args->socket_to_client, data, data_size, 0);
        
        if(DEBUG) printf("[aux-%d] Datos recibidos del cliente...\n", args->socket_to_client);

        //si el socket esta cerrado cuando es 0
        if(received_data_size == -1){ 
            is_closed = 1;
            //Si Cliente no se loguea y queda inactivo, cierro la conexion, libero data y cierro
            char* disconnect_por_inactividad = "* BYE Disconnected for inactivity.\n";
            int size_disconnect = strlen(disconnect_por_inactividad);
            
            if(DEBUG) printf("[aux-%d] Se desconectará el socket con el cliente por inactividad...\n", args->socket_to_client);
            
            send(args->socket_to_client, disconnect_por_inactividad, size_disconnect, 0);
            
            if(DEBUG) printf("[aux-%d] Enviado mensaje de desconexión...\n", args->socket_to_client);
            
            free(data);
            close(args->socket_to_client);
            //pthread_cancel(args->thr);
            pthread_exit(args->thr);
        }

        // Ejecutar procesos si no fue cerrado el socket
        if(is_closed == 0) {   //sino se quedaria bloqueado en el send que es bloqueante

            printf("[aux-%d] Recibido del cliente (%d bytes): %s\n", args->socket_to_client, received_data_size, data);

            //Aca hay que buscar en lo recibido un nombre de usuario, chequear que si es un mensaje roto me de algo para que le info_server responda "NO"
            //char* mensaje = buscar_nombre_usuario(data);

            //Creamos socket UDP y cosultamos por el usuario

            int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
            
            // timeout en el udp_socket
            struct timeval timeout;      
            timeout.tv_sec = 3;
            timeout.tv_usec = 0;

            //SO_RCVTIMEO	      set timeout value	for input
            if (setsockopt (udp_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
                error("setsockopt failed\n");

            //SO_SNDTIMEO	      set timeout value	for output
            if (setsockopt (udp_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
                error("setsockopt failed\n");
        
            struct addrinfo hints, *res;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            getaddrinfo(IP_INFO_SERVER, PUERTO_INFO_SERVER, &hints, &res);
            
            //char* mensaje = "GET-USER matias\n";
            //ACA DEBERIAMOS MANDARLO UN PAR DE VECES Y SI NO NOS RESPONDE DECIMOS QUE CERRO EL INFO SERVER
            char* mensaje = get_usuario_to_info_server(data);
            char* udp_mensaje = mensaje;
           
            
            char* udp_respuesta = malloc(MAX_MSG_SIZE);
            int recibi = 0;
            while(cantidad_pqts_enviado < 3 && recibi == 0){
                
                if(DEBUG) printf("[aux-%d] Enviando paquete a info_server...\n", args->socket_to_client);
                
                int sent_msg_size = sendto(udp_socket, udp_mensaje, strlen(udp_mensaje)+1, 0, res->ai_addr, res->ai_addrlen);
                
                int udp_tamanio_recibido = recv(udp_socket, udp_respuesta, MAX_MSG_SIZE, 0);
                
                if(udp_tamanio_recibido == -1) { 
                    cantidad_pqts_enviado++;
                    if(DEBUG) printf("[aux-%d] Fallo la respuesta del info_server...\n", args->socket_to_client);
                } else {
                    recibi = 1;
                    if(DEBUG) printf("[aux-%d] Recibida respuesta del info_server...\n", args->socket_to_client);
                }
            }
            if(cantidad_pqts_enviado == 3){     //Si envie el paquete al info_server 3 veces y no responde, asumo que esta caido
                if(DEBUG) printf("[aux-%d] No se pudo conectar con el info_server, cerrando la conexión...\n", args->socket_to_client);
                close(args->socket_to_client);
                pthread_exit(args->thr);                
            }

            printf("%c%c",udp_respuesta[0], udp_respuesta[1]);


            if(udp_respuesta[0] == 'N'){            //Si la respuesta arranca con N es porque me responde NO
                if(DEBUG) printf("[aux-%d] No existe el usuario ingresado (respuesta del info_server)...\n", args->socket_to_client);
                if(estoy_conectado_a_IMAP == 0){
                    if(DEBUG) printf("[aux-%d] No estoy conectado a IMAP, contectando...\n", args->socket_to_client);
                    //Si no estoy conectado, me conecto a IMAP por defecto (IMAP1)
                    estoy_conectado_a_IMAP = 1;
                    socket_IMAP = conectarse_IMAP(IP_IMAP1);
                }
                
                //Envio el mensaje recibido al IMAP
                if(DEBUG) printf("[aux-%d] Enviando mensaje al IMAP...\n", args->socket_to_client);
                send(socket_IMAP, data, received_data_size, 0);
                if(DEBUG) printf("[aux-%d] Mensaje enviado...\n", args->socket_to_client);
                
                //Recibo la respuesta del IMAP
                if(DEBUG) printf("[aux-%d] Recibiendo datos del IMAP...\n", args->socket_to_client);
                received_data_size = recv(socket_IMAP, data, data_size, 0);
                if(DEBUG) printf("[aux-%d] Datos recibidos...\n", args->socket_to_client);
                
                //Reenvio al cliente la respuesta
                if(DEBUG) printf("[aux-%d] Reenviando respuesta del IMAP al cliente...\n", args->socket_to_client);
                send(args->socket_to_client, data, received_data_size, 0);
                if(DEBUG) printf("[aux-%d] Reenvio completado...\n", args->socket_to_client);
                
                printf("Enviado al cliente (%d bytes): %s\n", received_data_size, data);
                
                if (strstr(data, "* BYE") != NULL){
                    if(DEBUG) printf("[aux-%d] Cerrando la conexión con el cliente, el IMAP se despidió...\n", args->socket_to_client);
                    //Si el IMAP cerro la conexion, libero data y cierro
                    free(data);
                    close(socket_IMAP);
                    close(args->socket_to_client);
                    printf("Socket cerrado\n");
                    //pthread_cancel(args->thr);
                    pthread_exit(args->thr);
                }
            }
            else{
                if(DEBUG) printf("[aux-%d] Existe usuario en el sistema...\n", args->socket_to_client);
                
                //Si el info_server dio alguna respuesta
                if(DEBUG) printf("[aux-%d] Obteniendo IP del IMAP de la respuesta del info_server...\n", args->socket_to_client);
                char* IP_IMAP = getIP(udp_respuesta);
                if(DEBUG) printf("[aux-%d] IP obtenida: %s...\n", args->socket_to_client, IP_IMAP);
                //char* IP_IMAP = "192.168.56.101";
                
                if(estoy_conectado_a_IMAP == 0){
                    //Si no estoy conectado a ningun IMAP, me conecto al que me corresponde
                    if(DEBUG) printf("[aux-%d] Conectandome al servidor IMAP...\n", args->socket_to_client);
                    estoy_conectado_a_IMAP = 1;
                    socket_IMAP = conectarse_IMAP(IP_IMAP);
                    if(DEBUG) printf("[aux-%d] Conectado...\n", args->socket_to_client);
                } else {
                    if(IP_IMAP != IP_IMAP1){
                        //Si estoy conectado, pero no al IMAP que me corresponde
                        close(socket_IMAP);
                        if(DEBUG) printf("[aux-%d] Conectandome al servidor IMAP...\n", args->socket_to_client);
                        socket_IMAP = conectarse_IMAP(IP_IMAP);
                        if(DEBUG) printf("[aux-%d] Conectado...\n", args->socket_to_client);
                    }
                }
                
                //Envio el mensaje recibido al IMAP
                if(DEBUG) printf("[aux-%d] Enviando mensaje al IMAP...\n", args->socket_to_client);
                send(socket_IMAP, data, received_data_size, 0);
                if(DEBUG) printf("[aux-%d] Enviado mensaje...\n", args->socket_to_client);
                
                //Recibo la respuesta del IMAP
                if(DEBUG) printf("[aux-%d] Esperando respuesta del IMAP...\n", args->socket_to_client);
                received_data_size = recv(socket_IMAP, data, data_size, 0);
                if(DEBUG) printf("[aux-%d] Recibida respuesta del IMAP...\n", args->socket_to_client);
                
                //Reenvio al cliente la respuesta
                if(DEBUG) printf("[aux-%d] Enviando respuesta del IMAP al cliente...\n", args->socket_to_client);
                send(args->socket_to_client, data, data_size, 0);
                if(DEBUG) printf("[aux-%d] Enviada respuesta del IMAP al cliente...\n", args->socket_to_client);
                
                if(strstr(data, "Logged in") != NULL){
                    //Si me loguee bien
                    if(DEBUG) printf("[aux-%d] Login exitoso...\n", args->socket_to_client);
                    estoy_logueado = 1;

                    //Creo estructuras para pasarle los sockets a los threads
                    struct sock sockets;
                    sockets.client_socket = args->socket_to_client;
                    sockets.socket_IMAP = socket_IMAP;
                    struct sock *sockets_aster = &sockets;
                    
                    if(DEBUG) printf("[aux-%d] Creando threads...\n", args->socket_to_client);
                    
                    pthread_t thr1;
                    pthread_t thr2;
                    void* retval;
                    
                    //Creo los threads que van a servir para comunicar el cliente con el IMAP
                    if(DEBUG) printf("[aux-%d] Ejecutando threads...\n", args->socket_to_client);
                    pthread_create(&thr1, NULL, (void*) Cliente_IMAP, sockets_aster);
                    pthread_create(&thr2, NULL, (void*) IMAP_cliente, sockets_aster);
                    
                    if(DEBUG) printf("[aux-%d] Realizando el join del thread 2...\n", args->socket_to_client);
                    pthread_join(thr2, (void*) retval);
                    
                    if(DEBUG) printf("[aux-%d] Finalizado join del thread 2...\n", args->socket_to_client);
                    pthread_cancel(thr1);
                    if(DEBUG) printf("[aux-%d] Terminado el thread 1...\n", args->socket_to_client);
                } else {
                    if(DEBUG) printf("[aux-%d] Login no exitoso...\n", args->socket_to_client);
                    if(strstr(data, "* BYE") != NULL){
                        //Si el servidor IMAP cerro la conexion
                        if(DEBUG) printf("[aux-%d] IMAP cerró la conexión...\n", args->socket_to_client);
                        free(data);
                        close(socket_IMAP);
                        close(args->socket_to_client);
                        printf("Socket cerrado\n");
                        pthread_exit(args->thr);
                    }
                }
            }            
        }

        //Aca deberia liberar todo?
        free(data);
    }

    //Si el info server esta cerrado, o termine la conexion, cierro este thread.
    //Que hace close si nunca abri el socket?
    close(socket_IMAP);
    close(args->socket_to_client);
    printf("Socket cerrado\n");
    pthread_exit(args->thr);
    
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

    if(DEBUG) printf("[main-%d] Servidor escuchando en el puerto %d...\n", welcome_socket, PORT);
     
    while (1) {
        //primitiva ACCEPT
        pthread_t thr;
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int socket_to_client = accept(welcome_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        
        if(DEBUG) printf("[main-%d] Cliente conectado en el socket %d\n", socket_to_client, socket_to_client);
        int tamanio_arg_struct = sizeof(struct arg_struct);
        struct arg_struct *args = malloc(tamanio_arg_struct);
        args->socket_to_client = socket_to_client;
        args->thr = thr;

     //   struct arg_struct *args_aster = &args;
        
        char* mensaje = "* OK Bienvenido IMAP4rev1 Fing \n";        
        int sent_data_size = send(socket_to_client, mensaje, strlen(mensaje)+1, 0);
        
        if(DEBUG) printf("[main-%d] Enviado mensaje de bienvenida al usuario...\n", socket_to_client);
        
        if(DEBUG) printf("[main-%d] Creando thread para el cliente...\n", socket_to_client);
        pthread_create(&thr, NULL, (void*) aux, args);
        
        //primitiva CLOSE
        // close(socket_to_client);
    }

   //CLOSE del socket que espera conexiones
   close(welcome_socket);
}
