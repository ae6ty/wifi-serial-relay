/*
	C socket server example
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write

int client_sock = 0;

int server_write(char *s, int len) {
  s[len] = '\0';
  if (client_sock != 0) {
    write(client_sock , s,len);
  }
  return 0;
}

int server_receive(int write(char *s, int len), char *msg) 
{
	int socket_desc , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000];
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	for (;;) {
	  //accept connection from an incoming client
	  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	  if (client_sock < 0) 	{
	    perror("accept failed");
	    return 1;
	  }
	  puts("Connection accepted");
	  fprintf(stderr,"msg %s\n",msg);
	  if (msg)
	    server_write(msg,strlen(msg));
	  //Receive a message from client
	  while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
	    client_message[read_size] = '\0';
	    //	  server_write(client_message , strlen(client_message));
	    write(client_message , strlen(client_message));
	  }
	
	  if(read_size == 0) {
	    puts("Client disconnected");
	    fflush(stdout);
	  }
	  else if(read_size == -1)
	    {
	      perror("recv failed");
	    }
	}
	return 0;
}

#ifdef TESTSOCKET
int main (int ac, char **av) {
  return server_receive(server_write);
}
#endif
