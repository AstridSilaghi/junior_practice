#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> 
#include <sys/time.h>

#define PORT 8080
#define N 20
#define DIR "files"

int main(int argc, char *argv[]){

	int server_socket_fd, new_socket_fd;
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	int buff_size = 100, data_len = 30;
	char *buff = (char*) malloc(buff_size * sizeof(char));
	int port, recv_size, activity = 0, sent_size;
	FILE *f;
	int ;
	char *data = (char*) malloc(data_len *sizeof(char));
	fd_set readfds;
	int client_socket[N], max_clients = N, i, max_sd, sd;
	struct timeval tv;
	tv.tv_sec = 20;  

	if(argc != 2){	
		printf("Not enough args. Usage: %s <port>\n", argv[0]);
		return -1;
	}

	if(chdir(DIR) != 0){
		perror("err moving to folder\n");
		return -1;
	}

	for(i = 0; i < max_clients; i++){
		client_socket[i] = 0;

	}
	
	if((server_socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == 0){
		return -1;
	}	
	
	memset(&addr, '0', sizeof(addr));
	memset(buff, 0, buff_size);

	port = atoi(argv[1]);
	addr.sin_family =AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	
	if(bind(server_socket_fd, (struct sockaddr *)&addr, 
                                 sizeof(addr)) < 0){
		perror("err bind\n");
		return -1;
	}

	if (listen(server_socket_fd, 5) < 0){
		perror("err listen\n");
		return -1;
	}

	printf("Server started to listen\n");
	setsockopt(server_socket_fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

	while(1){
		FD_ZERO(&readfds);
	
		FD_SET(server_socket_fd, &readfds);
		max_sd = server_socket_fd;
		
		for(i = 0; i < max_clients; i++){
			sd = client_socket[i];
			if(sd >= 0){
				FD_SET(sd, &readfds);			
			}
			if(sd > max_sd){
				max_sd = sd;			
			}
		}
		if((activity = select(max_sd+1, &readfds, NULL, NULL, NULL)) < 0){
			perror("select err\n");
		}

		if(FD_ISSET(server_socket_fd, &readfds)) {
			if ((new_socket_fd = accept(server_socket_fd, (struct sockaddr *)&addr, 
	               					(socklen_t*)&addrlen)) < 0){
				perror("err accept\n");
		    	}
			printf("The connection to server is opened from socket %d\n", new_socket_fd);

			for(i = 0; i < max_clients; i++){
				if(client_socket[i] == 0){
					client_socket[i] = new_socket_fd;
					break;
				}
			}
		}
		/* read data from client */
		for(i = 0; i < max_clients; i++){	
			sd = client_socket[i];

			if(FD_ISSET(sd, &readfds)){
				memset(data, 0, data_len);
				/* recv the data */ 
				if((recv_size = recv(sd, data, data_len, 0)) < 0){
					perror("err recv data\n");
				}
				data[strlen(data)] = '\0';
				
				
				/* client closed the connection */
				if(strstr(data, "quit") != NULL){
					printf("client is closed\n");
					close(client_socket[i]);
					client_socket[i] = 0;
					break;
				}else{

					printf("Fisierul este:%s\n", data);
					/* file does not exist */
					if((f = fopen(data, "r")) == NULL){
						printf("file does not exist\n");
						strcpy(buff, "File not found\n");
						if(send(sd, buff, strlen(buff), 0) != strlen(buff)){
							perror("err send\n");
						}	
					}else{
						/* send the file */
						while(!feof(f)){
							memset(buff, 0, buff_size);
							fread(buff, buff_size, 1, f);
							buff[strlen(buff)] = '\0';
							if(send(sd, buff, buff_size, 0) < 0){
								perror("err send\n");
							}
						}
			
						/* the file was sent */
						if((sent_size = write(sd, "finish", strlen("finish"))) < 0){
							perror("err send\n");
						}
						printf("am terminat de trimis\n");
						fclose(f);
					}
					
				}
				
			}	
		}
	}
	
	close(server_socket_fd);
	fclose(f);	
	printf("End of connection, server is closing...\n");	
	return 0;
}
