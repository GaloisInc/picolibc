// System calls and other low-level function implementations.  These are used
// when running on the MicroRAM target.

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <poll.h>
#include <sys/stat.h>
#include "fromager.h"
#include "noni.h"

// Initialize the current label to `bottom`.
label cc_current_label = bottom;

_READ_WRITE_RETURN_TYPE write (int fd, const void *buf, size_t nbyte) {
    return nbyte;
}

int kill(pid_t pid, int sig) {
    __cc_valid_if(0, "kill unsupported");
    return 0;
}

void _exit(int status) {
    // Note that `_exit` and `__cc_answer` are both declared `noreturn`.
    __cc_answer(0);
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
    // __cc_valid_if(0, "unimplemented socket");
    return 3;
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    // __cc_valid_if(0, "unimplemented setsockopt");
    return 0;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    // __cc_valid_if(0, "unimplemented bind");
    return 0;
}

int listen(int sockfd, int backlog) {
    // __cc_valid_if(0, "unimplemented listen");
    return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    // __cc_valid_if(0, "unimplemented accept");
    return 4;
}

int close(int fd) {
    // __cc_valid_if(0, "unimplemented close");
    return 0;
}

int open(const char* name, int flags, ...) {
    // __cc_valid_if(0, "unimplemented open");
    return 3;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    __cc_valid_if(0, "unimplemented connect");
}

int shutdown(int sockfd, int how) {
    // __cc_valid_if(0, "unimplemented shutdown");
    return 0;
}

int fstat(int fd, struct stat* st) {
    // __cc_valid_if(0, "unimplemented fstat");
    if (st) {
        st->st_dev = 5;
        st->st_ino = 1033;
    }
    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
    __cc_valid_if(0, "unimplemented lseek");
}

int ioctl(int fd, unsigned long request, ...) {
    __cc_valid_if(0, "unimplemented ioctl");
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    // __cc_valid_if(0, "unimplemented poll");
    if (fds) {
        for (int i = 0; i < nfds; i++) {
            fds[i].revents = 1;
        }
    }
    return 1;
}

// READ_COUNTS, read_bufs, and read_lens are defined in the driver and are generated from a recording run.
_READ_WRITE_RETURN_TYPE read(int fd, void* buf, size_t count) {
    static int i = 0;
    __cc_valid_if(i < READ_COUNTS, "Too many read calls.");
    __cc_valid_if(read_lens[i] <= count, "Buffer is not large enough to perform read.");

    for (int pos = 0; pos < read_lens[i]; pos++) {
        ((uint8_t*) buf)[pos] = read_bufs[i][pos];
    }
    
    return read_lens[i++];
}
