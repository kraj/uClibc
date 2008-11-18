/*
 * Support for dynamic linking code in static libc.
 * Copyright (C) 1996-2002, 2003, 2004, 2005 Free Software Foundation, Inc.
 *
 * Partially based on GNU C Library (file: libc/elf/dl-support.c)
 *
 * Copyright (C) 2008 STMicroelectronics Ltd.
 * Author: Carmelo Amoroso <carmelo.amoroso@st.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 */

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
