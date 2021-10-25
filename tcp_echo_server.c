#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define PORT 				10000
#define MAX_CONNECTION 		5
#define MAXLINE				128

/*handle child process terminated*/
void sigchld_handler(int signo){
	pid_t pid;
	int   stat;
	
	while((pid = waitpid(-1 ,&stat ,WNOHANG )) > 0)				//wait child process to prevent zombies 
		printf("child %d terminated\n", pid);					
	return;
}		

void str_echo(int sockfd){
	char recv_buf[MAXLINE];

	for(;;){
		memset(recv_buf ,0 ,sizeof(recv_buf));					//initail recv_buf
        recv(sockfd ,recv_buf ,sizeof(recv_buf) ,0); 			//recv the data from the other end 
        
		printf("Hi client, I received %s",recv_buf);
		//for(int i = 0 ; i < 128 ; i++)
		//	printf("recv[%d] = %c(ascii %d)\n",i,recv_buf[i],(int)recv_buf[i]);
			
	
		send(sockfd ,recv_buf ,sizeof(recv_buf) ,0);			//send the data to the other end 
		printf("OK, I send you back %s",recv_buf);
	}
}

int main(int argc,char** argv){
	
	pid_t child_pid;
	int listen_fd;
	int connect_fd;
	
	socklen_t cli_len;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	

	//bind the protocol to socket 
	//cast the ptr to generic socket address structure
	if(bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind :");
		exit(EXIT_FAILURE);
	}

	//listening, and define the action when catching SIGCHLD signal
	listen(listen_fd,MAX_CONNECTION);
	signal(SIGCHLD ,sigchld_handler);	

	while(1){	
	
		cli_len = sizeof(cli_addr);		
		connect_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);

		if((child_pid = fork()) == 0){
			close(listen_fd);			//child closes listen socket	
			str_echo(connect_fd);		//child does his/her task
			close(connect_fd);			//child finished its task
			exit(EXIT_SUCCESS);
		}

		close(connect_fd);				//parent close connected socket
	}
	
}
