/* Copyright (C) 1998, 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Phil Blundell, based on the Alpha version by
   David Mosberger.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* I/O port access on the ARM is something of a fiction.  What we do is to
   map an appropriate area of /dev/mem into user space so that a program
   can blast away at the hardware in such a way as to generate I/O cycles
   on the bus.  To insulate user code from dependencies on particular
   hardware we don't allow calls to inb() and friends to be inlined, but
   force them to come through code in here every time.  Performance-critical
   registers tend to be memory mapped these days so this should be no big
   problem.  */

/* Once upon a time this file used mprotect to enable and disable
   access to particular areas of I/O space.  Unfortunately the
   mprotect syscall also has the side effect of enabling caching for
   the area affected (this is a kernel limitation).  So we now just
   enable all the ports all of the time.  */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>

#include <asm/page.h>
#include <sys/sysctl.h>


#define IO_BASE			0x7c000000
#define IO_SHIFT		0
#define IO_ADDR(port)	(IO_BASE + ((port) << IO_SHIFT))


#if 0
static struct {
  unsigned long int	base;
  unsigned long int	io_base;
  unsigned int		shift;
  unsigned int		initdone;	/* since all the above could be 0 */
} io;


#define MAX_PORT	0x10000
int ioperm(unsigned long int from, unsigned long int num, int turn_on)
{
  /* this test isn't as silly as it may look like; consider overflows! */
  if (from >= MAX_PORT || from + num > MAX_PORT)
    {
      __set_errno (EINVAL);
      return -1;
    }

  if (turn_on)
    {
      if (! io.base)
	{
	  int fd;

	  fd = open ("/dev/mem", O_RDWR);
	  if (fd < 0)
	    return -1;

	  io.base = (unsigned long int) mmap (0, MAX_PORT << io.shift, 
					PROT_READ | PROT_WRITE, 
					MAP_SHARED, fd, io.io_base);
	  close (fd);
	  if ((long) io.base == -1)
	    return -1;
	}
    }

  return 0;
}


int iopl (unsigned int level)
{
    if (level > 3)
      {
	__set_errno (EINVAL);
	return -1;
      }
    if (level)
      {
	return _ioperm (0, MAX_PORT, 1);
      }
    return 0;
}
#endif

void outb (unsigned char b, unsigned long int port)
{
  *((volatile unsigned char *)(IO_ADDR (port))) = b;
}


void outw (unsigned short b, unsigned long int port)
{
  *((volatile unsigned short *)(IO_ADDR (port))) = b;
}


void outl (unsigned int b, unsigned long int port)
{
  *((volatile unsigned long *)(IO_ADDR (port))) = b;
}


unsigned int inb (unsigned long int port)
{
  return *((volatile unsigned char *)(IO_ADDR (port)));
}


unsigned int inw (unsigned long int port)
{
  return *((volatile unsigned short *)(IO_ADDR (port)));
}


unsigned int inl (unsigned long int port)
{
  return *((volatile unsigned long *)(IO_ADDR (port)));
}

