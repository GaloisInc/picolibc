#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
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
