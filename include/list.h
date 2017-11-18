#pragma once

#include <stdbool.h>

typedef struct cutil_list_node {
	void *data;
	struct cutil_list_node* next;
	struct cutil_list_node* prev;
} cutil_list_node;

typedef struct cutil_list {
	unsigned int size;
	cutil_list_node *front;
	cutil_list_node *back;
	unsigned int _item_size;
} cutil_list;

void cutil_list_init(cutil_list * list, unsigned int item_size);
void cutil_list_initp(cutil_list * list);

void cutil_list_destroy(cutil_list* list);
void cutil_list_clear(cutil_list* list);

void cutil_list_pop_front(cutil_list* list);
bool cutil_list_get_front(cutil_list* list, void *data);
bool cutil_list_get_frontp(cutil_list* list, void **data);

void cutil_list_pop_back(cutil_list* list);
bool cutil_list_get_back(cutil_list* list, void *data);
bool cutil_list_get_backp(cutil_list* list, void **data);

void cutil_list_push_front(cutil_list* list, void *data);
void cutil_list_push_frontp(cutil_list* list, void *data);

void cutil_list_push_back(cutil_list* list, void *data);
void cutil_list_push_backp(cutil_list* list, void *data);
