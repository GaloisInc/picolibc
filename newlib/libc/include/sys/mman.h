#ifndef SYS_MMAN_INCLUDED
#define SYS_MMAN_INCLUDED

#include <unistd.h>

#include <bits/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

#endif // SYS_MMAN_INCLUDED
