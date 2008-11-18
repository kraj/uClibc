/* Support for dynamic linking code in static libc.
   Copyright (C) 1996-2002, 2003, 2004, 2005 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* This file defines some things that for the dynamic linker are defined in
   rtld.c and dl-sysdep.c in ways appropriate to bootstrap dynamic linking.  */

#include <link.h>
#include <elf.h>

ElfW(Phdr) *_dl_phdr;
size_t _dl_phnum;

void
internal_function
_dl_aux_init (ElfW(auxv_t) *av)
{
   /* Get the program headers base address from the aux vect */
   _dl_phdr = (ElfW(Phdr) *) av[AT_PHDR].a_un.a_val;

   /* Get the number of program headers from the aux vect */
   _dl_phnum = (size_t) av[AT_PHNUM].a_un.a_val;
}
