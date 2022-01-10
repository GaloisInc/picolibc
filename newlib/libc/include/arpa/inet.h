#ifndef _ARPA_INET_H_
#define _ARPA_INET_H_ 1

#include <netinet/in.h>

in_addr_t    inet_addr(const char *);
char        *inet_ntoa(struct in_addr);
const char  *inet_ntop(int, const void *restrict, char *restrict,
                 socklen_t);
int          inet_pton(int, const char *restrict, void *restrict);

#endif
