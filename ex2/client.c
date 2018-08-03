#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

char *gen_clnt_qname(){
	char str[10];	
	char *q_name = (char*) malloc(QNAME_SIZE * sizeof(char));
	strcpy(q_name, CLIENT_QUEUE_NAME);
	sprintf(str, "%d", getpid());
	strcat(q_name, str);

	return q_name;
}

int main(){
	
	mqd_t qd_client;
	mqd_t qd_server;
	char *client_q_name;
	char msg[MSG_SIZE];
	char tmp[10];
	struct mq_attr attr;
 
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = 50;  
	attr.mq_curmsgs = 0;  

	printf("[Client]  Hello from client with queue : %s\n", client_q_name);

	client_q_name = gen_clnt_qname();
	if((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1){
		perror("error open server queue\n");
		exit(1);
	}

	if((qd_client = mq_open(client_q_name, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr)) == -1){
		perror("error open server queue\n");
		exit(1);
	}
	
	printf("Press ENTER for token\nPress q for exit\n");
	while(fgets(tmp, 10, stdin)){
		
		if(strcmp(tmp, "q\n") == 0){
			printf("Client closed.\n");
			mq_close(qd_client);
			mq_close(qd_server);
			exit(1);
		}
	
		if((mq_send(qd_server, client_q_name, strlen(client_q_name) + 1, 0)) == -1){
			perror("error send\n");
			continue;
		}
		printf("[Client]  After sending the queue name\n");

		if((mq_receive(qd_client, msg, MSG_SIZE, NULL)) == -1){
			perror("error receive from client\n");
			exit(1);	
		}
		
		printf("[Client]  Message received : %s\n\n", msg);
		printf("Press ENTER for token\nPress q for exit\n");
	}
	
	return 0;
}
