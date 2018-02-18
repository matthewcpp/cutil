#pragma once

#include <stdbool.h>

typedef struct _cutil_list_node {
	void *data;
	struct _cutil_list_node* next;
	struct _cutil_list_node* prev;
} _cutil_list_node;

typedef struct cutil_list {
	unsigned int _size;
	_cutil_list_node _base;
	unsigned int _item_size;

#ifdef CUTIL_DEBUGGING
	bool _debug_malloc;
	bool _debug_ptr;
	unsigned int _debug_generation;
#endif
} cutil_list;

cutil_list *cutil_list_create(unsigned int item_size);

void cutil_list_init(cutil_list *list, unsigned int item_size);

unsigned int cutil_list_size(cutil_list* list);
void cutil_list_clear(cutil_list* list);
void cutil_list_uninit(cutil_list* list);
void cutil_list_destroy(cutil_list* list);

void cutil_list_pop_front(cutil_list* list);
void cutil_list_pop_back(cutil_list* list);

bool cutil_list_get_front(cutil_list* list, void *data);
bool cutil_list_get_back(cutil_list* list, void *data);

void cutil_list_push_front(cutil_list* list, void *data);
void cutil_list_push_back(cutil_list* list, void *data);


typedef struct cutil_list_itr {
	cutil_list *_list;
	_cutil_list_node* _node;

#ifdef CUTIL_DEBUGGING
	bool _debug_malloc;
	unsigned int _debug_generation;
#endif
} cutil_list_itr;

void cutil_list_itr_init(cutil_list_itr *itr, cutil_list *list);
cutil_list_itr *cutil_list_itr_create(cutil_list *list);

void cutil_list_itr_uninit(cutil_list_itr *itr);
void cutil_list_itr_destroy(cutil_list_itr *itr);

bool cutil_list_itr_has_next(cutil_list_itr *itr);
bool cutil_list_itr_next(cutil_list_itr *itr, void* data);

bool cutil_list_itr_has_prev(cutil_list_itr *itr);
bool cutil_list_itr_prev(cutil_list_itr *itr, void* data);
