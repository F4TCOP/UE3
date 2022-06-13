#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //dup2
#include <sys/socket.h> //include for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "array.h"


#define PORT 9000
#define HOST "127.0.0.1"
#define MAX_SIZE 256



static inline void error (const char *msg){
	perror(msg);
	exit(-1);
}

char** split(char *str, const char *sep, char **arr) {
    arrayPush(arr) = strtok(str, sep);

    while (arrayTop(arr) != NULL)
        arrayPush(arr) = strtok(NULL, sep);

    (void) arrayPop(arr);
    
    return arr;
}

void send_file_to_server(FILE* p_file, int sfd){
	char buf[MAX_SIZE] = {0};

	while(fgets(buf, MAX_SIZE, p_file)!= NULL){ //NULL oder EOF? Lieber EOF aber wie genau
		if(send(sfd, buf, sizeof(buf), 0) == -1){
			error("Sending File to Server");
		}
		memset(&buf, 0, MAX_SIZE);
	}
	printf("Completed sending file");
}




int main() {

	struct sockaddr_in cliaddr;

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(PORT);
	cliaddr.sin_addr.s_addr = inet_addr(HOST);

	printf("Clients Port is %i\n", ntohs(cliaddr.sin_port));

	char buf [256];
	
	size_t MAX_LENGTH = sizeof(buf);
	int sfd;

	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sfd< 0){
		error("Couldnt open socket");
	}

	int ret = connect(sfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr));
	if (ret < 0){
		error("Couldnt connect to socket");
	} else if (ret == 0){
		printf("Client connected to server\n");
	}

	//Client Loop
	while(1){
		printf("$>");
		fgets(buf, (int)MAX_LENGTH, stdin);

		//deletes newline char from fgets
   		size_t len = strlen(buf);
    	if (buf > 0 && buf[len-1] == '\n'){
        	buf[--len] = '\0';
    		}
		//WTF was macht strtok damit ey scheisse!!!!!!
		send(sfd, buf, MAX_LENGTH, 0);

		//Senden der Message aber schicke ich die immer? oder lieber in den if Abfragen?
		//send(clifd, buf, MAX_LENGTH, 0);

		//tokenize input
		char** token;
		arrayInit(token);
		split(buf, " ", token);

		if (strcmp(token[0], "exit") == 0){
			close(sfd);
			printf("Client disconnected");
			exit(EXIT_SUCCESS);

		}else if(strcmp(token[0], "cd") == 0){
			//muss ich eigentlich nicht extra abfragen
			send(sfd, buf, MAX_LENGTH, 0);

		}else if(strcmp(token[0], "get") == 0){
			send(sfd, buf, MAX_LENGTH, 0);
			
		}else if(strcmp(token[0], "put") == 0){

			//Das soll als rdy up für server gelten damit er weiß was passiert
			
			char* filename = token[1];
			FILE* p_file;
			p_file = fopen(filename, "r");
			if(p_file == NULL){
				error("Error opening file");
			}
			send_file_to_server(p_file, sfd);
			
		}else{
			//run the command
			send(sfd, buf, MAX_LENGTH, 0);

		}
		
	}

	getc(stdin);
	printf("Client Exit\n");
	
	close(sfd);
	return 0;
}
