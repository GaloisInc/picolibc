#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
// #include <time.h>
#include <unistd.h>
#include "cc_native.h"

void* cc_native_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, length, prot, flags, fd, offset);
}

int cc_native_open(const char* name, int flags, int mode) {
    return open(name, flags, mode);
}

int cc_native_close(int fd) {
    return close(fd);
}

ssize_t cc_native_write(int fd, const void* buf, size_t count) {
    return write(fd, buf, count);
}

ssize_t cc_native_read(int fd, void* buf, size_t count) {
    return read(fd, buf, count);
}

void cc_native_exit(int status) {
    exit(status);
}

int cc_native_shutdown(int sockfd, int how) {
    return shutdown(sockfd, how);
}

off_t cc_native_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int cc_native_fstat(int fd, struct stat* st) {
    return fstat(fd, st);
}

int cc_native_poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    return poll(fds, nfds, timeout);
}

int cc_native_socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}

int cc_native_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return bind(sockfd, addr, addrlen);
}

int cc_native_listen(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

int cc_native_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return accept(sockfd, addr, addrlen);
}

int cc_native_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect(sockfd, addr, addrlen);
}

pid_t cc_native_getpid(void) {
    return getpid();
}

uid_t cc_native_getuid(void) {
    return getuid();
}

// time_t cc_native_time(time_t * t) {
//     return time(t);
// }

int cc_native_gettimeofday (struct timeval *__restrict p, void *__restrict tz) {
    return gettimeofday(p, tz);
}

int cc_native_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    return setsockopt(sockfd, level, optname, optval, optlen);
}

int cc_native_kill(pid_t pid, int sig) {
    return kill(pid, sig);
}
