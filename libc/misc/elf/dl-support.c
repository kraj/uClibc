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

#include <assert.h>
#include <tls.h>
#include <link.h>
#include <ldsodefs.h>
#include <string.h>

void (*_dl_init_static_tls) (struct link_map *) = &_dl_nothread_init_static_tls;

ElfW(Phdr) *_dl_phdr;
size_t _dl_phnum;

void
internal_function
_dl_aux_init (ElfW(auxv_t) *av)
{
   /* Get the program headers base address from the aux vect */
   GL(dl_phdr) = (ElfW(Phdr) *) av[AT_PHDR].a_un.a_val;

   /* Get the number of program headers from the aux vect */
   GL(dl_phnum) = (size_t) av[AT_PHNUM].a_un.a_val;
}

/* Initialize static TLS area and DTV for current (only) thread.
   libpthread implementations should provide their own hook
   to handle all threads.  */
void
_dl_nothread_init_static_tls (struct link_map *map)
{
# if TLS_TCB_AT_TP
  void *dest = (char *) THREAD_SELF - map->l_tls_offset;
# elif TLS_DTV_AT_TP
  void *dest = (char *) THREAD_SELF + map->l_tls_offset + TLS_PRE_TCB_SIZE;
# else
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif

  /* Fill in the DTV slot so that a later LD/GD access will find it.  */
  dtv_t *dtv = THREAD_DTV ();
  assert (map->l_tls_modid <= dtv[-1].counter);
  dtv[map->l_tls_modid].pointer.val = dest;
  dtv[map->l_tls_modid].pointer.is_static = true;

  /* Initialize the memory.  */
  memset (mempcpy (dest, map->l_tls_initimage, map->l_tls_initimage_size),
	  '\0', map->l_tls_blocksize - map->l_tls_initimage_size);
}
