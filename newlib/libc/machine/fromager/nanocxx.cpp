#include <stdlib.h>
#include <new>
#include "fromager.h"

void* operator new(std::size_t sz) throw(std::bad_alloc) {
    return malloc(sz);
}

void operator delete(void* ptr)  throw() {
    free(ptr);
}

extern "C" {
    void __cxa_pure_virtual() {
        __cc_flag_invalid();
    }
}
