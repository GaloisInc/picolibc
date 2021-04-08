#include <sys/time.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "fromager.h"

_READ_WRITE_RETURN_TYPE write (int fd, const void *buf, size_t nbyte) {
    return nbyte;
}

pid_t getpid() {
    return 1;
}

int kill(pid_t pid, int sig) {
    __cc_valid_if(0, "kill unsupported");
    return 0;
}

int gettimeofday (struct timeval *__restrict p, void *__restrict tz) {
    if (p != NULL) {
        p->tv_sec = 0;
        p->tv_usec = 0;
    }
    return 0;
}

void _exit(int status) {
    for (;;) {
        // Ensure there's an observable side effect inside the loop, so the
        // compiler can't optimize it away.
        __cc_trace("_exit");
    }
}

void __cc_malloc_init(void* addr) __attribute__((noinline)) {
    // No-op.  In `syscalls_native,c`, this is replaced with an `mmap` call.
}

void __cc_malloc_init_from_snapshot(void* addr, size_t len) __attribute__((noinline)) {
    __cc_valid_if(addr == __cc_malloc_heap_start(),
        "heap snapshot was placed at the wrong location");
    __cc_valid_if(__cc_malloc_heap_end() == NULL,
        "heap has already been initialized");
    __cc_malloc_set_heap_end(addr + len);
    // TODO: mark only allocated parts of the heap as valid
    // (This will require walking through the heap metadata.)
    __cc_access_valid(addr, addr + len);
}
