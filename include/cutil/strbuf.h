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

void cutil_strbuf_clear(cutil_strbuf* strbuf);
size_t cutil_strbuf_length(cutil_strbuf* strbuf);


#endif
