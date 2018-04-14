#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define NUM_CLIENT 5
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *connection_handler(void *socket)
{
	int sock = *(int*)socket;
	char buffer[256];
	int n;
	
	if (sock < 0) 
		  error("ERROR on accept");
		
	bzero(buffer,256);
	n = read(sock,buffer,255);
	
	if (n < 0) error("ERROR reading from socket");
	printf("Here is the message: %s\n",buffer);
	
	n = write(sock,"I got your message",18);
	
	if (n < 0) error("ERROR writing to socket");
	
	close(sock);
}




int main(){

	
	int sockfd, client_sock, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 2001;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0) 
	      error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	pthread_t thread_id;
	
	while(1){
		client_sock = accept(sockfd, 
			 (struct sockaddr *) &cli_addr, 
			 &clilen);
		if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0){
			perror("could not create thread");
		        	return 1;
		}
		
		
	}
	close(sockfd);	
	
	return 0;
}
