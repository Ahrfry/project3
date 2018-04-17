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

#define QUERRY 1
#define PUT 2
#define GET 3
#define FINALIZE 4
#define NUM_NODES 5



using namespace std;

//map key to values
static map<string, list<string>> map_user_to_cart;

int verbose = 0;


//replicas <por_number , <map<key,list<values>>
//static map<int , map<string , list<string>> replicas;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void print_str(string str){
	if(verbose == 1){
		cout<<str<<endl;
	}
}

int query(char buff[]){
	int value = 0;
	for(int i = 1 ; i < strlen(buff); i++){
		value += buff[i];
	}

	return value % NUM_NODES;
}



char *get(char buff[]){
	
	char key[20];
	bzero(key , 20);
	strncpy(key , buff , strlen(buff));
	
	string key_str = key + 1;
	string temp_load = "";
	int flag = 0;
	char *load;	
	map<string, list<string>>::iterator it;
	it = map_user_to_cart.find(key_str);
	if(it != map_user_to_cart.end()){
		flag = 1;
		for(list<string>::iterator list_it = it->second.begin(); list_it!=it->second.end(); list_it++){
			temp_load = temp_load + " " + *list_it;
		}
	}
	bzero(buff , strlen(buff));
	strcpy(buff , temp_load.c_str());
	load = buff;	
	//printf("key: %s value: %s \n" , key , value);
	return load;

}


//strings buff and maps<key,value>
int put(char buff[]){
	char key[20];
	char value[256];
	bzero(key,20);
	bzero(value,256);


	int i = 0;
	while(buff[i] != ','){
		i++;
	}	
	
	strncpy(key , buff + 1 , i -1);
	strncpy(value , buff + i + 1 , strlen(buff));
	string key_str = key;
	string value_str = value;	
		
	map<string, list<string>>::iterator it;
	it = map_user_to_cart.find(key_str);
	
	if(it == map_user_to_cart.end()){
		list<string> cart;
		cart.push_back(value_str);
		
		map_user_to_cart.insert(pair<string , list<string>>(key_str, cart));
		print_str("PUT: creating new cart ");
	}else{
		it->second.push_back(value_str);
		print_str("PUT: Cart exists");	
	}
	//printf("key: %s value: %s \n" , key , value);
	return 1;
}

void finalize(char buff[]){
	
	char key[20];
	bzero(key , 20);
	strncpy(key , buff , strlen(buff));
	
	string key_str = key + 1;
	string temp_load = "";
	map<string, list<string>>::iterator it;
	it = map_user_to_cart.find(key_str);
	if(it != map_user_to_cart.end()){
		it->second.clear();
		map_user_to_cart.erase(it);
	}
}

//thread function that handles the connection
void *connection_handler(void *socket)
{
	print_str("Connected Started");
	//cast socket pointer
	int sock = *(int*)socket;
	//buffer to receive message
	char buffer[256];
	//test read , write and port number
	int n , node_port = -1;
	
	if (sock < 0){ 
		  error("ERROR on accept");
	}
	
	bzero(buffer,256);
	n = read(sock,buffer,255);
	
	print_str("Received message: " + string(buffer));
		
	if (n < 0){
	       	error("ERROR reading from socket");
	}
	
	int req;
	req =  buffer[0] -'0';	
	
		
	//if service request type is QUERRY [1smith]	
	if(req == QUERRY){
	 	node_port = 2000 + query(buffer);
	//param [2smith,soap]
	}else if(req == PUT){
		node_port = put(buffer);
		bzero(buffer,256);
		sprintf(buffer, "%d", node_port);
	//[3smith]
	}else if(req == GET){
		get(buffer);	
	//[4smith]
	}else if(req == FINALIZE){
		finalize(buffer);	
		bzero(buffer,256);
		sprintf(buffer, "%s", "Session finalized");
	}
	
		
	n = write(sock, buffer , strlen(buffer));
	if (n < 0){
	       	error("ERROR writing to socket");
	}
	
	
	//send_message(2001);	
		
	
	close(sock);
}



int send_message(int p_number){
	
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	
	portno = p_number;
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
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
		error("ERROR connecting");
	}
	
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	
	if (n < 0){ 
		error("ERROR writing to socket");
	}
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	
	if (n < 0){ 
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);
	close(sockfd);

	return 0;
}



int main(int argc , char *argv[]){


	if (argc < 2) {
       		fprintf(stderr,"usage %s port_number\n", argv[0]);
       		exit(0);
    	}	
	
	int sockfd, client_sock, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]) + 2000;
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
