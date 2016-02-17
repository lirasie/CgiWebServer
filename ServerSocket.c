#include "CgiServer.h"

int serverOpen(char* port){
	int getaddr_status;
	int servSock;
	struct addrinfo hints, *servinfo, *p;
	int one = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//IPv4, IPv6
	hints.ai_socktype = SOCK_STREAM;	//TCP
	hints.ai_flags = AI_PASSIVE;		//my IP

	if( ( getaddr_status = getaddrinfo(NULL, port, &hints, &servinfo) ) != 0){
		//get address error
		fprintf(stderr, "get address info error: %s", gai_strerror(getaddr_status) );
		return -3;
	}

	for(p = servinfo; p != NULL; p = p->ai_next){
		servSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if(servSock < 0){
		//socket errer
			perror("socket error");
			continue;
		}

		setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

		if( bind(servSock, p->ai_addr, p->ai_addrlen) < 0){
			perror("bind error");
			continue;
		}
		break;
	}

	freeaddrinfo(servinfo);

	if(p == NULL){
		exit(1);
	}

	if( listen(servSock, BACKLOG) <0 ){
		perror("listen error");
		exit(1);
	}

	return servSock;
}

int recvRequest(int connSock){
	char rcvmsg[RCVLEN]={0,};
	Request* request;
	int rcvd;
	int status;

	rcvd = recv(connSock, rcvmsg, RCVLEN, 0);
	
	
	if(rcvd > 0){
		request = (Request*)malloc(sizeof(Request));
		request->list = (HeaderList*)malloc(sizeof(HeaderList));

		if( (status = parseRequest(rcvmsg, request)) != 200){
			httpRespondStatus(connSock, status);
			return rcvd;
		}

		if( (status = httpReqValid(request)) != 200){
			httpRespondStatus(connSock, status);
			return rcvd;
		}

		httpRespond(connSock, request);
		
	}

	close(connSock);
	return rcvd;
	
}

