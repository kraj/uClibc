/* Copyright (C) 1996, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#ifndef	_SYS_IO_H

#define	_SYS_IO_H	1
#include <features.h>

__BEGIN_DECLS

/* If TURN_ON is TRUE, request for permission to do direct i/o on the
   port numbers in the range [FROM,FROM+NUM-1].  Otherwise, turn I/O
   permission off for that range.  This call requires root privileges.  */
extern int ioperm __P ((unsigned long int __from, unsigned long int __num,
			int __turn_on));

/* Set the I/O privilege level to LEVEL.  If LEVEL is nonzero,
   permission to access any I/O port is granted.  This call requires
   root privileges. */
extern int iopl __P ((int __level));

/* The functions that actually perform reads and writes.  */
#ifdef __i386__
/*
 * For i386, glibc defines the in* and out* functions as static inlines
 * in this header file.  For a number of other archs, these appear to be
 * external functions.  Since uClibc shares this header amongst archs,
 * stash the i386 stuff in bits/io_i386.h for now to cut down on clutter
 * and still give expected behavior.
 */
#include <bits/io_i386.h>
#else
extern unsigned char inb (unsigned long port);
extern unsigned short inw (unsigned long port);
extern unsigned long inl (unsigned long port);

extern void outb (unsigned char value, unsigned long port);
extern void outw (unsigned short value, unsigned long port);
extern void outl (unsigned long value, unsigned long port);
#endif /* __i386__ */

__END_DECLS

#endif /* _SYS_IO_H */
