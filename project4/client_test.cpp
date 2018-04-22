#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;
void error(const char *msg)
{
    perror(msg);
    exit(0);
}


static map<string, map<std::string, list<string>>> reps;

void parse_rep(char buffer[]){
	char port[20];
	string key , value;

	bzero(port , 20);
	strncpy(port, buffer + 1 , 4);
	
	map<string, list<string>> map_rep;
	int i = 5;
	int flag = 0 , dump = 0;
	while(buffer[i] != ';'){
		key = "";
		list<string> cart;
		

		while(buffer[i] != '.'){
			if(buffer[i] == ','){
				flag++;
				dump = 1;	
				
			}
			if(flag == 0){
				key.push_back(buffer[i]);
			}else if(flag == 1 && buffer[i] != ','){
				value.push_back(buffer[i]);
			}else if(flag == 1 && buffer[i] == ','){
			
				cart.push_back(value);
				cout<<"Key "<<key<<" Value "<<value<<endl;
			}
			i++;
		}
		
		map_rep.insert(pair<string , list<string>>(string(key), cart));
		
		flag = 0;
		value = "";
		i++;
		//cout<<buffer[i++]<<endl;
	}
	reps.insert(pair<string , map<string , list<string>>>(string(port) , map_rep));
	cout<<"port "<<port<<endl;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
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
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
  	
   	//buffer[strlen(buffer)] = ';'; 
   	//parse_rep(buffer);	 
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
