// file: TCP server implement by socket API
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(char * msg){
	// fprintf from stdio.h
	fprintf(stderr, "%s\n", msg);
	// exit from stdlib.h, stop the process immediately
	exit(1);
}


int main(int argc, char * argv[]){
	
	int sockfd; // file descriptor for server socket
	int newsockfd; // the new socket fd when accept client call
	int portno;
	int clilen;
	int n;
	char buffer[256];

	// struct to save the socket attrtube for server and client
	// sockaddr_in from netinet/in.h
	struct sockaddr_in serv_addr, cli_addr;	
	
	if(argc < 2) error("Error: No port provided.");

	// socket(int domain, int type, int protocol) from sys/type.h and sys/socket.h
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("Error: fail to open socket.");

	// memset() from string.h, set the array/struct memory
	memset(&serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[1]); // set the port, atoi() from stdlib.h, change str into int

	// set the server parameter
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno); // htons() from netinet/in.h, change int into net 
	serv_addr.sin_addr.s_addr = INADDR_ANY; // local IP address

	// bind and listen the socket
	// bind(int sockfd, struct sockaddr* addr, socklen_t addrlen) return 0 or -1
	if(bind(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		error("Error: Binding.");
	listen(sockfd, 5); // listen the socket on port
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
	if(newsockfd < 0) error("Error: On accept.");
	
	// accept message
	memset(buffer, 0, 256);
	n = read(newsockfd, buffer, 255);
	if(n < 0) error("Error: reading from socket.");
	printf("Client message: %s\n", buffer);
	// send message
	n = write(newsockfd, "Got the message", 15);
	if(n < 0) error("Error: writing to socket.");
	
	close(newsockfd);
	close(sockfd);
	
	return 0;
}
