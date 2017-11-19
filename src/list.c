#include "list.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

cutil_list *cutil_list_create(unsigned int item_size) {
	cutil_list *list = malloc(sizeof(cutil_list));
	cutil_list_init(list, item_size);

#ifdef _DEBUG
	list->_debug_malloc = true;
	list->_debug_ptr = false;
#endif

	return list;
}

cutil_list *cutil_list_createp() {
#ifdef _DEBUG
	cutil_list * list = cutil_list_create(sizeof(uintptr_t));
	list->_debug_ptr = true;

	return list;
#else
	return cutil_list_create(sizeof(uintptr_t));
#endif
	
}

void cutil_list_init(cutil_list * list, unsigned int item_size) {
	list->_size = 0;
	list->_front = NULL;
	list->_back = NULL;
	list->_item_size = item_size;

#ifdef _DEBUG
	list->_debug_malloc = false;
	list->_debug_ptr = false;
#endif
}

void cutil_list_initp(cutil_list * list) {
	cutil_list_init(list, sizeof(uintptr_t));

#ifdef _DEBUG
	list->_debug_ptr = true;
#endif
}

void cutil_list_destroy(cutil_list* list) {
#ifdef _DEBUG
	assert(list->_debug_malloc);
#endif

	cutil_list_clear(list);
	free(list);
}

void cutil_list_uninit(cutil_list* list) {
#ifdef _DEBUG
	assert(!list->_debug_malloc);
#endif

	cutil_list_clear(list);
}

void cutil_list_clear(cutil_list* list) {
	_cutil_list_node* node_to_delete = list->_front;
		
	while (node_to_delete) {
		_cutil_list_node *next_node = node_to_delete->next;
		free(node_to_delete);
		node_to_delete = next_node;
	}

	list->_size = 0;
	list->_front = NULL;
	list->_back = NULL;
}

unsigned int cutil_list_size(cutil_list* list) {
	return list->_size;
}

_cutil_list_node* cutil_list_node_create(unsigned int item_size) {
	_cutil_list_node* new_node = (_cutil_list_node*)malloc(sizeof(_cutil_list_node));
	new_node->data = malloc(item_size);
	new_node->prev = NULL;
	new_node->next = NULL;

	return new_node;
}

void cutil_list_push_front(cutil_list* list, void *data) {
	_cutil_list_node* new_node = cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	if (list->_front) {
		new_node->next = list->_front;
		list->_front->prev = new_node;
	}

	list->_front = new_node;

	if (list->_back == NULL) {
		list->_back = list->_front;
	}

	list->_size += 1;
}

void cutil_list_push_frontp(cutil_list* list, void *data) {
#ifdef _DEBUG
	assert(list->_debug_ptr);
#endif

	uintptr_t int_ptr = (uintptr_t)data;
	cutil_list_push_front(list, &int_ptr);
}

bool cutil_list_get_front(cutil_list* list, void *data) {
	if (list->_front) {
		memcpy(data, list->_front->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_frontp(cutil_list* list, void **data) {
#ifdef _DEBUG
	assert(list->_debug_ptr);
#endif

	uintptr_t ptr;

	if (cutil_list_get_front(list, &ptr)) {
		*data = (void*)ptr;
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_back(cutil_list* list, void *data) {
	if (list->_back) {
		memcpy(data, list->_back->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_backp(cutil_list* list, void **data) {
#ifdef _DEBUG
	assert(list->_debug_ptr);
#endif

	uintptr_t ptr;

	if (cutil_list_get_back(list, &ptr)) {
		*data = (void*)ptr;
		return true;
	}
	else {
		return false;
	}
}

void cutil_list_push_back(cutil_list* list, void *data) {
	_cutil_list_node* new_node = cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	if (list->_back) {
		list->_back->next = new_node;
		new_node->prev = list->_back;
	}

	list->_back = new_node;

	if (list->_front == NULL) {
		list->_front = list->_back;
	}

	list->_size += 1;
}

void cutil_list_push_backp(cutil_list* list, void *data) {
#ifdef _DEBUG
	assert(list->_debug_ptr);
#endif

	uintptr_t int_ptr = (uintptr_t)data;
	cutil_list_push_back(list, &int_ptr);
}

void cutil_list_pop_front(cutil_list* list) {
	if (list->_front) {
		_cutil_list_node* node_to_delete = list->_front;
		list->_front = list->_front->next;

		if (list->_front) {
			list->_front->prev = NULL;
		}

		if (list->_back == node_to_delete) {
			list->_back = NULL;
		}

		free(node_to_delete);
		list->_size -= 1;
	}
}

void cutil_list_pop_back(cutil_list* list) {
	if (list->_back) {
		_cutil_list_node* node_to_delete = list->_back;
		list->_back = list->_back->prev;

		if (list->_back) {
			list->_back->next = NULL;
		}

		if (list->_front == node_to_delete) {
			list->_front = NULL;
		}

		free(node_to_delete);
		list->_size -= 1;
	}
}
