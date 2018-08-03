#include <stdio.h>
#include <stdlib.h>


typedef struct node{
	int val;
	void (*callback_print) (struct node*);
	struct node* next;
}List;

void print_node(List *node);

List *aloc_node(int val);

void add_node(List **list, int new_val);

void delete_node(List **list, int val);

void print_list(List *list);

void swap(List *a, List *b);

void sort_list(List *list);

void flush_list(List **list);



