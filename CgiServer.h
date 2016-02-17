#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define URILEN 100
#define RCVLEN 4096
#define RESLEN 4096

#define CODE200 "HTTP/1.1 200 OK\r\n"
#define CODE400 "HTTP/1.1 400 Bad Request\r\n"
#define CODE404 "HTTP/1.1 404 Not Found\r\n"
#define CODE501 "HTTP/1.1 501 Bad Gateway\r\n"

#define BACKLOG 500

#define AUTH_TYPE "auth-scheme"
#define CONTENT_LENGTH "content-Length"

typedef struct headerList{
	char* header;
	struct headerList* next;
}HeaderList;

typedef struct Header{
	char* method;
	char* URI;
	char* httpVersion;
	struct headerList* list;
}Request;

int serverOpen(char* port);
int recvRequest(int connSock);
int parseRequest(char* rcvmsg, Request* request );
int needCGI(Request* header);
int httpReqValid(Request* request);
int httpRespondStatus(int connSock, int status);
int httpRespondBody(int connSock, FILE* fp);
int httpRespond(int connSock, Request* header);
int existFile(int connSock, char* URI);
int createCGIrequest(Request* request, int connSock);
int execCGI(char* path, char* cgiRequest, int connSock);
int getRequestCGI(Request* request);
int postRequestCGI(Request* request, char* cgiRequest);
int execCGI(char* path, char* cgiRequest, int connSock);

