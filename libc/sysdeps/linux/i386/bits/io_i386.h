/* Copyright (C) 1996, 2000 Free Software Foundation, Inc.
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

/*
 * June 16, 2001     hacked for uClibc inclusion by Manuel Novoa III
 *
 * This was originally sys/io.h from glibc-2.2.2.  However, uClibc
 * currently shares the same sys/io.h for all archs.  So, I placed
 * moved and renamed it to bits/io_i386.h and modified sys/io.h to
 * conditionally include this if __i386__ was defined.
 */

#if !defined _SYS_IO_H || defined _BITS_IO_I386_H
#error Never include <bits/io_i386.h> directly; use <sys/io.h> instead.
#endif

#define	_BITS_IO_I386_H    1

#if defined __GNUC__ && __GNUC__ >= 2

static __inline unsigned char
inb (unsigned short int port)
{
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline unsigned char
inb_p (unsigned short int port)
{
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline unsigned short int
inw (unsigned short int port)
{
  unsigned short _v;

  __asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline unsigned short int
inw_p (unsigned short int port)
{
  unsigned short int _v;

  __asm__ __volatile__ ("inw %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline unsigned int
inl (unsigned short int port)
{
  unsigned int _v;

  __asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline unsigned int
inl_p (unsigned short int port)
{
  unsigned int _v;
  __asm__ __volatile__ ("inl %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
  return _v;
}

static __inline void
outb (unsigned char value, unsigned short int port)
{
  __asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (port));
}

static __inline void
outb_p (unsigned char value, unsigned short int port)
{
  __asm__ __volatile__ ("outb %b0,%w1\noutb %%al,$0x80": :"a" (value),
			"Nd" (port));
}

static __inline void
outw (unsigned short int value, unsigned short int port)
{
  __asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd" (port));

}

static __inline void
outw_p (unsigned short int value, unsigned short int port)
{
  __asm__ __volatile__ ("outw %w0,%w1\noutb %%al,$0x80": :"a" (value),
			"Nd" (port));
}

static __inline void
outl (unsigned int value, unsigned short int port)
{
  __asm__ __volatile__ ("outl %0,%w1": :"a" (value), "Nd" (port));
}

static __inline void
outl_p (unsigned int value, unsigned short int port)
{
  __asm__ __volatile__ ("outl %0,%w1\noutb %%al,$0x80": :"a" (value),
			"Nd" (port));
}

static __inline void
insb (unsigned short int port, void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; insb":"=D" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

static __inline void
insw (unsigned short int port, void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; insw":"=D" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

static __inline void
insl (unsigned short int port, void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; insl":"=D" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

static __inline void
outsb (unsigned short int port, const void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; outsb":"=S" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

static __inline void
outsw (unsigned short int port, const void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; outsw":"=S" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

static __inline void
outsl (unsigned short int port, const void *addr, unsigned long int count)
{
  __asm__ __volatile__ ("cld ; rep ; outsl":"=S" (addr),
			"=c" (count):"d" (port), "0" (addr), "1" (count));
}

#endif	/* GNU C */
