#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 	10000
#define MAXLINE 128

void str_cli(int sockfd){
	
	char send_buf[MAXLINE];
	char recv_buf[MAXLINE];
	
	for(;;){
		memset(send_buf,0,sizeof(send_buf));				//initail send buffer to zero
        if(fgets(send_buf ,MAXLINE ,stdin) == NULL)			//read from stdin
			break;
        send(sockfd ,send_buf ,sizeof(send_buf) ,0);		//send the data to the other end

		//for(int i = 0 ; i < MAXLINE ;i++ )
		//	printf("send[%d] = %c(ascii%d)\n",i,send_buf[i],(int)send_buf[i]);

		printf("Hi server, I sends you %s",send_buf);
		
		memset(recv_buf ,0 ,sizeof(recv_buf));				//initail recv buffer to zero 
		recv(sockfd ,recv_buf ,sizeof(send_buf) ,0);		//recv the data from the other end
		fputs(recv_buf ,stdout);							//print the data to stdout				
    }		


}

int main (int argc ,char** argv ){
	
	int check;
	int sockfd;
	struct sockaddr_in serv_addr;

	if (argc != 2){											
		printf("Please enter 2 arguments\n");
		exit(EXIT_FAILURE);
	}
	
	//man 7 ip : linux ipv4 protocol implementation
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;					
	serv_addr.sin_port   = htons(PORT);
    //convert IPv4 address from text to binary
	//take argv[1] in here ,argv[1]:address of server
	check = inet_pton(AF_INET ,argv[1] ,&serv_addr.sin_addr.s_addr);	
	assert(check == 1);										//using assert to check program
	printf("connection success\n");

	//check connection status
	if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0){
		perror("connect :");
		exit(EXIT_FAILURE);	
	}
	
	str_cli(sockfd);										//Client does what he/she wants.
	exit(EXIT_SUCCESS);

}
