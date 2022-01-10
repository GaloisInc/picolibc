
#ifndef _TERMIO_H_
#define _TERMIO_H_ 1

#define NCC 8
struct termio
  {
    unsigned short int c_iflag;		/* input mode flags */
    unsigned short int c_oflag;		/* output mode flags */
    unsigned short int c_cflag;		/* control mode flags */
    unsigned short int c_lflag;		/* local mode flags */
    unsigned char c_line;		/* line discipline */
    unsigned char c_cc[NCC];		/* control characters */
};

/* c_lflag bits */
#define ISIG	0000001
#define ICANON	0000002
#define XCASE	0000004
#define ECHO	0000010
#define ECHOE	0000020
#define ECHOK	0000040
#define ECHONL	0000100
#define NOFLSH	0000200
#define TOSTOP	0000400
#define ECHOCTL	0001000
#define ECHOPRT	0002000
#define ECHOKE	0004000
#define FLUSHO	0010000
#define PENDIN	0040000
#define IEXTEN	0100000
#define EXTPROC	0200000

/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#define TCGETA		0x5405
#define TCSETA		0x5406

#endif
