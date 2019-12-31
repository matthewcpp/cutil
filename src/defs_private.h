#ifndef  CUTIL_DEFS_PRIVATE_H
#define CUTIL_DEFS_PRIVATE_H

#ifdef _WIN32
    #include <malloc.h>
    #define alloca_func _alloca
#else
    #include <alloca.h>
    #define alloca_func alloca
#endif

#endif
