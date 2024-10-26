#ifndef __ALLOCATOR__
#define __ALLOCATOR__
#include <stdlib.h>

typedef struct {
    size_t allocated;
    size_t freed;
    size_t inuse;
} AllocStats;

typedef struct {
    void *memory;
    int line;
    size_t size;
    const char *function;
} Alloced;

#ifdef USE_CUSTOM_ALLOC
void *_c_alloc(size_t len, size_t size, int line, const char *function);
void *_c_realloc(void *memory, size_t len, size_t size, int line, const char *function);
void _c_free(void *memory, int line, const char *function);
void set_log_file();
void close_log_file();
const AllocStats *alloc_stats();

#define c_alloc(len, size) _c_alloc(len, size, __LINE__, __func__)
#define c_malloc(size) _c_alloc(1, size, __LINE__, __func__)
#define c_realloc(memory, size) _c_realloc(memory, 1, size, __LINE__, __func__)
#define c_free(memory) _c_free(memory, __LINE__, __func__)
#else
#define set_log_file(...)

#define close_log_file(...)

#define alloc_stats(...)

#define c_alloc(len, size) calloc(len, size)
#define c_malloc(size) malloc(size)
#define c_realloc(memory, size) realloc(memory, size)
#define c_free(memory) free(memory)
#endif

#endif