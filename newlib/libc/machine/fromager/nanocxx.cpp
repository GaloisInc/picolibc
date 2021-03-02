#include <stdlib.h>
#include <new>

void* operator new(std::size_t sz) throw(std::bad_alloc) {
    return malloc(sz);
}

void operator delete(void* ptr)  throw() {
    free(ptr);
}
