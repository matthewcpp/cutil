#include "cutil/trait.h"

#include <stdlib.h>
#include <string.h>

int cutil_trait_int_compare(void* a, void* b, void* user_data) {
    int int_a = *(int*)a;
    int int_b = *(int*)b;

    (void)user_data;

    if (int_a > int_b) {
        return 1;
    }
    else if (int_a < int_b) {
        return -1;
    }
    else {
        return 0;
    }
}

int cutil_trait_uint_compare(void* a, void* b, void* user_data) {
    int uint_a = *(unsigned int*)a;
    int uint_b = *(unsigned int*)b;

    (void)user_data;

    if (uint_a > uint_b) {
        return 1;
    }
    else if (uint_a < uint_b) {
        return -1;
    }
    else {
        return 0;
    }
}

int cutil_trait_float_compare(void* a, void* b, void* user_data) {
    float float_a = *(float*)a;
    float float_b = *(float*)b;

    (void)user_data;

    if (float_a > float_b) {
        return 1;
    }
    else if (float_a < float_b) {
        return -1;
    }
    else {
        return 0;
    }
}

int cutil_trait_ptr_compare(void* a, void* b, void* user_data) {
    void* a_ptr = *(void**)a;
    void* b_ptr = *(void**)b;

    (void)user_data;

    if (a_ptr > b_ptr) {
        return 1;
    }
    else if (a_ptr < b_ptr) {
        return -1;
    }
    else {
        return 0;
    }
}

int cutil_trait_cstring_compare(void* a, void* b, void* user_data) {
    char* a_ptr = *((char**)a);
    char* b_ptr = *((char**)b);

    (void)user_data;

    return strcmp(a_ptr, b_ptr);
}

void cutil_trait_cstring_copy(void* dest, void* src, void* user_data) {
    char* src_ptr = *((char**)src);

    size_t buffer_size = strlen(src_ptr) + 1;
    char* str_copy = malloc(buffer_size);
    strncpy(str_copy, src_ptr, buffer_size);

    (void)user_data;

    memcpy(dest, &str_copy, sizeof(char*));
}

void cutil_trait_cstring_destroy(void* item, void* user_data) {
    char* item_ptr = *((char**)item);

    (void)user_data;

    free(item_ptr);
}

static cutil_trait* default_traits = NULL;

typedef enum {
    CUTIL_DEFAULT_TRAIT_INT = 0,
    CUTIL_DEFAULT_TRAIT_UINT,
    CUTIL_DEFAULT_TRAIT_FLOAT,
    CUTIL_DEFAULT_TRAIT_PTR,
    CUTIL_DEFAULT_TRAIT_CSTRING,
    CUTIL_DEFAULT_TRAIT_COUNT

} default_enum_indices;

void init_default_traits() {
    cutil_trait* trait = NULL;
    if (default_traits != NULL) return;

    default_traits = calloc(CUTIL_DEFAULT_TRAIT_COUNT, sizeof(cutil_trait));

    /* int trait */
    trait = default_traits + CUTIL_DEFAULT_TRAIT_INT;
    trait->compare_func = cutil_trait_int_compare;
    trait->size = sizeof(int);

    /* int trait */
    trait = default_traits + CUTIL_DEFAULT_TRAIT_UINT;
    trait->compare_func = cutil_trait_uint_compare;
    trait->size = sizeof(unsigned int);

    /* int trait */
    trait = default_traits + CUTIL_DEFAULT_TRAIT_FLOAT;
    trait->compare_func = cutil_trait_float_compare;
    trait->size = sizeof(float);

    /* ptr trait */
    trait = default_traits + CUTIL_DEFAULT_TRAIT_PTR;
    trait->compare_func = cutil_trait_ptr_compare;
    trait->size = sizeof(void*);

    /* cstring trait*/
    trait = default_traits + CUTIL_DEFAULT_TRAIT_CSTRING;
    trait->compare_func = cutil_trait_cstring_compare;
    trait->destroy_func = cutil_trait_cstring_destroy;
    trait->copy_func = cutil_trait_cstring_copy;
    trait->size = sizeof(char*);
}

cutil_trait* cutil_trait_int() {
    init_default_traits();

    return default_traits + CUTIL_DEFAULT_TRAIT_INT;
}

cutil_trait* cutil_trait_uint() {
    init_default_traits();

    return default_traits + CUTIL_DEFAULT_TRAIT_UINT;
}

cutil_trait* cutil_trait_float() {
    init_default_traits();

    return default_traits + CUTIL_DEFAULT_TRAIT_FLOAT;
}

cutil_trait* cutil_trait_ptr() {
    init_default_traits();

    return default_traits + CUTIL_DEFAULT_TRAIT_PTR;
}

cutil_trait* cutil_trait_cstring() {
    init_default_traits();

    return default_traits + CUTIL_DEFAULT_TRAIT_CSTRING;
}

void cutil_trait_destroy() {
    if (default_traits != NULL) {
        free(default_traits);
        default_traits = NULL;
    }
}
