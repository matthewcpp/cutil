#include "cutil/list.h"
#include "cutil/allocator.h"

#include <stddef.h>
#include <string.h>

typedef struct cutil_list_node {
    void* data;
    struct cutil_list_node* next;
    struct cutil_list_node* prev;
} cutil_list_node;

struct cutil_list {
    unsigned int size;
    cutil_list_node base;
    cutil_trait* trait;
};

struct cutil_list_itr {
    cutil_list* list;
    cutil_list_node* node;
};

void cutil_list_node_destroy(cutil_list* list, cutil_list_node* list_node);
cutil_list_node* cutil_list_node_create(cutil_list* list, void* data);

cutil_list* cutil_list_create(cutil_trait* trait) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_list* list = allocator->malloc(sizeof(cutil_list), allocator->user_data);

    list->size = 0;
    list->base.prev = &list->base;
    list->base.next = &list->base;
    list->base.data = NULL;
    list->trait = trait;

    return list;
}

void cutil_list_destroy(cutil_list* list) {
    cutil_allocator* allocator = cutil_current_allocator();

    cutil_list_clear(list);
    allocator->free(list, allocator->user_data);
}

void cutil_list_clear(cutil_list* list) {
    if (list->size > 0) {
        cutil_list_node* node_to_delete = list->base.next;

        while (node_to_delete != &list->base) {
            cutil_list_node* next_node = node_to_delete->next;
            cutil_list_node_destroy(list, node_to_delete);
            node_to_delete = next_node;
        }

        list->size = 0;
        list->base.next = &list->base;
        list->base.prev = &list->base;
    }
}

unsigned int cutil_list_size(cutil_list* list) {
    return list->size;
}

cutil_list_node* cutil_list_node_create(cutil_list* list, void* data) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_list_node* new_node = allocator->malloc(sizeof(cutil_list_node), allocator->user_data);
    new_node->data = allocator->malloc(list->trait->size, allocator->user_data);
    
    if (list->trait->copy_func) {
        list->trait->copy_func(new_node->data, data, list->trait->user_data);
    }
    else {
        memcpy(new_node->data, data, list->trait->size);
    }

    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

void cutil_list_node_destroy(cutil_list* list, cutil_list_node* list_node){
    cutil_allocator* allocator = cutil_current_allocator();

    if (list->trait->destroy_func) {
        list->trait->destroy_func(list_node->data, list->trait->user_data);
    }
    
    allocator->free(list_node->data, allocator->user_data);
    allocator->free(list_node, allocator->user_data);
}

void _cutil_list_push_add_first(cutil_list* list, cutil_list_node* new_node) {
    new_node->prev = &list->base;
    new_node->next = &list->base;

    list->base.prev = new_node;
    list->base.next = new_node;
}

void cutil_list_push_front(cutil_list* list, void* data) {
    cutil_list_node* new_node = cutil_list_node_create(list, data);

    cutil_list_node* current_front_node = list->base.next;

    /* at least one item already in list */
    if (list->size > 0) {
        new_node->next = current_front_node;
        new_node->prev = &list->base;

        list->base.next = new_node;
        current_front_node->prev = new_node;
    }
    else { /* first item added to the list */
        _cutil_list_push_add_first(list, new_node);
    }

    list->size += 1;
}

int cutil_list_front(cutil_list* list, void* out) {
    if (list->size > 0) {
        memcpy(out, list->base.next->data, list->trait->size);
        return 1;
    }
    else {
        return 0;
    }
}

int cutil_list_back(cutil_list* list, void* out) {
    if (list->size > 0) {
        memcpy(out, list->base.prev->data, list->trait->size);
        return 1;
    }
    else {
        return 0;
    }
}

void cutil_list_push_back(cutil_list* list, void* data) {
    cutil_list_node* new_node = cutil_list_node_create(list, data);

    if (list->size > 0) {
        cutil_list_node* current_back_node = list->base.prev;
        new_node->next = &list->base;
        new_node->prev = current_back_node;

        current_back_node->next = new_node;
        list->base.prev = new_node;
    }
    else {
        _cutil_list_push_add_first(list, new_node);
    }

    list->size += 1;
}

int cutil_list_pop_front(cutil_list* list) {
    if (list->size > 0) {
        cutil_list_node* node_to_delete = list->base.next;
        cutil_list_node* new_front = node_to_delete->next;

        /* additional node in the list, make it the new "first" node */
        if (new_front != &list->base) {
            list->base.next = new_front;
            new_front->prev = &list->base;
        }
        else { /* no items left in the list */
            list->base.next = &list->base;
            list->base.prev = &list->base;
        }

        cutil_list_node_destroy(list, node_to_delete);
        list->size -= 1;

        return 1;
    }
    else {
        return 0;
    }
}

int cutil_list_pop_back(cutil_list* list) {
    if (list->size > 0) {
        cutil_list_node* node_to_delete = list->base.prev;
        cutil_list_node* new_back = node_to_delete->prev;

        /* addtional node in the list, bake it the new "back" node */
        if (new_back != &list->base) {
            new_back->next = &list->base;
            list->base.prev = new_back;
        }
        else { /* no items left in the list */
            list->base.next = &list->base;
            list->base.prev = &list->base;
        }

        cutil_list_node_destroy(list, node_to_delete);
        list->size -= 1;

        return 1;
    }
    else {
        return 0;
    }
}

cutil_list_itr* cutil_list_itr_create(cutil_list* list) {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_list_itr* itr = allocator->malloc(sizeof(cutil_list_itr), allocator->user_data);
    
    itr->list = list;
    itr->node = &list->base;

    return itr;
}

void cutil_list_itr_destroy(cutil_list_itr* itr) {
    cutil_allocator* allocator = cutil_current_allocator();
    allocator->free(itr, allocator->user_data);
}

int cutil_list_itr_has_next(cutil_list_itr* itr) {
    return (itr->node->next->data != NULL);
}

int cutil_list_itr_next(cutil_list_itr* itr, void* out) {
    if (cutil_list_itr_has_next(itr)) {
        itr->node = itr->node->next;

        if (out) {
            memcpy(out, itr->node->data, itr->list->trait->size);
        }

        return 1;
    }

    return 0;
}

int cutil_list_itr_has_prev(cutil_list_itr* itr) {
    return (itr->node->prev->data != NULL);
}

int cutil_list_itr_prev(cutil_list_itr* itr, void* out) {
    if (cutil_list_itr_has_prev(itr)) {
        itr->node = itr->node->prev;

        if (out) {
            memcpy(out, itr->node->data, itr->list->trait->size);
        }

        return 1;
    }

    return 0;
}
