/* vi: set sw=4 ts=4: */
/*
 * Thread-local storage handling in the ELF dynamic linker.
 *
 * Copyright (C) 2005 by Steven J. Hill <sjhill@realitydiluted.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <tls.h>
#include <dl-tls.h>

/* Taken from glibc/sysdeps/generic/dl-tls.c */
#ifndef GET_ADDR_ARGS
# define GET_ADDR_ARGS		tls_index *ti
#endif
#ifndef GET_ADDR_MODULE
# define GET_ADDR_MODULE	ti->ti_module
#endif
#ifndef GET_ADDR_OFFSET
# define GET_ADDR_OFFSET	ti->ti_offset
#endif

/* Taken from glibc/elf/dl-reloc.c */
#define CHECK_STATIC_TLS(sym_map)											\
	do {																	\
		if (__builtin_expect ((sym_map)->l_tls_offset == NO_TLS_OFFSET, 0))	\
			_dl_allocate_static_tls (sym_map);								\
	} while (0)

/* Taken from glibc/elf/dl-reloc.c */
void 
internal_function __attribute_noinline__
_dl_allocate_static_tls (struct elf_resolve *map)
{
	_dl_dprintf(2, "_dl_allocate_static_tls NOT IMPLEMENTED!\n");
	_dl_exit(1);
	return;
}

/* Taken from glibc/sysdeps/generic/dl-tls.c */
void *
__tls_get_addr (GET_ADDR_ARGS)
{
	dtv_t *dtv = THREAD_DTV ();
	struct link_map *the_map = NULL;
	void *p;

	if (__builtin_expect (dtv[0].counter != _dl_tls_generation, 0))
		the_map = _dl_update_slotinfo (GET_ADDR_MODULE);

	p = dtv[GET_ADDR_MODULE].pointer.val;

	if (__builtin_expect (p == TLS_DTV_UNALLOCATED, 0))
	{
		/* The allocation was deferred.  Do it now.  */
		if (the_map == NULL)
		{
			/* Find the link map for this module.  */
			size_t idx = GET_ADDR_MODULE;
			struct dtv_slotinfo_list *listp = _dl_tls_dtv_slotinfo_list;

			while (idx >= listp->len)
			{
				idx -= listp->len;
				listp = listp->next;
			}

			the_map = listp->slotinfo[idx].map;
		}

		p = dtv[GET_ADDR_MODULE].pointer.val = allocate_and_init (the_map);
		dtv[GET_ADDR_MODULE].pointer.is_static = false;
	}

	return (char *) p + GET_ADDR_OFFSET;
}
