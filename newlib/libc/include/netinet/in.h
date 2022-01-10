/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */


#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_ 1

#include <inttypes.h>
#include <sys/socket.h>

#include <bits/netinet_in.h>

#include <machine/endian.h>

#define htonl(x)  __htonl(x)
#define htons(x)  __htons(x)
#define ntohl(x)  __ntohl(x)
#define ntohs(x)  __ntohs(x)

#endif
