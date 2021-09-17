#include <stddef.h>

void* cc_native_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);

int cc_native_open(const char* name, int flags, int mode);
int cc_native_close(int fd);

ssize_t cc_native_write(int fd, const void* buf, size_t count);
ssize_t cc_native_read(int fd, void* buf, size_t count);

void cc_native_exit(int status);

