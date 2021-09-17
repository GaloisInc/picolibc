#include <sys/mman.h>
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

