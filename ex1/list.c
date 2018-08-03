#include <stdio.h>
#include <stdlib.h>

#include "list.h"

void print_node(List *node){
	if(node == NULL){
		return ;
	}
	printf("%d ",node->val);
}

List *aloc_node(int val){
	List *new_node = (List*) malloc(sizeof(List));
	
	if(new_node == NULL){
		return NULL;
	}
	
	new_node->val = val;
	new_node->callback_print = &print_node;
	new_node->next = NULL;
	
	return new_node;
}

void add_node(List **list, int new_val){
	List *new_node = aloc_node(new_val);
	new_node->callback_print;
	
	if(*list == NULL){
		*list = new_node;
		return ;
	}

	List *tmp = *list;
	while(tmp != NULL && tmp->next != NULL){
		tmp = tmp->next;
	}
	
	tmp->next = new_node;
}

void delete_node(List **list, int val){
	List *tmp, *prev = NULL;
	for(tmp = *list; tmp != NULL; tmp = tmp ->next){
		if(tmp->val == val){
			// trebuie sters primul element din lista
			if(prev == NULL){
				*list = (*list)->next;
			}else{
				prev->next = tmp->next;
			}
		}
		prev = tmp;
	}
}


void print_list(List *list){
	if(list == NULL){
		printf("lista este vida\n");
	}
	List *tmp;
	
	for(tmp = list; tmp != NULL; tmp = tmp->next){
		tmp->callback_print(tmp);

	}
	//printf("\n");
}

void swap(List *a, List *b){
	int tmp = a->val;
	a->val = b->val;
	b->val = tmp;
}

void sort_list(List *list){
	List *last = NULL, *tmp = list; 
	
	if(list == NULL){
		printf("Lista nu poate fi sortata, lista este vida\n");
		return ;
	}
	
	while(last != list){
		tmp = list;
		for(tmp = list; tmp->next != last; tmp = tmp->next){
			if(tmp->val > tmp->next->val){
				swap(tmp, tmp->next);
			}
		}
		
		last = tmp;
	}
}

void flush_list(List **list){
	List *tmp = *list, *crt;
	while(tmp != NULL){
		crt = tmp;
		tmp = tmp->next;
		free(crt);
	}

	free(*list);
	*list = NULL;
}




