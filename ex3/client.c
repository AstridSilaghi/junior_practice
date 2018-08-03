#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PORT 8080

/* Creeaza directorul clientului unde urmeaza sa se salveze
   fisierele trimise de la server */
int move_to_clnt_folder(){
	struct stat st;
	char *dir = (char*) malloc(30 * sizeof(char));
	char pid[10];

	strcpy(dir, "client_");
	sprintf(pid, "%d", getpid());
	strcat(dir, pid);
	strcat(dir, "_files");

	if (stat(dir, &st) == -1) {
    		mkdir(dir, 0700);
	}

	if(chdir(dir) != 0){
		perror("err moving to folder\n");
		return -1;
	}

	return 1;
}

void list_commands(){
	printf("\n[Download file]: download <filename>\n[Close client]: quit\n\n");
}


int main(int argc, char *argv[]){
	struct sockaddr_in addr, serv_addr;
	int buff_size = 100;
	int sock = 0, port, size_recv = 0, size_send = 0, finished = 0;
	int filename_len = 30;
	char *buff = (char*) malloc(sizeof(char) * buff_size);
	char *filename = (char*) malloc(sizeof(char) * filename_len);
	FILE *f;
	char *command = (char*) malloc(10 * sizeof(char));
	char *tok;
	int open_file = 0;

	if(argc != 2){	
		printf("Not enough args. Usage: %s <port>\n", argv[0]);
		return -1;
	}
	
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket creation err\n");
		return -1;
	}

	port = atoi(argv[1]);
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family =AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(PF_INET, "localhost", &serv_addr.sin_addr);
	
	memset(buff, '0', buff_size);
	
	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("conn failed\n");
		return -1;
	}
	printf("Client connected to server\n");

	// move to client folder
	if(move_to_clnt_folder() == -1){
		perror("couldn't move to client's folder\n");
		return -1;	
	}
	
	list_commands();
	
	/* In functie de comanda introdusa va face diverse actiuni */
	while(1){
		scanf("%s", command);
		fflush(stdin);

		/* download file */
		if(strcmp(command, "download") == 0){
			scanf("%s", filename);
			fflush(stdin);
			filename_len = strlen(filename);
			printf("%s\n", filename);
			if((send(sock, filename, filename_len, 0)) != filename_len){
				perror("err send filename to server\n");
				return -1;
			}

				
			open_file = 0;
			memset(buff, 0, buff_size);
			while((size_recv = recv(sock, buff, buff_size, 0))){
				if(size_recv < 0){
					perror("err\n");
					return -1;
				}
				
				/* Daca primeste comanda de finish, a terminat 
				   receptia si iese din while*/
				if(strstr(buff, "finish") != NULL){
					break;
				} else {
					if(strstr(buff, "File not found") != NULL){
						printf("File not found\n");
						break;
					}else{
					
						if(open_file == 0){

							if((f = fopen(filename, "w+a")) == NULL){
								printf("File could not be opened\n");
								exit(1);
							}
							open_file = 1;
						}
						fprintf(f, "%s", buff, 1);
						memset(buff, 0, buff_size);
					}
				}
				
			}
			if(open_file == 1){
				fclose(f);
			}

		/* close client */
		} else if(strcmp(command, "quit") == 0){
			strcpy(buff, "quit"); 
			if((size_send = send(sock, buff, strlen(buff), 0)) < 0){
				perror("err send\n");
			} 

			printf("End of connection, client is closing...\n");
			break ;
		} else {
			printf("Wrong command!\n");
			fflush(stdin);
		}

		list_commands();
	}
	
	close(sock);
	free(command);
	free(filename);
	free(buff);
	return 0;
}
