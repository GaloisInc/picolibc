// System calls and other low-level function implementations.  These are used
// when running on the MicroRAM target.

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <unistd.h>
#include "fromager.h"
#include "noni.h"

// Initialize the current label to `bottom`.
label cc_current_label = bottom;

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

uid_t getuid(void) {
    // Hardcoding the UID on my machine.
    return 1000;
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

extern unsigned char read_buf_0[32];
extern unsigned char read_buf_1[11];
extern unsigned char read_buf_2[220];
extern unsigned char read_buf_3[5];
extern unsigned char read_buf_4[518];
extern unsigned char read_buf_5[5];
extern unsigned char read_buf_6[1];
extern unsigned char read_buf_7[5];
extern unsigned char read_buf_8[40];
extern unsigned char read_buf_9[5];
extern unsigned char read_buf_10[249];
extern unsigned char read_buf_11[11];
extern unsigned char read_buf_12[214];
extern unsigned char read_buf_13[5];
extern unsigned char read_buf_14[3];

const size_t READ_COUNTS = 15;
const unsigned char* read_bufs[READ_COUNTS] = {read_buf_0,read_buf_1,read_buf_2,read_buf_3,read_buf_4,read_buf_5,read_buf_6,read_buf_7,read_buf_8,read_buf_9,read_buf_10,read_buf_11,read_buf_12,read_buf_13,read_buf_14};
const unsigned int read_lens[READ_COUNTS] = {32, 11, 220, 5, 518, 5, 1, 5, 40, 5, 249, 11, 214, 5, 3};

_READ_WRITE_RETURN_TYPE read(int fd, void* buf, size_t count) {
    static int i = 0;
    __cc_valid_if(i < READ_COUNTS, "Too many read calls.");
    __cc_valid_if(read_lens[i] <= count, "Buffer is not large enough to perform read.");

    for (int pos = 0; pos < read_lens[i]; pos++) {
        ((uint8_t*) buf)[pos] = read_bufs[i][pos];
    }
    
    i++;
}
