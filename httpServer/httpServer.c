// The simple http server implement based on TCP socket
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENT_COUNT 5

void error(char *);
void reuseAddr(int);
int startTCPServer(int);


int main(int argc, char * argv[]){
	
	// catch the input port
	if(argc < 2) error("Error: wrong input, start the server with port number");
	int portNum = atoi(argv[1]);
	if (portNum != 80)&&(portNum < 1024 || portNum > 65535){
		error("Error: the port should be 80 or 1024~65535");
	}

	// open the TCP socket for TCP server
	int httpSocket = startTCPServer(portNum);

	// start the server
	while(1){
		struct sockaddr_in browserSocketAddr;
		int browserLen = sizeof(browserSocketAddr);
		int browserSocket = accept(httpSocket,)
	}

	return 0;
}


// output the error message and exit the program
void error(char * msg){
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

// reset the port when it can not be used yet
void reuseAddr(int sockfd){
	int on = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret < 0){
		error("Error: fail to setsockopt.");
	}
}

// start the TCP server by return a sockfd
int startTCPServer(int portNum){

	// get the sockfd
	int httpSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error: can not create socket, \n the error number is %d\n", errno);
	}

	// bind the port
	struct sockaddr_in tcpServerSockAddr;
	memset(&tcpServerSockAddr, 0, sizeof(tcpServerSockAddr));
	tcpServerSockAddr.sin_family = AF_INET;
	tcpServerSockAddr.sin_port = htons(portNum);
	tcpServerSockAddr.sin_addr.s_addr = 0;  // 0.0.0.0 for local

	if(bind(httpSockfd, (struct sockaddr*)&tcpServerSockAddr, 
		sizeof(tcpServerSockAddr)) < 0){
		error("Error: can not bind the port %d,\n, errno is %d.", portNum, errno);
	}

	// listen
	if(listen(httpSockfd, MAX_CLIENT_COUNT) < 0){
		error("Error: can not listen on port %d,\n, errno is %d.", portNum, errno);
	}

	return httpSockfd;
}