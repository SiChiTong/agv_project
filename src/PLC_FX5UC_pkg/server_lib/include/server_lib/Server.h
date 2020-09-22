#ifndef SERVER_H
#define SERVER_H

#pragma once 

#include <stdio.h> 
#include <iostream>
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
  
using namespace std;
  
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 

class Server
{
    private:
        struct client_class
        {
            char hostbuffer[256];
            char ip_address[30];
            char databuffer[1024]; 
        };
        struct server_class
        {
            int opt = TRUE; 
            int master_socket; 
            int addrlen; 
            int new_socket; 
            int client_socket[30];
            int valread;
            int sd;
            int max_sd;             
        };
        struct sockaddr_in address; 
        //a message 
        const char *message = "Connecting to IPC\n";
        //set of socket descriptors 
        char buffer[1025]; //data buffer of 1K 
        fd_set readfds;  
        server_class server_class;
        void initialiseall(int port); 
    public:
        //initialise all client_socket[] to 0 so not checked 
        int max_clients = 5;
        client_class client_class[5];
    public:
        Server(void);
        Server(int port);
        ~Server(void);
        void socketloop(void);
};

#endif