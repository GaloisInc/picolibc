#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include "fromager.h"


// Indicate that the current trace is invalid.
void __cc_flag_invalid(void) {}
// Indicate that the current trace has exhibited a bug.
void __cc_flag_bug(void) {}

// Print a message during evaluation in the MicroRAM interpreter.
void __cc_trace(const char* msg) {
    fprintf(stderr, "[TRACE] %s\n", msg);
}


void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    register intptr_t rax __asm__ ("rax") = __NR_mmap;
    register void* rdi __asm__ ("rdi") = addr;
    register size_t rsi __asm__ ("rsi") = length;
    register int rdx __asm__ ("rdx") = prot;
    register int r10 __asm__ ("r10") = flags;
    register int r8 __asm__ ("r8") = fd;
    register off_t r9 __asm__ ("r9") = offset;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8), "r" (r9)
        : "cc", "rcx", "r11", "memory"
    );
    if (rax < 0) {
        errno = -rax;
        rax = -1;
    }
    return (void*)rax;
}

_READ_WRITE_RETURN_TYPE write(int fd, const void* buf, size_t count) {
    register intptr_t rax __asm__ ("rax") = __NR_write;
    register int rdi __asm__ ("rdi") = fd;
    register const void* rsi __asm__ ("rsi") = buf;
    register size_t rdx __asm__ ("rdx") = count;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi), "r" (rsi), "r" (rdx)
        : "cc", "rcx", "r11", "memory"
    );
    if (rax < 0) {
        errno = -rax;
        rax = -1;
    }
    return rax;
}

_READ_WRITE_RETURN_TYPE read(int fd, void* buf, size_t count) {
    register intptr_t rax __asm__ ("rax") = __NR_read;
    register int rdi __asm__ ("rdi") = fd;
    register void* rsi __asm__ ("rsi") = buf;
    register size_t rdx __asm__ ("rdx") = count;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi), "r" (rsi), "r" (rdx)
        : "cc", "rcx", "r11", "memory"
    );
    if (rax < 0) {
        errno = -rax;
        rax = -1;
    }
    return rax;
}

void _exit(int status) {
    register intptr_t rax __asm__ ("rax") = __NR_exit_group;
    register int rdi __asm__ ("rdi") = status;
    __asm__ __volatile__ (
        "syscall"
        : "+r" (rax)
        : "r" (rdi)
        : "cc", "rcx", "r11", "memory"
    );
    if (rax < 0) {
        errno = -rax;
        rax = -1;
    }
}

void abort() {
    _exit(1);
}

// Copied from bits/mman-linux.h
#define PROT_READ	0x1		/* Page can be read.  */
#define PROT_WRITE	0x2		/* Page can be written.  */
#define PROT_EXEC	0x4		/* Page can be executed.  */
#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */
#define MAP_FIXED	0x10		/* Interpret addr exactly.  */
#define MAP_ANONYMOUS	0x20		/* Don't use a file.  */

void __cc_malloc_init(void* addr) __attribute__((noinline)) {
    void* addr2 = mmap(addr, 64 * 1024 * 1024, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (addr2 != addr) {
        abort();
    }
}

void __cc_malloc_init_from_snapshot(void* addr, size_t len) __attribute__((noinline)) {
    __cc_valid_if(__cc_malloc_heap_end() == NULL,
        "heap has already been initialized");
    void* heap = __cc_malloc_heap_start();
    void* heap2 = mmap(heap, 64 * 1024 * 1024, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (heap2 != heap) {
        abort();
    }
    memcpy(heap, addr, len);
    __cc_malloc_set_heap_end(heap + len);
}


static int fromager_getchar(FILE *file) {
    char c;
    read(0, &c, 1);
    return c;
}

static int fromager_putchar(char c, FILE *file) {
    write(1, &c, 1);
    return 0;
}

static int fromager_putchar_err(char c, FILE *file) {
    write(2, &c, 1);
    return 0;
}

static FILE __stdin = FDEV_SETUP_STREAM(NULL, fromager_getchar, NULL, _FDEV_SETUP_READ);
static FILE __stdout = FDEV_SETUP_STREAM(fromager_putchar, NULL, NULL, _FDEV_SETUP_WRITE);
static FILE __stderr = FDEV_SETUP_STREAM(fromager_putchar_err, NULL, NULL, _FDEV_SETUP_WRITE);

FILE *const __iob[3] = { &__stdin, &__stdout, &__stderr };
