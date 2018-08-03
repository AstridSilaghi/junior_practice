#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#include "list.h"

#define N 3

typedef struct operation{
	char operation;
	int value;
}Operations;

typedef struct thread{
	pthread_t thread;
	Operations *operations;
	int crt_op, total_op;
}ThreadData;

pthread_cond_t cond;
pthread_mutex_t cond_mutex;
pthread_mutex_t list_mutex;
sem_t semaphor;
List *list = NULL;
ThreadData td[N];

/*
	Operatii posibile
	a = add
	p = print
	s = sort
	d = delete
	f = flush
*/
void exec_op(int id){
	
	printf(" %d  ", id);

	char op = td[id].operations[td[id].crt_op].operation;
	int value = td[id].operations[td[id].crt_op].value;	
	
	if(op == 'a')
		add_node(&list, value);		
	if(op == 'p')
		print_list(list);
	if(op == 'f')
		flush_list(&list);
	if(op == 'd')
		delete_node(&list, value);	
	if(op == 's')
		sort_list(list);

	td[id].crt_op += 1;
	
	if(op == 'd' || op == 'a'){
		printf("and the exec operation is : %c with value = %d\n", op, value);	
	}else{
		printf("and the exec operation is : %c \n", op);	
	}
}

/* Se initializeaza vectorul de operatii pentru fiecare thread */
void init_operations(){
	Operations o1 = {'a', 2};
	Operations o2 = {'a', 4};
	Operations o3 = {'a', 10};
	Operations o4 = {'d', 2};
	Operations o5 = {'s'};
	Operations o6 = {'d', 10};
	Operations o7 = {'d', 5};
	Operations o8 = {'a', 11};
	Operations o9 = {'a', 1};
	Operations o10 = {'d', 11};
	Operations o11 = {'a', 8};
	Operations o12 = {'p'};
	Operations o13 = {'a', 30};
	Operations o14 = {'a', 25};
	Operations o15 = {'a', 100};
	Operations o16 = {'s'};
	Operations o17 = {'p'};
	Operations o18 = {'d', 100};
	
	td[0].crt_op = 0;
	td[0].total_op = 7;
	td[0].operations = (Operations*) malloc(td[0].total_op * sizeof(Operations));
	
	td[0].operations[0] = o1;;
	td[0].operations[1] = o2;
	td[0].operations[2] = o3;
	td[0].operations[3] = o4;
	td[0].operations[4] = o5;
	td[0].operations[5] = o6;
	td[0].operations[6] = o7;

	td[1].crt_op = 0;
	td[1].total_op = 5;
	td[1].operations = (Operations*) malloc(td[1].total_op * sizeof(Operations));
	 
	td[1].operations[0] = o8;;
	td[1].operations[1] = o9;
	td[1].operations[2] = o10;
	td[1].operations[3] = o11;
	td[1].operations[4] = o12;
	
	td[2].crt_op = 0;
	td[2].total_op = 6;
	td[2].operations = (Operations*) malloc(td[2].total_op * sizeof(Operations));
	
	td[2].operations[0] = o13;;
	td[2].operations[1] = o14;
	td[2].operations[2] = o15;
	td[2].operations[3] = o16;
	td[2].operations[4] = o17;
	td[2].operations[5] = o18;
}

void init(){
	pthread_mutex_init(&cond_mutex, NULL);
	pthread_cond_init(&cond, NULL);
	sem_init(&semaphor, 0, N);
	pthread_mutex_init(&list_mutex, NULL);
}

void *routine(void *arg){
	int id = (int) arg;	
	
	sem_wait(&semaphor);

	pthread_mutex_lock(&cond_mutex);
	pthread_cond_wait(&cond, &cond_mutex);
	pthread_mutex_unlock(&cond_mutex);
	
	printf("Thread %d incepe executia\n\n", id);
	while(td[id].total_op - 1 != td[id].crt_op){
		pthread_mutex_lock(&list_mutex);	
		printf("Thread id is : %ld ", pthread_self());	
		exec_op(id);
		pthread_mutex_unlock(&list_mutex);
	}	
	pthread_exit(0);
}

int main(){
	int i, count_sem = 4;
	pthread_t threads[N];

	init();
	init_operations();	
	
	for(i = 0; i < N; i++){
		pthread_create(&threads[i], NULL, routine, (void*) i);	
	}

	while(count_sem != 0){
		sem_getvalue(&semaphor, &count_sem);
	}

	/* cand toate thread-urile au trecut de semafor se deblocheaza 
	   conditia de trecere */
	pthread_cond_broadcast(&cond);
	
	for(i = 0; i < N; i++){
		pthread_join(threads[i], NULL);		
	}
	
	printf("Lista finala este : \n");
	print_list(list);
	printf("\n");

	sem_destroy(&semaphor);
	pthread_mutex_destroy(&cond_mutex);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&list_mutex);

	return 0;
}


