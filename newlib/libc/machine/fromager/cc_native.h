#include <stddef.h>
#include <poll.h>

void* cc_native_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);

int cc_native_open(const char* name, int flags, int mode);
int cc_native_close(int fd);

ssize_t cc_native_write(int fd, const void* buf, size_t count);
ssize_t cc_native_read(int fd, void* buf, size_t count);

void cc_native_exit(int status) __attribute__((noreturn));

int cc_native_socket(int domain, int type, int protocol);
int cc_native_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int cc_native_listen(int sockfd, int backlog);
int cc_native_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int cc_native_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int cc_native_shutdown(int sockfd, int how);
off_t cc_native_lseek(int fd, off_t offset, int whence);
int cc_native_fstat(int fd, struct stat* st);
int cc_native_poll(struct pollfd *fds, nfds_t nfds, int timeout);
pid_t cc_native_getpid(void);
// time_t cc_native_time(time_t * t);
uid_t cc_native_getuid(void);
int cc_native_kill(pid_t pid, int sig);
int cc_native_gettimeofday (struct timeval *__restrict p, void *__restrict tz);
int cc_native_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
