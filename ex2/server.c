#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#include "client.h"

#define SERVER_QUEUE_NAME   "/server_queue"
#define CLINET_QUEUE_NAME   "/client_queue_"
#define QNAME_SIZE 20
#define QUEUE_PERMISSIONS 0666

mqd_t qd_server;

void tstp_handler(){
	mq_close(qd_server);
	printf("Server is closing..\n");
	exit(1);
}

int main(){
	
	mqd_t qd_client;
	char *client_q_name = (char*) malloc(MSG_SIZE * sizeof(char));
	int i = 0;
	char msg[MSG_SIZE];
	int token_number = 0;

	struct mq_attr attr;  
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = 50;  
	attr.mq_curmsgs = 0;  
	
	printf("[Server]  Hello\n");

	if((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1){
		perror("error open server queue\n");
		exit(1);
	}

	while (1){

		signal(SIGTSTP, tstp_handler);
		if(mq_receive(qd_server, client_q_name, MSG_SIZE, NULL) == 1){
			perror("error receive\n");
			exit(1);
		}
		printf("[Server]  Recv queue name : %s from client\n", client_q_name);
		
		if((qd_client = mq_open(client_q_name, O_CREAT | O_RDWR)) == -1){
			perror("error open client queue\n");
			exit(1);	
		}
		printf("[Server]  Queue is opened\n");

		token_number++;
		sprintf(msg, "%d", token_number);
		if(mq_send(qd_client, msg, strlen(msg) + 1, 0) == -1){
			perror("err send\n");
			exit(1);
		} 
		printf("[Server]  Message sent to client.\n\n");	
		
		mq_close(qd_client);
	}

	return 0;
}
