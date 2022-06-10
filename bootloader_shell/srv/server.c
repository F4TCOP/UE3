#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> //include for sockets
#include <netinet/in.h>
#include <arpa/inet.h>

//inet_addr("127.0.0.1") converts a IP4 ADdress from a string to a dotted decimal



#define PORT 9000

static inline void error (const char *msg){
	perror(msg);
	exit(-1);
}

int main() {

	//inits srv_addr and declares cli_addr
	struct sockaddr_in srv_addr, cli_addr;

	srv_addr.sin_family = AF_INET; //address family, type of address -> socket braucht immer ein type mit dem es kommuniziert -> AF_INET = IPv4
	srv_addr.sin_port = htons(PORT); //htons -> something with left byte swapped and network byte order
	srv_addr.sin_addr.s_addr = INADDR_ANY; //address we agree to accept -> in diesem case alle
	
	socklen_t srvaddr_sz = sizeof(struct sockaddr_in);
	socklen_t cliaddr_sz = sizeof(cli_addr);

	printf("clisize: %i\n", cliaddr_sz);
	printf("srvsize: %i\n", srvaddr_sz);
	
	//server/socket file descriptor, client file descriptor
	int sfd, cfd;
	ssize_t bytes;
	char buf[256];

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
	if (bind(sfd, (struct sockaddr*) &srv_addr, srvaddr_sz) < 0){
		error("Couldnt bind socket");
	}
	int ret = listen(sfd, 50);
	if (ret < 0){
		error("Couldnt listen for connections on the socket");
	} else if (ret == 0){
		printf("Server Listening\n");
	} else {
		printf("ret: %i", ret);
	}
	
	

	//Datentransfer mt read/write der file descriptors

	while (1) {
		printf("[srv]: idle\n");
		sleep(2);
		
		
		cfd = accept(sfd, (struct sockaddr*) &cli_addr, &srvaddr_sz);
		
		if (cfd<0){
			error("Couldnt accept incoming connection");
		}
		int cli_port = ntohs(cli_addr.sin_port);
		printf("Connection accepted from %s:%d\n", 
					inet_ntoa(cli_addr.sin_addr), 
					cli_port);

		while ((bytes = read(cfd, buf, sizeof(buf)))) {
			if (bytes < 0)
				error("Couldnt recv message");
			
			if (write (cfd, buf, bytes) < 0)
				error("Couldnt send message");
		}
	}
	
	close(sfd);
	close(cfd);
	return 0;
}
