#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define NCLNTS 3
#define SERVER_QUEUE_NAME "/server_queue"
#define CLIENT_QUEUE_NAME "/client_queue_"
#define QNAME_SIZE 20
#define QUEUE_PERMISSIONS 0666
#define MSG_SIZE 70

char *gen_clnt_qname();

void my_client();
