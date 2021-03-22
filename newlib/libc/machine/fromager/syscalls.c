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
