#include "CgiServer.h"


int parseRequest(char* rcvmsg, Request* request ){		//parsing, return status or valid

	int reqCheck = 0;
	char* headerCheck = NULL;
	char* tmp;
	
	HeaderList* list = request->list;

    headerCheck = strstr(rcvmsg, "\r\n\r\n");
    if(headerCheck == NULL){
        //bad request header
        return 400;
    }
    headerCheck = headerCheck+2;

    tmp = rcvmsg;
    request->method = tmp;

    tmp = strchr(tmp,' ');
    *tmp = '\0';
    tmp = tmp+1;
    request->URI = tmp;

    tmp = strchr(tmp, ' ');
    *tmp = '\0';
    tmp = tmp+1;
    request->httpVersion = tmp;

    while(1)
    {
        tmp = strchr(tmp, '\r');
        *tmp = '\0';
        tmp = tmp+2;
        list->header = tmp;
        if(tmp == headerCheck){
			list->header = (list->header)+2;
            break;
		}
        list->next = (HeaderList*)malloc(sizeof(HeaderList));
        list = list->next;
        list->next = NULL;
    }

	return 200;

}

int httpReqValid(Request* request){		//parsing, return status or valid

	int reqCheck = 0;

	if(strcmp(request->method, "GET") != 0 && strcmp(request->method, "POST") != 0)
		return 400;

	if(strncmp(request->httpVersion, "HTTP/1.0", 8)==0 || strncmp(request->httpVersion, "HTTP/1.1", 8) == 0)
		reqCheck = 1;

	int i;
	int reqURI;
	char* check;

	reqURI = strlen(request->URI);

	if(reqURI > URILEN){
		//request URI too long
		return 400;
	}

	check = request->URI;

	for(i=0; i<reqURI-1; i++){
		
		if( (request->URI)[i] == '/' && (request->URI)[i+1] == '/'){
			check = &(request->URI[i+1]);		
		}
		else
			break;
	}

	request->URI = check;
	if( strncmp( request->URI, "/\0", 2) == 0){
		//default page: index.html
		
		request->URI = "/index.html";
	}
	
	printf("%s\n", request->URI);

	if( (check = strstr(request->URI, "../")) != NULL){
		return 400;
	}

	return 200;

}

int needCGI(Request* header){
	
	if( (strncmp(header->method,"POST\0",5) == 0) || (strstr(header->URI,"?") != NULL) ){
		return 1;
	}
	
	return 0;
	
}



