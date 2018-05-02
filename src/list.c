#include "list.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void _cutil_list_node_destroy(_cutil_list_node* list_node);
_cutil_list_node* _cutil_list_node_create(unsigned int item_size);

cutil_list* cutil_list_create(unsigned int item_size) {
	cutil_list* list = malloc(sizeof(cutil_list));
	cutil_list_init(list, item_size);

#ifdef CUTIL_DEBUGGING
	list->_debug_malloc = true;
#endif

	return list;
}

void cutil_list_init(cutil_list*  list, unsigned int item_size) {
	list->_size = 0;
	list->_base.prev = &list->_base;
	list->_base.next = &list->_base;
	list->_base.data = NULL;
	list->_item_size = item_size;

#ifdef CUTIL_DEBUGGING
	list->_debug_generation = 0;
	list->_debug_malloc = false;
#endif
}

void cutil_list_destroy(cutil_list* list) {
#ifdef CUTIL_DEBUGGING
	assert(list->_debug_malloc);
#endif

	cutil_list_uninit(list);
	free(list);
}

void cutil_list_uninit(cutil_list* list) {
	cutil_list_clear(list);
}

void cutil_list_clear(cutil_list* list) {
	if (list->_size > 0) {
		_cutil_list_node* node_to_delete = list->_base.next;

		while (node_to_delete != &list->_base) {
			_cutil_list_node* next_node = node_to_delete->next;
            _cutil_list_node_destroy(node_to_delete);
			node_to_delete = next_node;
		}

		list->_size = 0;
		list->_base.next = &list->_base;
		list->_base.prev = &list->_base;

#ifdef CUTIL_DEBUGGING
		list->_debug_generation += 1;
#endif
	}
}

unsigned int cutil_list_size(cutil_list* list) {
	return list->_size;
}

_cutil_list_node* _cutil_list_node_create(unsigned int item_size) {
	_cutil_list_node* new_node = (_cutil_list_node*)malloc(sizeof(_cutil_list_node));
	new_node->data = malloc(item_size);
	new_node->prev = NULL;
	new_node->next = NULL;

	return new_node;
}

void _cutil_list_node_destroy(_cutil_list_node* list_node){
    free(list_node->data);
    free(list_node);
}

void _cutil_list_push_add_first(cutil_list* list, _cutil_list_node* new_node) {
	new_node->prev = &list->_base;
	new_node->next = &list->_base;

	list->_base.prev = new_node;
	list->_base.next = new_node;
}

void cutil_list_push_front(cutil_list* list, void* data) {
	_cutil_list_node* new_node = _cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	_cutil_list_node* current_front_node = list->_base.next;

	// at least one item already in list
	if (list->_size > 0) {
		new_node->next = current_front_node;
		new_node->prev = &list->_base;

		list->_base.next = new_node;
		current_front_node->prev = new_node;
	}
	else { //first item added to the list
		_cutil_list_push_add_first(list, new_node);
	}

	list->_size += 1;
#ifdef CUTIL_DEBUGGING
	list->_debug_generation += 1;
#endif
}

bool cutil_list_get_front(cutil_list* list, void* data) {
	if (list->_size > 0) {
		memcpy(data, list->_base.next->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

bool cutil_list_get_back(cutil_list* list, void* data) {
	if (list->_size > 0) {
		memcpy(data, list->_base.prev->data, list->_item_size);
		return true;
	}
	else {
		return false;
	}
}

void cutil_list_push_back(cutil_list* list, void* data) {
	_cutil_list_node* new_node = _cutil_list_node_create(list->_item_size);
	memcpy(new_node->data, data, list->_item_size);

	if (list->_size > 0) {
		_cutil_list_node* current_back_node = list->_base.prev;
		new_node->next = &list->_base;
		new_node->prev = current_back_node;

		current_back_node->next = new_node;
		list->_base.prev = new_node;
	}
	else {
		_cutil_list_push_add_first(list, new_node);
	}

	list->_size += 1;
#ifdef CUTIL_DEBUGGING
	list->_debug_generation += 1;
#endif
}

void cutil_list_pop_front(cutil_list* list) {
	if (list->_size > 0) {
		_cutil_list_node* node_to_delete = list->_base.next;
		_cutil_list_node* new_front = node_to_delete->next;

		// additional node in the list, make it the new "first" node
		if (new_front != &list->_base) {
			list->_base.next = new_front;
			new_front->prev = &list->_base;
		}
		else { //no items left in the list
			list->_base.next = &list->_base;
			list->_base.prev = &list->_base;
		}

        _cutil_list_node_destroy(node_to_delete);
		list->_size -= 1;

#ifdef CUTIL_DEBUGGING
		list->_debug_generation += 1;
#endif
	}
}

void cutil_list_pop_back(cutil_list* list) {
	if (list->_size > 0) {
		_cutil_list_node* node_to_delete = list->_base.prev;
		_cutil_list_node* new_back = node_to_delete->prev;

		//addtional node in the list, bake it the new "back" node
		if (new_back != &list->_base) {
			new_back->next = &list->_base;
			list->_base.prev = new_back;
		}
		else {//no items left in the list
			list->_base.next = &list->_base;
			list->_base.prev = &list->_base;
		}

		_cutil_list_node_destroy(node_to_delete);
		list->_size -= 1;

#ifdef CUTIL_DEBUGGING
		list->_debug_generation += 1;
#endif
	}
}


void cutil_list_itr_init(cutil_list_itr* itr, cutil_list* list) {
	itr->_list = list;
	itr->_node = &list->_base;

#ifdef CUTIL_DEBUGGING
	itr->_debug_generation = list->_debug_generation;
	itr->_debug_malloc = false;
#endif
}

cutil_list_itr* cutil_list_itr_create(cutil_list* list) {
	cutil_list_itr* itr = malloc(sizeof(cutil_list_itr));
	cutil_list_itr_init(itr, list);

#ifdef CUTIL_DEBUGGING
	itr->_debug_malloc = true;
#endif

	return itr;
}

void cutil_list_itr_uninit(cutil_list_itr* itr) {
	(void)itr;
}

void cutil_list_itr_destroy(cutil_list_itr* itr) {
#ifdef CUTIL_DEBUGGING
    assert(itr->_debug_malloc);
#endif

	cutil_list_itr_uninit(itr);
    free(itr);
}

bool cutil_list_itr_has_next(cutil_list_itr* itr) {
#ifdef CUTIL_DEBUGGING
	assert(itr->_debug_generation == itr->_list->_debug_generation);
#endif

	return (itr->_node->next->data != NULL);
}

bool cutil_list_itr_next(cutil_list_itr* itr, void* data) {
	if (cutil_list_itr_has_next(itr)) {
		itr->_node = itr->_node->next;

		if (data) {
			memcpy(data, itr->_node->data, itr->_list->_item_size);
		}

		return true;
	}

	return false;
}

bool cutil_list_itr_has_prev(cutil_list_itr* itr) {
#ifdef CUTIL_DEBUGGING
	assert(itr->_debug_generation == itr->_list->_debug_generation);
#endif

	return (itr->_node->prev->data != NULL);
}

bool cutil_list_itr_prev(cutil_list_itr* itr, void* data) {
	if (cutil_list_itr_has_prev(itr)) {
		itr->_node = itr->_node->prev;

		if (data) {
			memcpy(data, itr->_node->data, itr->_list->_item_size);
		}

		return true;
	}

	return false;
}
