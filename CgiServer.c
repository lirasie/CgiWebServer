#include "CgiServer.h"

int main(int argc, char** argv){

	int pid;

	int servSock;
	int connSock;
	struct sockaddr_storage theirAddr;
	socklen_t addrSize = sizeof(theirAddr);
	int status;


	servSock = serverOpen(argv[1]);

	while(1){
		connSock = accept(servSock, (struct sockaddr *)&theirAddr, &addrSize);

		if(connSock < 0){
			perror("accept error");
			continue;
		}
		
		pid = fork();
		if( pid == -1){
			perror("fork error");
			continue;
		}
		else if( pid == 0){
			//child process
	
			if( recvRequest(connSock) < 0){
				perror("recieve error");
			}
			exit(0);
		}
		else{
			wait(&status);
			close(connSock);	
		}
	}	
		
	close(servSock);	
	return 1;
}