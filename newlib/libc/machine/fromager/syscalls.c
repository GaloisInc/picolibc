// System calls and other low-level function implementations.  These are used
// when running on the MicroRAM target.

#include <sys/types.h>
#include <sys/socket.h>
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

int socket(int domain, int type, int protocol) {
    __cc_valid_if(0, "unimplemented socket");
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    __cc_valid_if(0, "unimplemented bind");
}

int listen(int sockfd, int backlog) {
    __cc_valid_if(0, "unimplemented listen");
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    __cc_valid_if(0, "unimplemented accept");
}

int close(int fd) {
    __cc_valid_if(0, "unimplemented close");
}

int open(const char* name, int flags, ...) {
    __cc_valid_if(0, "unimplemented open");
}

uid_t getuid(void) {
    __cc_valid_if(0, "unimplemented getuid");
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    __cc_valid_if(0, "unimplemented socket");
}

int shutdown(int sockfd, int how) {
    __cc_valid_if(0, "unimplemented shutdown");
}

off_t lseek(int fd, off_t offset, int whence) {
    __cc_valid_if(0, "unimplemented lseek");
}

int ioctl(int fd, unsigned long request, ...) {
    __cc_valid_if(0, "unimplemented ioctl");
}

_READ_WRITE_RETURN_TYPE read(int fd, void* buf, size_t count) {
    __cc_valid_if(0, "unimplemented read");
}
