#include "server_lib/Server.h"

//initialise all client_socket[] to 0 so not checked 
Server::Server(int port)
{
	initialiseall(port);
}
/***************************************************/
Server::Server(void)
{
	initialiseall(PORT);
}
Server::~Server()
{
	
}
/***************************************************/
void Server::initialiseall(int port)
{
	for (int i = 0; i < max_clients; i++) 
	{ 
		server_class.client_socket[i] = 0; 
	} 
	//create a master socket 
	if( (server_class.master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(server_class.master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&server_class.opt, 
	sizeof(server_class.opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 

	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY;//inet_addr("192.168.1.199"); 
	address.sin_port = htons( PORT ); 

	//bind the socket to localhost port 8888 
	if (bind(server_class.master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 

	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(server_class.master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	//accept the incoming connection 
	server_class.addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
}

/***************************************************/
//process in loop
void Server::socketloop(void)
{
	//clear the socket set 
	FD_ZERO(&readfds); 

	//add master socket to set 
	FD_SET(server_class.master_socket, &readfds); 
	server_class.max_sd = server_class.master_socket; 

	//add child sockets to set 
	for (int i = 0 ; i < max_clients ; i++) 
	{ 
		//socket descriptor 
		server_class.sd = server_class.client_socket[i]; 

		//if valid socket descriptor then add to read list 
		if(server_class.sd > 0) FD_SET( server_class.sd , &readfds); 

		//highest file descriptor number, need it for the select function 
		if(server_class.sd > server_class.max_sd) server_class.max_sd = server_class.sd; 
	} 

	//wait for an activity on one of the sockets , timeout is NULL , 
	//so wait indefinitely 
	if ((select( server_class.max_sd + 1 , &readfds , NULL , NULL , NULL) < 0) 
		&& (errno!=EINTR)) {printf("select error");} 
	//If something happened on the master socket , 
	//then its an incoming connection 
	if (FD_ISSET(server_class.master_socket, &readfds)) 
	{ 
		if ((server_class.new_socket = accept(server_class.master_socket,(struct sockaddr *)&address, (socklen_t*)&server_class.addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		//inform user of socket number - used in send and receive commands 
		cout<<"New connection , socket fd is "<< inet_ntoa(address.sin_addr) << "---Port is :"<< ntohs(address.sin_port) << endl; 

		//send new connection greeting message 
		if( send(server_class.new_socket, message, strlen(message), 0) != strlen(message)) 
			if( send(server_class.new_socket, message, strlen(message), 0) != strlen(message)) {perror("send");} 
			printf("Welcome message sent successfully -- "); 

			//add new socket to array of sockets 
			for (int i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( server_class.client_socket[i] == 0 ) 
				{ 
					server_class.client_socket[i] = server_class.new_socket;
					printf("Adding to list of sockets as %s\n" , client_class[i].hostbuffer);
					printf("....................................................................\n"); 
					break; 
				} 
			} 
	} 

	//else its some IO operation on some other socket 
	for (int i = 0; i < max_clients; i++) 
	{ 
		server_class.sd = server_class.client_socket[i]; 

		if (FD_ISSET( server_class.sd , &readfds)) 
		{ 
			//Check if it was for closing , and also read the 
			//incoming message 
			if ((server_class.valread = read( server_class.sd , buffer, 1024)) == 0) 
			{ 
				//Somebody disconnected , get his details and print 
				getpeername(server_class.sd , (struct sockaddr*)&address , (socklen_t*)&server_class.addrlen); 
				printf("Host disconnected , ip %s , port %d \n," , 
				inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
				printf("....................................................................\n"); 

				//Close the socket and mark as 0 in list for reuse 
				close( server_class.sd ); 
				server_class.client_socket[i] = 0; 
			} 
  			
			//Echo back the message that came in 
			else
			{ 
				//set the string terminating NULL byte on the end 
				//of the data read 
				buffer[server_class.valread] = '\0'; 
				strcpy(client_class[i].ip_address ,inet_ntoa(address.sin_addr));
				strcpy(client_class[i].databuffer ,buffer);
			} 
		} 
	} 
}

