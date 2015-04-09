/* linux/termios.h - Missing headers for glibc on Linux for use with TCC
 * Copyright 2013, Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef GLIBC_MISSING_HEADERS_LINUX_TERMIOS_H_INC
#define GLIBC_MISSING_HEADERS_LINUX_TERMIOS_H_INC 1

#pragma pack(push, 4)

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

#define NCCS 32
struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_line;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

#pragma pack(pop)

#define VINTR     0
#define VQUIT     1
#define VERASE    2
#define VKILL     3
#define VEOF      4
#define VTIME     5
#define VMIN      6
#define VSWTC     7
#define VSTART    8
#define VSTOP     9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

#define IGNBRK  0x0001
#define BRKINT  0x0002
#define IGNPAR  0x0004
#define PARMRK  0x0008
#define INPCK   0x0010
#define ISTRIP  0x0020
#define INLCR   0x0040
#define IGNCR   0x0080
#define ICRNL   0x0100
#define IUCLC   0x0200
#define IXON    0x0400
#define IXANY   0x0800
#define IXOFF   0x1000
#define IMAXBEL 0x2000

#define OPOST  0x0001
#define OLCUC  0x0002
#define ONLCR  0x0004
#define OCRNL  0x0008
#define ONOCR  0x0010
#define ONLRET 0x0020
#define OFILL  0x0040
#define OFDEL  0x0080
#define VTDLY  0x4000
#define VT0    0x0000
#define VT1    0x4000

#define B0       0x0000
#define B50      0x0001
#define B75      0x0002
#define B110     0x0003
#define B134     0x0004
#define B150     0x0005
#define B200     0x0006
#define B300     0x0007
#define B600     0x0008
#define B1200    0x0009
#define B1800    0x000A
#define B2400    0x000B
#define B4800    0x000C
#define B9600    0x000D
#define B19200   0x000E
#define B38400   0x000F
#define CSIZE    0x0030
#define CS5      0x0000
#define CS6      0x0010
#define CS7      0x0020
#define CS8      0x0030
#define CSTOPB   0x0040
#define CREAD    0x0080
#define PARENB   0x0100
#define PARODD   0x0200
#define HUPCL    0x0400
#define CLOCAL   0x0800
#define B57600   0x1001
#define B115200  0x1002
#define B230400  0x1003
#define B460800  0x1004
#define B500000  0x1005
#define B576000  0x1006
#define B921600  0x1007
#define B1000000 0x1008
#define B1152000 0x1009
#define B1500000 0x100A
#define B2000000 0x100B
#define B2500000 0x100C
#define B3000000 0x100D
#define B3500000 0x100E
#define B4000000 0x100F
#define CBAUD    0x100F

#define ISIG   0x0001
#define ICANON 0x0002
#define ECHO   0x0008
#define ECHOE  0x0010
#define ECHOK  0x0020
#define ECHONL 0x0040
#define NOFLSH 0x0080
#define TOSTOP 0x0100
#define IEXTEN 0x8000

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

#define TCSANOW   0
#define TCSADRAIN 1
#define TCSAFLUSH 2

#endif /* ! defined(GLIBC_MISSING_HEADERS_LINUX_TERMIOS_H_INC) */
