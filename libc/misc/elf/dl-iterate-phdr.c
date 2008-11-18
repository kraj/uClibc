/* Get loaded objects program headers.
   Copyright (C) 2001,2002,2003,2004,2006,2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2001.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   */


#include <link.h>
#include <ldso.h>

/* we want this in libc but nowhere else */
#ifdef __USE_GNU

extern __typeof(dl_iterate_phdr) __dl_iterate_phdr;

hidden_proto(__dl_iterate_phdr)
int
__dl_iterate_phdr (int (*callback) (struct dl_phdr_info *info, size_t size, void *data), void *data)
{
	struct elf_resolve *l;
	struct dl_phdr_info info;
	int ret = 0;

	for (l = _dl_loaded_modules; l != NULL; l = l->next) {
		info.dlpi_addr = l->loadaddr;
		info.dlpi_name = l->libname;
		info.dlpi_phdr = l->ppnt;
		info.dlpi_phnum = l->n_phent;
		ret = callback (&info, sizeof (struct dl_phdr_info), data);
		if (ret)
			break;
	}
	return ret;
}
hidden_def (__dl_iterate_phdr)

# ifdef SHARED

weak_alias(__dl_iterate_phdr, dl_iterate_phdr)

# else

/* dl-support.c defines these and initializes them early on.  */
extern ElfW(Phdr) *_dl_phdr;
extern size_t _dl_phnum;

int
dl_iterate_phdr (int (*callback) (struct dl_phdr_info *info,
                                  size_t size, void *data), void *data)
{
  if (_dl_phnum != 0)
    {
      /* This entry describes this statically-linked program itself.  */
      struct dl_phdr_info info;
      int ret;
      info.dlpi_addr = 0;
      info.dlpi_name = "";
      info.dlpi_phdr = _dl_phdr;
      info.dlpi_phnum = _dl_phnum;
      ret = (*callback) (&info, sizeof (struct dl_phdr_info), data);
      if (ret)
        return ret;
    }
   /* Then invoke callback on loaded modules, if any */
  return __dl_iterate_phdr (callback, data);
}

# endif
#endif
