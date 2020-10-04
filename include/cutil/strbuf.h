#ifndef CUTIL_STRBUF_H
#define CUTIL_STRBUF_H

#include <stddef.h>

typedef struct cutil_strbuf cutil_strbuf;

cutil_strbuf* cutil_strbuf_create();
cutil_strbuf* cutil_strbuf_create_with_cstring(const char* str);
cutil_strbuf* cutil_strbuf_create_with_str(const char* str, size_t size);
void cutil_strbuf_destroy(cutil_strbuf* strbuf);

void cutil_strbuf_append_char(cutil_strbuf* strbuf, char ch);
void cutil_strbuf_append_cstring(cutil_strbuf* strbuf, const char* str);
void cutil_strbuf_append_str(cutil_strbuf* strbuf, const char* str, size_t size);
const char* cutil_strbuf_cstring(cutil_strbuf* strbuf);

/**
Removes the last character from the strbuf.
@returns non zero value if a character was popped off or zero if the buffer was empty.
*/
int cutil_strbuf_pop_back(cutil_strbuf* strbuf);

/**
Sets a character at a given index to a new value
@param index the index in the buffer that will be set.
@param ch the character to set at the supplied index
@returns non zero value if the index was valid, or zero if the index was invalid
*/
int cutil_strbuf_set_char(cutil_strbuf* strbuf, size_t index, char ch);

void cutil_strbuf_clear(cutil_strbuf* strbuf);
size_t cutil_strbuf_length(cutil_strbuf* strbuf);


#endif
