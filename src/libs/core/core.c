#include "core.h"


uchar is_debugging = 0;


void *memalloc(size_t size) {
    void *allocated_memory = calloc(1, size);
    if (!allocated_memory && size) {
        fprintf(stderr, "Failed while allocating %d bytes of memory.\n\n", size);
        exit(EXIT_FAILURE);
    }
    
    return allocated_memory;
}