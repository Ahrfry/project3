#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>


#define CM_PORT 3000


using namespace std;

typedef struct _env_t{
	char key[20];
	char buffer[255];
	int port_number;
}env_t;

env_t event;
//char buffer[256];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


void send_message(int port_number , char buffer[]){
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	portno = port_number;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");
	server = gethostbyname("localhost");
	
	if (server == NULL) {
	
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	 (char *)&serv_addr.sin_addr.s_addr,
	 server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
		error("ERROR connecting");	
	}
	
	printf("Sending: %s to port %d \n" , buffer , port_number);	

	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0){ 
	 	error("ERROR writing to socket");
	}
	
	bzero(buffer, strlen(buffer));
	n = read(sockfd,buffer, 255);
	if (n < 0){ 
	 	error("ERROR reading from socket");
	}
	cout<<"Message received: "<<buffer<<endl;
	close(sockfd);
}

void init_session(env_t *env){
	bzero(env->buffer,strlen(env->buffer));
	env->buffer[0] = '1';
	strncpy(env->buffer + 1, env->key , strlen(env->key));
	printf("Init function sending message to CM: %s \n" , env->buffer);
	send_message(CM_PORT,env->buffer);
	
	printf("Message received from CM: %s\n",env->buffer);
	env->port_number = atoi(env->buffer);
}

void put(env_t *env , char key[20] , char value[236]){
	bzero(env->buffer , strlen(env->buffer));
	env->buffer[0] = '2';
	strncpy(env->buffer + 1 , key , strlen(key));
	strncpy(env->buffer + strlen(key) + 1, value, strlen(value));
	cout<<env->buffer<<endl;
	send_message(env->port_number , env->buffer);

}

void get(env_t *env , char key[20]){
	bzero(env->buffer , strlen(env->buffer));
	env->buffer[0] = '3';
	strncpy(env->buffer + 1 , env->key , strlen(env->key));
	send_message(env->port_number , env->buffer);
	//cout<<"Cart contains: "<<buffer<<endl;
}

void finalize(env_t *env){
	
	bzero(env->buffer , strlen(env->buffer));
	env->buffer[0] = '4';
	strncpy(env->buffer + 1 , env->key , strlen(env->key));
	send_message(env->port_number , env->buffer);
}


void *client_thread(void *args){

}

int main()
{
	env_t *env = (env_t *)malloc(sizeof(env_t));	
	bzero(env->key , strlen(env->key));
	strcpy(env->key , "smith");
	init_session(env);
	for(int i=0; i<1000; i++){
		for(int i=0; i<5; i++){
			put(env , "smith" , ",soap");
		}
		//sleep(1);
		get(env , "smith");
		finalize(env);
	}
	//strcpy(buffer , "2smith,soap");
	//send_message(buffer , 2004);
	return 0;
}
