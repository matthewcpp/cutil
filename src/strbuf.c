#include "cutil/strbuf.h"
#include "cutil/allocator.h"

#include <string.h>

struct cutil_strbuf {
    char* buffer;
    size_t size;
    size_t capacity;
};

#define CUTIL_STRBUF_INTIAL_CAPACITY 16

cutil_strbuf* cutil_strbuf_create() {
    cutil_allocator* allocator = cutil_current_allocator();
    cutil_strbuf* strbuf = allocator->malloc(sizeof(cutil_strbuf), allocator->user_data);

    strbuf->buffer = 0;
    strbuf->size = 0;
    strbuf->capacity = 0;

    return strbuf;
}

cutil_strbuf* cutil_strbuf_create_with_cstring(const char* str) {
    cutil_strbuf* strbuf = cutil_strbuf_create();
    cutil_strbuf_append_cstring(strbuf, str);

    return strbuf;
}

cutil_strbuf* cutil_strbuf_create_with_str(const char* str, size_t size) {
    cutil_strbuf* strbuf = cutil_strbuf_create();
    cutil_strbuf_append_str(strbuf, str, size);

    return strbuf;
}

void cutil_strbuf_destroy(cutil_strbuf* strbuf) {
    cutil_allocator* allocator = cutil_current_allocator();

    if (strbuf->buffer)
        allocator->free(strbuf->buffer, allocator->user_data);

    allocator->free(strbuf, allocator->user_data);
}

void cutil_strbuf_create_buffer(cutil_strbuf* strbuf, size_t data_size) {
    cutil_allocator* allocator = cutil_current_allocator();

    if (data_size >= CUTIL_STRBUF_INTIAL_CAPACITY)
        strbuf->capacity = data_size + 1; /* include null terminator */
    else
        strbuf->capacity = CUTIL_STRBUF_INTIAL_CAPACITY;

    strbuf->size = 0;
    strbuf->buffer = allocator->malloc(strbuf->capacity, allocator->user_data);
    strbuf->buffer[0] = '\0';
}

void cutil_strbuf_ensure_buffer(cutil_strbuf* strbuf, size_t data_size) {
    if (strbuf->buffer == NULL) {
        cutil_strbuf_create_buffer(strbuf, data_size);
        return;
    }

    /* need to grow string buffer */
    if (strbuf->size + data_size >= strbuf->capacity) {
        cutil_allocator* allocator = cutil_current_allocator();
        strbuf->capacity *= 2;
        allocator->realloc(strbuf->buffer, strbuf->capacity, allocator->user_data);
    }
}

void cutil_strbuf_append_char(cutil_strbuf* strbuf, char ch) {
    cutil_strbuf_ensure_buffer(strbuf, 1);

    strbuf->buffer[strbuf->size++] = ch;
    strbuf->buffer[strbuf->size] = '\0';
}

void cutil_strbuf_append_cstring(cutil_strbuf* strbuf, const char* str){
    if (str != NULL) {
        cutil_strbuf_append_str(strbuf, str, strlen(str));
    }
}

void cutil_strbuf_append_str(cutil_strbuf* strbuf, const char* str, size_t size) {
    if (str != NULL) {
        cutil_strbuf_ensure_buffer(strbuf, size);
        memcpy(strbuf->buffer + strbuf->size, str, size);
        strbuf->size += size;
        strbuf->buffer[strbuf->size] = '\0';
    }
}

void cutil_strbuf_clear(cutil_strbuf* strbuf) {
    strbuf->size = 0;
    if (strbuf->buffer) {
        strbuf->buffer[0] = '\0';
    }
}

size_t cutil_strbuf_length(cutil_strbuf* strbuf) {
    return strbuf->size;
}

const char* cutil_strbuf_cstring(cutil_strbuf* strbuf) {
    if (strbuf->buffer)
        return strbuf->buffer;
    else
        return "";
}
