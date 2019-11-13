// TCP client implemented by socket api
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(char* msg){
	fprintf(stderr, "%s/n", msg);
	exit(1);
}

int main(int argc, char* argv[]){
	
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	char buffer[256];
	
	if(argc < 3){
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(1);
	}
	
	// set the port
	portno = atoi(argv[2]);
	// create socket fd
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) error("Error: Opening socket.");

	// set the connect parameter
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port =htons(portno);
	
	// connect to server
	if(connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) error("Error: Connecting.");
	printf("Enter the message: ");
	memset(buffer, 0, 256);
	fgets(buffer, 255, stdin);

	// send to server
	n = write(sockfd, buffer, strlen(buffer));
	if(n < 0) error("Error: fail to write.");

	// receive msg from server
	memset(buffer, 0, 256);
	n = read(sockfd, buffer, 255);
	if(n < 0) error("Error: fail to reveive data");

	printf("Message receive from server: \n%s\n", buffer);
	return 0;
}
