#include "heap.h"

int main() {
    simple_heap_init();

    char *ptr = simple_malloc(100);
    char *ptr2 = simple_malloc(200);

    simple_free(ptr);
    simple_free(ptr2);

    return 0;
}