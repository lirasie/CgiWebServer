#include "CgiServer.h"

int createCGIrequest(Request* request, int connSock){
	char* cgiRequest;
	int contentLen;
	//set environment variable
	char* msgBody;
	char* headers;
	char path[URILEN] = {0,};
	char rcvmsg[RCVLEN+1] = {0,};
	int i=0;
	int tmprcv=0;
	char* tmp = NULL;
	int queryPoint;

	HeaderList* list = request->list;
	
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("SERVER_NAME", "kimkm1015", 1);
	setenv("SERVER_PORT", "8979", 1);
	setenv("SERVER_SOFTWARE", "kimkm1015/1.0", 1);
	setenv("REQUEST_METHOD", request->method, 1);
	setenv("SERVER_PROTOCOL", request->httpVersion, 1);

	strcat(path, ".");

	if( strncmp(request->method,"POST\0",5) == 0){
		contentLen = postRequestCGI(request, cgiRequest);
		strcat(path, request->URI);
	}
	else{
		queryPoint = getRequestCGI(request);
		for(i = 0; i < queryPoint; i++){
			path[i+1] = request->URI[i];
		}
	}

	while(list->next != NULL){	
		list = list->next;			
	}

	cgiRequest=	(char*)malloc(sizeof(char)*(2*contentLen+1));
	memset(cgiRequest, 0, 2*contentLen+1);

	i = 0;
	if(contentLen > RCVLEN){

		i = strlen(list->header);
		strncpy(cgiRequest, list->header, i);
		while( i < contentLen){	//server가 오와라나이..
			tmprcv = recv(connSock, rcvmsg, RCVLEN, 0);

			
			i += tmprcv;
			strcat(cgiRequest, rcvmsg);
			printf("***********\n%s\n*******\n",cgiRequest);
			printf("::::::::::::::::%d||%d::::::::::::\n",contentLen, i);
			memset(rcvmsg, 0, RCVLEN);
		}
	}
	else{
		strncpy(cgiRequest, list->header, contentLen);
		printf("*******************\n%s\n******************\n",cgiRequest);
	}
	
	execCGI(path, cgiRequest, connSock);

}

int getRequestCGI(Request* request){

	char* query;

	int i = 0;	
	
	for(i = 0; i<strlen(request->URI); i++){
		if( request->URI[i] == '?'){
			query = &(request->URI[i]);
			break;
		}
		
	}

	setenv("QUERY_STRING", query, 1);

	return i;

}

int postRequestCGI(Request* request, char* cgiRequest){
	int contentLen;
	HeaderList* list = request->list;
	char* tmp;	

	setenv("QUERY_STRING","",1);

	while(list != NULL){
		if( (strncmp(list->header, "Content-Length", 14)) == 0){
			tmp = strchr(list->header, ' ');
			tmp = tmp+1;
			contentLen = atoi(tmp);
			break;
		}	
		list = list->next;
	}
	setenv("CONTENT_LENGTH", tmp, 1);

	return contentLen;
}

int execCGI(char* path, char* cgiRequest, int connSock){
	int child;
	int inputPipe[2];
	int outputPipe[2];
	char resmsg[RESLEN]={0,};
	int resbyte;
	int status;

	pipe(inputPipe);
	pipe(outputPipe);

	child = fork();

	if(child == -1){
		//fork error
		perror("fork error : ");
	}
	else if(child == 0){
		//child process
		close(inputPipe[1]);
		close(outputPipe[0]);

		
		dup2(inputPipe[0], 0);
		dup2(outputPipe[1], 1);
		
		close(inputPipe[0]);
		close(outputPipe[1]);
		
		if(execlp(path, path, NULL) == -1)
			printf("open fail");

		exit(0);
		
	}
	else{
		//parent process
		close(inputPipe[0]);
		close(outputPipe[1]);

		write(inputPipe[1], cgiRequest, (strlen(cgiRequest)));

		close(inputPipe[1]);
		httpRespondStatus(connSock, 200);
		while( (resbyte = read(outputPipe[0], resmsg, RESLEN)) > 0){
			send(connSock, resmsg, resbyte, 0);
		}
		close(outputPipe[0]);
		wait(&status);
		close(connSock);
		printf("%s\n",cgiRequest);
		printf("%d\n", resbyte);
	}

	return 1;
}
