#include <assert.h> //for assert
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//log heap events and current memory ranges, bool log_events to disable
static bool log_events = true;

static void log_event(const char *fmt, ...) {
    if(log_events){
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

//Backing storage to allocate
#define STORAGE_SIZE 4096
static char storage[STORAGE_SIZE];
static const char *storage_end = storage + STORAGE_SIZE;

//Store records in heap. Heap divided in ranges
typedef struct {
    uint64_t size : 63;
    bool allocates : 1;
} Header;

//Place header at start of each range, point to first byte after header
_Static_assert(sizeof(Header) == sizeof(uint64_t));

void log_header(Header header) {
    log_event("0x%0161x (%s, size = %u bytes)\n", header,
            header.allocated ? "allocated" : "free", header.size);
}

static Header read_header(const char *ptr) { return *(Header *)ptr;}

static void write_header(char *ptr, Header header) {
    *(Header *)ptr = header;
    log_event("[%p] Set header to ", ptr);
    log_header(header);
}

//log a table showing ranges marked in heap
static void log_ranges() {
    Header header = {.size = 0, .allocated = false};
    for (const char *header_ptr = storage; header_ptr < storage_end; header _ptr += header.size) {
        header = read_header(header_ptr);
        log_event("  [%p -> %p) : ", header_ptr, header_ptr + header.size);
        log_header(header);
    }
}

void simple_heap_init() {
    log_event("Simple heap init:\n");
    log_event("Storage [%p -> %p) (%d bytes)\n", storage, storage_end. STORAGE_SIZE);

    //On start-up, heap is one free range
    Header hdr = {.size = STORAGE_SIZE, .allocated = false};
    write_header(storage, hdr);
    log_ranges();
}

//Search for free range >=size
static char *find_free_space(size_t bytes) {
    Header header = {.size = 0, .allocated = false};
    for (char *header_ptr = storage; header_ptr < storage_end; header_ptr = header.size) {
        header = read_header(header_ptr);
        assert(header.size != 0 && "Header should always have non-zero size.");
        if (!header.allocated && (header.size >= bytes))
            return header_ptr;
    }
    return NULL;
}

//Split a free range into two
static void split_range(char *range, uint64_t size) {
    Header original_header = read_header(range);
    assert(!original_header.allocated && "Shouldn't be splitting an allocated range.");

    //Mark what needs to be allocated
    Header new_header = {.size = size, .allocated = true};
    write_header(range, new_header);

    //Split free space from allocated and add header
    uint64_t remaning = original_header.size - size;
    if (remaning) {
        Header free_header = {.size = remaining , .allocated = false};
        write_header(range + size, free_header);
    }
}

//Return NULL for 0 size or we run out of heap memory
void *simple_malloc (size_t size) {
    if (!size)
        return NULL;
    log_event("\nTrying to allocated %ld bytes\n", size);

    //extra space for header
    uint64_t required_size = size + sizeof(Header);
    char *allocated = find_free_space(required_size);

    if(!allocated) {
        log_event("Heap exhausted. \n");
        return NULL;
    }
    
    //Split found range into new allocation and free
    split_range(allocated, required_size);

    //Return a pointer to after header
    allocated += sizeof(Header);

    log_event("[%p] Memory was allocated, size %ld bytes\n", allocated, size);
    log_ranges();
    return allocated;
}

//Free range
void simple_free ( void *ptr ) {
    //If no pointer, return and check if valid
    if (!ptr)
        return;
        
        assert(((char*)ptr > storage) && ((char*)ptr < storage_end) && "Trying to free pointer that is not within the heap.");
    log_event("\n[%p] Freeing allocation\n", ptr);

    //Back up to before header of range
    char *header_ptr = (char *)ptr - sizeof(Header);

    Header header = read_header(header_ptr);
    assert(header.size != 0 && "Can't free an allocation of zero size.");

    //Mark range as free
    header.allocated = false;
    write_header(header_ptr, header);

    log_event("[%p] Memory was freed\n", ptr);
    log_ranges();
}