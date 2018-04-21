#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>


#define QUERRY 1
#define REPLICA 2
#define NUM_NODES 5


using namespace std;


static map<int, list<string>> map_reps;

//oppress couts 
int verbose = 1;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int query(char buff[]){
	int value = 0;
	for(int i = 1 ; i < strlen(buff); i++){
		value += buff[i];
	}

	return value % NUM_NODES;
}

void print_str(string str){
	if(verbose == 1){
		cout<<str<<endl;
	}
}

void get_reps(char buff[]){
	int port  = atoi(buff + 1);
	int rep1, rep2;
	map<int, list<string>>::iterator it;
	it = map_reps.find(port);
	//rep1 = it->second[0];
	bzero(buff , 255);
	string temp_load = "";
	for(list<string>::iterator list_it = it->second.begin(); list_it!=it->second.end(); list_it++){
		temp_load = temp_load + " " + *list_it;
	}

	strcpy(buff-1 , temp_load.c_str());
}

//thread function that handles the connection
void *connection_handler(void *socket)
{
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
	
	if (n < 0){
	       	error("ERROR reading from socket");
	}
	
	int req;
	req =  buffer[0] -'0';	
	
		
	//if service request type is QUERRY	
	if(req == QUERRY){
	 	node_port = 2000 + query(buffer);
		bzero(buffer,256);
		sprintf(buffer, "%d", node_port);
		print_str("Service Request: Querry | Query Value: " + string(buffer + 1) + " | Node Port: " + to_string(node_port));
	}else if(req == REPLICA){
		//first char is the command
		get_reps(buffer);
	}
	
	
	
	//print_str("Received requests for Replica from " + string(buff));	
	n = write(sock, buffer , 255);
	
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


//initialize mapping of replicas
void init_reps(){
	
	list<string> port1;
	port1.push_back("2002");
	port1.push_back("2003");

	list<string> port2;
	port2.push_back("2003");
	port2.push_back("2004");

	list<string> port3;
	port3.push_back("2004");
	port3.push_back("2005");
	
	list<string> port4;
	port4.push_back("2005");
	port4.push_back("2001");
	
	list<string> port5;
	port5.push_back("2001");
	port5.push_back("2002");
	map_reps.insert(pair<int , list<string>>(2001, port1));
	map_reps.insert(pair<int , list<string>>(2002, port2));
	map_reps.insert(pair<int , list<string>>(2003, port3));
	map_reps.insert(pair<int , list<string>>(2004, port4));
	map_reps.insert(pair<int , list<string>>(2005, port5));
}

int main(){

	init_reps();	
	
	int sockfd, client_sock, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 3000;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0) 
	      error("ERROR on binding");
	listen(sockfd,1000);
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


