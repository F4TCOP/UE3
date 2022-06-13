#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //dup2
#include <sys/socket.h> //include for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

//inet_addr("127.0.0.1") converts a IP4 ADdress from a string to a dotted decimal

#include "array.h"
#include "shell.h"

#define PORT 9000
#define MAX_SIZE 256

typedef struct {
	char key;
}newchar;

typedef struct {
	char * key;
}newstring;

static inline void error (const char *msg){
	perror(msg);
	exit(-1);
}

void write_file_from_client(int sfd, char* filename){
	FILE* p_file;
	p_file = fopen("test.txt", "w");
	if(p_file == NULL){
		error("Error opening file");
		}else{
		printf("Opened file %s\n", filename);
		}
	char buf[MAX_SIZE];
	while(1){
		int n = recv(sfd, buf, (size_t)MAX_SIZE, 0);
		printf("%s", buf);
		if(n<0){
			break;
		}
		fwrite(&buf, sizeof(char), sizeof(buf), p_file);
		memset(&buf, 0, MAX_SIZE);
	}

	printf("Completed writing file");
}

void* shell_connection( void* p_cfd){
	int cfd = *((int*)p_cfd);
	free(p_cfd);
	char buf[256];
	int MAX_LENGTH = 256;
	//dup2(cfd, 0); Hat nicht funktioniert
	//dup2(cfd, 1);
	
	while(1){
		recv(cfd, buf, (size_t)MAX_LENGTH, 0);
		//printf("%s", buf);

		//deletes newline char from fgets
   		size_t len = strlen(buf);
    	if (buf > 0 && buf[len-1] == '\n'){
        	buf[--len] = '\0';
    		}
		
		//tokenize input
		char** token;
		arrayInit(token);
		split(buf, " ", token);

		if (strcmp(token[0], "exit") == 0){
			
			break;

		}else if(strcmp(token[0], "cd") == 0){
			

		}else if(strcmp(token[0], "get") == 0){
			
			
		}else if(strcmp(token[0], "put") == 0){

			write_file_from_client(cfd, token[1]);			
			
		}else{
			//run the command
			

		}
	}
	close(cfd);
}

int main() {

	//inits srv_addr and declares cli_addr
	struct sockaddr_in srv_addr, cli_addr;

	srv_addr.sin_family = AF_INET; //address family, type of address -> socket braucht immer ein type mit dem es kommuniziert -> AF_INET = IPv4
	srv_addr.sin_port = htons(PORT); //htons -> something with left byte swapped and network byte order
	srv_addr.sin_addr.s_addr = INADDR_ANY; //address we agree to accept -> in diesem case alle
	
	//Same but for better read
	socklen_t sockaddr_sz = sizeof(struct sockaddr_in);
	

	//server/socket file descriptor, client file descriptor
	int sfd, cfd, client_count;

	//opens a socket of domain AF_INET (IPv4)
	// type = SOCK_STREAM (bidirectional connection mode)
	//protocoll = 0 -> wählt standard für diese art von socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd<0){
		error("Couldnt open Socket");
	}
	//socket, level = SOL_SOCKET -> übernimmt das protocol des sockets
	//option ->
	//option value ->
	//option len ->
	//Wofür werden die beiden letzten Parameter gebraucht?
	int sockopt = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*) &sockopt, sizeof(sockopt));
	
	//binds a addr to a socket in our case a IPv4 addr to a socket which looks for IPv4
	if (bind(sfd, (struct sockaddr*) &srv_addr, sockaddr_sz) < 0){
		error("Couldnt bind socket");
	}

	//Maximum Connections 50
	int ret = listen(sfd, 50);
	if (ret < 0){
		error("Couldnt listen for connections on the socket");
	} else if (ret == 0){
		printf("Server Listening\n");
	}	
	client_count = 0;
	//Server Loop -> Accepting Clients and Handling the Connection
	while (1) {
		printf("[srv]: idle\n");
		sleep(2);
				
		cfd = accept(sfd, (struct sockaddr*) &cli_addr, &sockaddr_sz);
		
		if (cfd<0){
			error("Couldnt accept incoming connection");
		}
		client_count++;
		printf("Connection accepted from %s:%d in total %d connections\n", 
					inet_ntoa(cli_addr.sin_addr), 
					ntohs(cli_addr.sin_port),
					client_count);
		
		pthread_t client_thread;

		//NULL = Standard Einstellungen
		//Wieso kann ich hier nicht &cfd und muss extra ein Ptr dafür machen?
		int* p_cfd = malloc(sizeof(int));
		*p_cfd = cfd;
		pthread_create(&client_thread, NULL, shell_connection, p_cfd);
		pthread_exit(&client_thread);
		client_count--;
	}
	
	close(sfd);
	
	return 0;
}
