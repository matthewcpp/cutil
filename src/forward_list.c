#include "cutil/forward_list.h"
#include "cutil/allocator.h"

#include <stddef.h>
#include <string.h>

typedef struct cutil_forward_list_node {
    void* data;
    struct cutil_forward_list_node* next;
} cutil_forward_list_node;

struct cutil_forward_list {
    cutil_forward_list_node before_begin;
    cutil_trait* trait;
};

struct cutil_forward_list_itr {
    cutil_forward_list* list;
    cutil_forward_list_node* node;
};

void cutil_forward_list_node_destroy(cutil_forward_list* list, cutil_forward_list_node* list_node);
cutil_forward_list_node* cutil_forward_list_node_create(cutil_forward_list* list, void* data);

cutil_forward_list* cutil_forward_list_create(cutil_trait* trait) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_forward_list* list = allocator->malloc(sizeof(cutil_forward_list), allocator->user_data);
    
    list->before_begin.next = &list->before_begin;
    list->before_begin.data = NULL;
    list->trait = trait;

    return list;
}

void cutil_forward_list_destroy(cutil_forward_list* list) {
    cutil_allocator* allocator = cutil_current_allocator();

    cutil_forward_list_clear(list);
    allocator->free(list, allocator->user_data);
}

void cutil_forward_list_clear(cutil_forward_list* list) {
    if (list->before_begin.next != &list->before_begin) {
        cutil_forward_list_node* node_to_delete = list->before_begin.next;

        while (node_to_delete != &list->before_begin) {
            cutil_forward_list_node* next_node = node_to_delete->next;
            cutil_forward_list_node_destroy(list, node_to_delete);
            node_to_delete = next_node;
        }

        list->before_begin.next = &list->before_begin;
    }
}

int cutil_forward_list_empty(cutil_forward_list* list) {
    return list->before_begin.next == &list->before_begin;
}

cutil_forward_list_node* cutil_forward_list_node_create(cutil_forward_list* list, void* data) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_forward_list_node* new_node = allocator->malloc(sizeof(cutil_forward_list_node), allocator->user_data);
    new_node->data = allocator->malloc(list->trait->size, allocator->user_data);

    if (list->trait->copy_func) {
        list->trait->copy_func(new_node->data, data, list->trait->user_data);
    }
    else {
        memcpy(new_node->data, data, list->trait->size);
    }

    new_node->next = NULL;

    return new_node;
}

void cutil_forward_list_node_destroy(cutil_forward_list* list, cutil_forward_list_node* list_node){
    cutil_allocator* allocator = cutil_current_allocator();

    if (list->trait->destroy_func) {
        list->trait->destroy_func(list_node->data, list->trait->user_data);
    }

    allocator->free(list_node->data, allocator->user_data);
    allocator->free(list_node, allocator->user_data);
}

void cutil_forward_list_push_front(cutil_forward_list* list, void* data) {
    cutil_forward_list_node* new_node = cutil_forward_list_node_create(list, data);

    /* at least one item already in list */
    if (cutil_forward_list_empty(list)) {
        new_node->next = &list->before_begin;
        list->before_begin.next = new_node;
    }
    else { /* first item added to the list */
        cutil_forward_list_node* current_front_node = list->before_begin.next;

        new_node->next = current_front_node;
        list->before_begin.next = new_node;
    }
}

int cutil_forward_list_front(cutil_forward_list* list, void* out) {
    if (cutil_forward_list_empty(list)) {
        return 0;
    }
    else {
        memcpy(out, list->before_begin.next->data, list->trait->size);
        return 1;
    }
}

int cutil_forward_list_pop_front(cutil_forward_list* list) {
    if (list->before_begin.next != &list->before_begin) {
        cutil_forward_list_node* node_to_delete = list->before_begin.next;
        cutil_forward_list_node* new_front = node_to_delete->next;

        /* additional node in the list, make it the new "first" node */
        if (new_front != &list->before_begin) {
            list->before_begin.next = new_front;
        }
        else { /* no items left in the list */
            list->before_begin.next = &list->before_begin;
        }

        cutil_forward_list_node_destroy(list, node_to_delete);

        return 1;
    }
    else {
        return 0;
    }
}

cutil_forward_list_itr* cutil_forward_list_itr_create(cutil_forward_list* list) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_forward_list_itr* itr = allocator->malloc(sizeof(cutil_forward_list_itr), allocator->user_data);

    itr->list = list;
    itr->node = &list->before_begin;

    return itr;
}

void cutil_forward_list_itr_destroy(cutil_forward_list_itr* itr) {
    cutil_allocator* allocator = cutil_current_allocator();
    allocator->free(itr, allocator->user_data);
}

int cutil_forward_list_itr_has_next(cutil_forward_list_itr* itr) {
    return (itr->node->next->data != NULL);
}

int cutil_forward_list_itr_next(cutil_forward_list_itr* itr, void* out) {
    if (cutil_forward_list_itr_has_next(itr)) {
        itr->node = itr->node->next;

        if (out) {
            memcpy(out, itr->node->data, itr->list->trait->size);
        }

        return 1;
    }

    return 0;
}
