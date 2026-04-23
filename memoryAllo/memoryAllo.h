#include <stddef.h> //for size_t and NULL

void simple_heap_init();

void *simple_malloc(size_t size);

void simple_free(void *ptr);