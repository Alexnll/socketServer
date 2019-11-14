// The simple http server implement based on TCP socket
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENT_COUNT 5
#define RECV_BUF_SIZE 512
#define SEND_BUF_SIZE 512


void error(char *);
void reuseAddr(int);
int startTCPServer(int);
void * responseBrowserRequest(void *);
int getOneLineFromSocket(int, char *, int);
void responseStaticFile(int, int, char *, char *)

// request method
enum RequestType{REQUEST_GET, REQUEST_POST, REQUEST_UNDEFINED};


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
		
		// accept new client
		struct sockaddr_in browserSocketAddr;
		int browserLen = sizeof(browserSocketAddr);
		int browserSocket = accept(httpSocket, 
			(struct sockaddr*)&browserSocketAddr, &browserLen);
		if(browserSocket < 0) error("Error: fail to accept, errno is %d.", errno);

		printf("%s:%d linked!\n", inet_ntoa(browserSocketAddr.sin_addr), 
			browserSocketAddr.sin_port);

		// creat new thread to deal with request
		pthread_t responseThread;
		int threadReturn = pthread_create(&responseThread, 
			NULL, responseBrowserRequest, &browserSocket);
		if(threadReturn) error("Error: fail to create thread, erron is %d.", errno);

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
	if(httpSockfd < 0){
		error("Error: can not create socket, \n the error number is %d\n", errno);
	}

	// bind the port
	struct sockaddr_in tcpServerSockAddr;
	memset(&tcpServerSockAddr, 0, sizeof(tcpServerSockAddr));
	tcpServerSockAddr.sin_family = AF_INET;
	tcpServerSockAddr.sin_port = htons(portNum);
	tcpServerSockAddr.sin_addr.s_addr = 0;  // 0.0.0.0 for local
	
	reuseAddr(httpSockfd);
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


// accept a socket to receive the data from browser, return a http package
void * responseBrowserRequest(void * ptr){

	// get the browser socket
	int browserSocket = *(int*)ptr;
	char recvBuf[RECV_BUF_SIZE + 1] = {0};	
	int contentLength = 0;
	RequestType requestType = REQUEST_UNDEFINED;

	// define the memory block for saving the requested file
	#define FILE_PATH_LENGTH 128
	char requestFilePath[FILE_PATH_LENGTH] = {0};

	// define the memory block for saving the query string
	#define QUERY_STRING_LENGTH 128
	char requestQueryString[QUERY_STRING_LENGTH] = {0};

	// see if the file requested is executed 	
	int isXFile = 0;

	// read the received http package header
	while(getOneLineFromSocket(browserSocket, recvBuf, RECV_BUF_SIZE)){

		if(strcmp(recvBuf, "\n") == 0) break;

		// get the request method
		if(requestType == REQUEST_UNDEFINED){

			int pFileName = 0;
			int pQuerySring = 0;
			int pRecvBuf = 0;
			// for GET
			if(strncmp(recvBuf, "GET", 3) == 0){

				requestType = REQUEST_GET;
				pRecvBuf = 4;

			} else if(strncmp(recvBuf, "POST", 4) == 0){

				requestType = REQUEST_POST;
				pRecvBuf = 5;

			}

			// extract the request url/path
			if(pRecvBuf){
				requestFilePath[pFileName++] = ".";

				// copy the file name in the url
				while(pFileName < FILE_PATH_LENGTH && recvBuf[pRecvBuf]
					&& recvBuf[pRecvBuf] != ' ' && recvBuf[pRecvBuf] != '?'){
					requestFilePath[pFileName++] = recvBuf[pRecvBuf++];
				}

				// copy the query string
				if(pFileName < FILE_PATH_LENGTH && recvBuf[pRecvBuf] == '?'){
					++pRecvBuf;
					while(pQuerySring < QUERY_STRING_LENGTH &&
						recvBuf[pRecvBuf] && recvBuf[pRecvBuf] != ' '){
						requestQueryString[pQuerySring++] = recvBuf[pRecvBuf++];
					}
				}
			}

		}else if(requestType == REQUEST_GET){

		}else if(requestType == REQUEST_POST){
			if(strncmp(recvBuf, "Content-Length:", 15) == 0){
				contentLength = atoi(recvBuf+15);
			}
		}
	}

	// if GET or UNDEFINED, do not read the content
	// for POST, rend the content with length as contentLength
	if(requestType == REQUEST_POST && contentLength){
		if(contentLength > QUERY_STRING_LENGTH){
			fprintf(stderr, "Query string buffer is smaller than content length.\n");
			contentLength = QUERY_STRING_LENGTH;
		}
		read(browserSocket, requestQueryString, contentLength);
	}

	// judge if the request file is a dictionary
	struct stat fileInfo;
	stat(requestFilePath, &fileInfo);
	if(S_ISDIR(fileInfo, st_mode)){
		// is a dictionary
	}else{
		// not a dictionary
		if(access(requestFilePath, X_OP) == 0){
			isXFile = 1;
		}
	}

	// deal with different request based on the request method 
	switch(requestType){

		case REQUEST_GET:
			// excute or not
			if(isXFile == 0){
				responseStaticFile(browserSocket, 200, requestFilePath, NULL);
			}else{

			}
			break;

		case REQUEST_POST:

			if(contentLength == 0){
				responseStaticFile(browserSocket, 400, "./err400.html", "text/html");
				break;
			}

			beak;

		case REQUEST_UNDEFINED:
			responseStaticFile(browserSocket, 501, "./err501.html", "text/html");
			break;

		default:
			break;
	}

	close(browserSocket);
	return NULL;
}


// read one line from the sockfd, with buf as bufLength, end with \r\n
int getOneLineFromSocket(int sockfd, char * buf, int bufLength){

	int byteCount = 0;
	char tmpChar;
	memset(buf, 0, bufLength);

	// read one char once
	while(read(sockfd, &tmpChar, 1) && byteCount < bufLength){

		if(tmpChar=='\r'){
			if(recv(sockfd, &tmpChar, 1, MSG_PEEK) < 0){
				error("Error: fail to recv char after \\r.");
			}

			// if \n follow \r, means end of one line
			if(tmpChar=='\n' && byteCount < bufLength){
				read(sockfd, &tmpChar, 1);
				buf[byteCount++] = '\n';
				break;
			}

			buf[byteCount++] = '\r';
		} else {
			buf[byteCount++] = tmpChar;
		}
	}

	return byteCount;
}

// send the static file to the browser socket
void responseStaticFile(int sockfd, int returnNum, char* filePath, 
	char* contentType){

	char sendBuf[SEND_BUF_SIZE] = {0};

	if(strcmp(filePath, "./") == 0){
		// did specific the file name, return index.html
		filePath = "./index.html";
	}
}