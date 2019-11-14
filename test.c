// for code testing
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char * argv[]){

	struct sockaddr_in sock;

	sock.sin_addr.s_addr = inet_addr("132.241.5.10");
	printf("%s\n", inet_ntoa(sock.sin_addr));
	return 0;
}