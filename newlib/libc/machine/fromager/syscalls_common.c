#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void) {
    return 1;
}

int gettimeofday (struct timeval *__restrict p, void *__restrict tz) {
    if (p != NULL) {
        p->tv_sec = 1634000000;
        p->tv_usec = 0;
    }
    return 0;
}

uid_t getuid(void) {
    return 1000;
}
