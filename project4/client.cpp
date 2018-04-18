#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
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

int verbose = 0;

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

void print_str(string str){
	if(verbose == 1){
		cout<<str<<endl;
	}
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
	
	print_str("Sending: " + string(buffer) + " to port "  + to_string( port_number));	

	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0){ 
	 	error("ERROR writing to socket");
	}
	
	bzero(buffer, strlen(buffer));
	n = read(sockfd,buffer, 255);
	if (n < 0){ 
	 	error("ERROR reading from socket");
	}
	print_str("Message received: " + string(buffer));
	close(sockfd);
}

void init_session(env_t *env){
	bzero(env->buffer,strlen(env->buffer));
	env->buffer[0] = '1';
	strncpy(env->buffer + 1, env->key , strlen(env->key));
	print_str("Init function sending message to CM: "   + string(env->buffer));
	send_message(CM_PORT,env->buffer);
	
	print_str("Message received from CM: " + string(env->buffer));
	env->port_number = atoi(env->buffer);
}

void put(env_t *env , char key[20] , char value[236]){
	bzero(env->buffer , strlen(env->buffer));
	env->buffer[0] = '2';
	strncpy(env->buffer + 1 , key , strlen(key));
	strncpy(env->buffer + strlen(key) + 1, value, strlen(value));
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

int main(int argc, char *argv[])
{
	if (argc < 2) {
       		fprintf(stderr,"usage %s client_id\n", argv[0]);
       		exit(0);
    	}
	
	struct timeval old_time, new_time , diff;	
	//Set preempted time
	gettimeofday(&old_time , NULL);
		
	env_t *env = (env_t *)malloc(sizeof(env_t));	
	bzero(env->key , strlen(env->key));
	strcpy(env->key , argv[1]);
	init_session(env);
	for(int i=0; i<5; i++){
		for(int i=0; i<10; i++){
			put(env , env->key, ",soap");
		}
		//sleep(1);
		get(env , env->key);
		finalize(env);
	}

	
	gettimeofday(&new_time , NULL);
	//get per_round time
	timersub(&new_time, &old_time, &diff);
	//cout<<diff.tv_usec<<endl;

	return 0;
}
