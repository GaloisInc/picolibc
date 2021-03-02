#include <unistd.h>

_READ_WRITE_RETURN_TYPE write (int fd, const void *buf, size_t nbyte) {
    return nbyte;
}
