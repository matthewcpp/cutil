#ifndef CUTIL_LIST_H
#define CUTIL_LIST_H

#include "trait.h"

#include <stdbool.h>
#include <stddef.h>


typedef struct cutil_list cutil_list;

cutil_list* cutil_list_create(cutil_trait* trait);
void cutil_list_destroy(cutil_list* list);

unsigned int cutil_list_size(cutil_list* list);
void cutil_list_clear(cutil_list* list);

bool cutil_list_pop_front(cutil_list* list);
bool cutil_list_pop_back(cutil_list* list);

bool cutil_list_front(cutil_list* list, void* data);
bool cutil_list_back(cutil_list* list, void* data);
bool cutil_list_at(cutil_list* list, size_t index, void* data);

void cutil_list_push_front(cutil_list* list, void* data);
void cutil_list_push_back(cutil_list* list, void* data);


typedef struct cutil_list_itr cutil_list_itr;

cutil_list_itr* cutil_list_itr_create(cutil_list* list);
void cutil_list_itr_destroy(cutil_list_itr* itr);

bool cutil_list_itr_has_next(cutil_list_itr* itr);
bool cutil_list_itr_next(cutil_list_itr* itr, void* data);

bool cutil_list_itr_has_prev(cutil_list_itr* itr);
bool cutil_list_itr_prev(cutil_list_itr* itr, void* data);

#endif
