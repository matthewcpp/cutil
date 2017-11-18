#include "list.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void cutil_list_init(cutil_list * list, unsigned int item_size) {
	list->size = 0;
	list->front = NULL;
	list->back = NULL;
	list->_item_size = item_size;
}

void cutil_list_initp(cutil_list * list) {
	cutil_list_init(list, sizeof(uintptr_t));
}

void cutil_list_destroy(cutil_list* list) {
	cutil_list_clear(list);
}

void cutil_list_clear(cutil_list* list) {
	cutil_list_node* node_to_delete = list->front;
		
	while (node_to_delete) {
		cutil_list_node *next_node = node_to_delete->next;
		free(node_to_delete);
		node_to_delete = next_node;
	}

	list->size = 0;
	list->front = NULL;
	list->back = NULL;
}

cutil_list_node* cutil_list_node_create(unsigned int item_size) {
	cutil_list_node* new_node = (cutil_list_node*)malloc(sizeof(cutil_list_node));
	new_node->data = malloc(item_size);
	new_node->prev = NULL;
	new_node->next = NULL;

	return new_node;
}

void cutil_list_push_front(cutil_list* list, void *data) {
	cutil_list_node* new_node = cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	if (list->front) {
		new_node->next = list->front;
		list->front->prev = new_node;
	}

	list->front = new_node;

	if (list->back == NULL) {
		list->back = list->front;
	}

	list->size += 1;
}

void cutil_list_push_frontp(cutil_list* list, void *data) {
	uintptr_t int_ptr = (uintptr_t)data;
	cutil_list_push_front(list, &int_ptr);
}

bool cutil_list_get_front(cutil_list* list, void *data) {
	if (list->front) {
		memcpy(data, list->front->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_frontp(cutil_list* list, void **data) {
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
	if (list->back) {
		memcpy(data, list->back->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_backp(cutil_list* list, void **data) {
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
	cutil_list_node* new_node = cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	if (list->back) {
		list->back->next = new_node;
		new_node->prev = list->back;
	}

	list->back = new_node;

	if (list->front == NULL) {
		list->front = list->back;
	}

	list->size += 1;
}

void cutil_list_push_backp(cutil_list* list, void *data) {
	uintptr_t int_ptr = (uintptr_t)data;
	cutil_list_push_back(list, &int_ptr);
}

void cutil_list_pop_front(cutil_list* list) {
	if (list->front) {
		cutil_list_node* node_to_delete = list->front;
		list->front = list->front->next;

		if (list->front) {
			list->front->prev = NULL;
		}

		if (list->back == node_to_delete) {
			list->back = NULL;
		}

		free(node_to_delete);
		list->size -= 1;
	}
}

void cutil_list_pop_back(cutil_list* list) {
	if (list->back) {
		cutil_list_node* node_to_delete = list->back;
		list->back = list->back->prev;

		if (list->back) {
			list->back->next = NULL;
		}

		if (list->front == node_to_delete) {
			list->front = NULL;
		}

		free(node_to_delete);
		list->size -= 1;
	}
}
