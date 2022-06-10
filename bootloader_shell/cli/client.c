#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9000
#define HOST "127.0.0.1"

static inline void error (const char *msg){
	perror(msg);
	exit(-1);
}

int main() {

	struct sockaddr_in cliaddr;

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(PORT);
	cliaddr.sin_addr.s_addr = inet_addr(HOST);

	printf("Clients Port is %i\n", ntohs(cliaddr.sin_port));

	char buf [256];
	int clifd;

	clifd = socket(AF_INET, SOCK_STREAM, 0);

	if (clifd < 0){
		error("Couldnt open socket");
	} else {
		printf("Client FD is %i\n", clifd);
	}

	int ret = connect(clifd, (struct sockaddr*) &cliaddr, sizeof(cliaddr));
	if (ret < 0){
		error("Couldnt connect to socket");
	} else if (ret == 0){
		printf("Client connected to server\n");
	}

	//Datentransfer mit read/write 

	for(int i = 0; i < 5; ++i) {
		if (write(clifd, "Ping", 4) < 0)
			error("Couldnt send message");

		if (read(clifd, buf, sizeof(buf)) < 0)
			error("Couldnt recv message");
		
		printf("[recv] %s\n", buf);
	}










	getc(stdin);
	printf("Client Exit\n");
	
	close(clifd);
	return 0;
}
