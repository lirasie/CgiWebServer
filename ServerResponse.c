#include "CgiServer.h"

int httpRespondStatus(int connSock, int status){
	
	char resmsg[RESLEN] = {0,};
	int sended = 0;
		
	switch(status){
		case 200:
			sended += send(connSock, CODE200, strlen(CODE200), 0);
			
			printf("200\n");
			break;

		case 400:
			sended += send(connSock, CODE400, strlen(CODE400), 0);		//status-line: 400 bad request
			sended += send(connSock, "\r\n", 2,0);						//CRLF

			sprintf(resmsg,"<html><body>400 Bad Request</body></html>");	
			sended += send(connSock, resmsg, strlen(resmsg), 0);		//message-body: bad request page
			printf("400\n");
			break;

		case 404:
			sended += send(connSock, CODE404, strlen(CODE404), 0);		//status-line: 404 not found
			sended += send(connSock, "\r\n", 2, 0);						//CRLF

			sprintf(resmsg, "<html><body>404 Not Found</body></html>");
			sended += send(connSock, resmsg, strlen(resmsg), 0);		//message-body: 404 page
			printf("404\n");
			break;
		
		case 501:
			sended += send(connSock, CODE501, strlen(CODE501), 0);		//status-line: 404 not found
			sended += send(connSock, "\r\n", 2, 0);						//CRLF

			sprintf(resmsg, "<html><body>501 Gateway...</body></html>");
			sended += send(connSock, resmsg, strlen(resmsg), 0);		//message-body: 404 page
			printf("501\n");
			break;
	
	}			

	return sended;

}

int httpRespondBody(int connSock, FILE* fp){

	int readCnt;
	int sended = 0;
	char resmsg[RESLEN] = {0,};
	
	while(!feof(fp)){
		memset(resmsg, 0, RESLEN);
		readCnt = fread(resmsg, sizeof(char), RESLEN, fp);
		sended += send(connSock, resmsg, readCnt, 0);				//message-body
	}
	sended += send(connSock,"\0",1,0);
	
	fclose(fp);
	
	return sended;

}

int httpRespond(int connSock, Request* header){

	int fileExist;
	int i;

	int cgi = needCGI(header);

	if(cgi){
		createCGIrequest(header, connSock);
		return 1;
	}

	else{
		existFile(connSock, header->URI);
		return 0;
	}

}	

int existFile(int connSock, char* URI){

	int sended = 0;
	FILE* fp;
	char path[URILEN] = {0,};

	strcpy(path, "./");
	strcpy(&path[2], URI);
	
	fp = fopen(path, "r");
	
	if( fp != NULL){	
		
		httpRespondStatus(connSock, 200);	
	
		FILE* pp;
		char pipeCmd[128] = {0,};
		char filePipe[48] = {0,};
		char* tmpPipe;
	
		sprintf(pipeCmd, "file -i %s", path);
		pp = popen(pipeCmd, "r");							//pipe open
		fgets(filePipe, 48, pp);
		strtok(filePipe, " ");
		tmpPipe = strtok(NULL, "\n");

		sended += send(connSock, "Content-Type: ", 14, 0);			//entity-header: Content-Type
		sended += send(connSock, tmpPipe, strlen(tmpPipe), 0);
		sended += send(connSock, "\r\n", 2, 0);						

		sended += send(connSock, "\r\n", 2, 0);						//CRLF

		httpRespondBody(connSock, fp);
	
	}
	else{
		//file open error
		
		httpRespondStatus(connSock, 404);
	}
	
	return sended;

}