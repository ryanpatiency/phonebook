#include <stddef.h>
typedef struct pool {
    char * next;
    char * end;
} POOL;

POOL *mpool;

POOL * pool_create(size_t size);

void pool_destroy(POOL *p);

size_t pool_available(POOL *p);

void * pool_alloc(POOL *p, size_t size);
