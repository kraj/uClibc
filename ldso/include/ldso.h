/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2000-2011 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifndef _LDSO_H
#define _LDSO_H

#include <features.h>
#define __need_NULL
#include <stddef.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <bits/wordsize.h>
#include <elf.h>
#include <link.h>
/* Pull in the arch specific page size */
#include <bits/uClibc_page.h>

#include <ldsodefs.h>

#ifndef __ARCH_HAS_NO_SHARED__
#include <dlfcn.h>

#include <dl-string.h>
#include <dl-elf.h>
#include <dl-hash.h>
#include <dl-syscall.h>
#ifdef __UCLIBC_HAS_TLS__
# include <tls.h>
# include <dl-tls.h>
#endif

/* common align masks, if not specified by dl-sysdep.h */
#ifndef ADDR_ALIGN
#define ADDR_ALIGN (_dl_pagesize - 1)
#endif

#ifndef PAGE_ALIGN
#define PAGE_ALIGN (~ADDR_ALIGN)
#endif

#ifndef OFFS_ALIGN
#define OFFS_ALIGN (PAGE_ALIGN & ~(1ul << (sizeof(_dl_pagesize) * 8 - 1)))
#endif

/* Global variables used within the shared library loader */
extern char *_dl_library_path attribute_hidden;	/* Where we look for libraries */
extern char *_dl_ldsopath attribute_hidden;	/* Where the shared lib loader was found */
extern size_t _dl_pagesize attribute_hidden;	/* Store the page size for later use */

#ifdef __UCLIBC_HAS_TLS__
extern void _dl_add_to_slotinfo (struct link_map  *l);
extern void ** __attribute__ ((const)) _dl_initial_error_catch_tsd (void);
#endif

#ifdef __SUPPORT_LD_DEBUG__
extern char *_dl_debug;
# define __dl_debug_dprint(fmt, args...) \
	_dl_dprintf(_dl_debug_file, "%s:%i: " fmt, __FUNCTION__, __LINE__, ## args);
# define _dl_if_debug_dprint(fmt, args...) \
	do { if (_dl_debug) __dl_debug_dprint(fmt, ## args); } while (0)
#else
# define __dl_debug_dprint(fmt, args...) do {} while (0)
# define _dl_if_debug_dprint(fmt, args...) do {} while (0)
/* disabled on purpose, _dl_debug_file should be guarded by __SUPPORT_LD_DEBUG__
# define _dl_debug_file 2*/

# define debug_sym(symtab, strtab, symtab_index)
# define debug_reloc(symtab, strtab, rpnt)

#endif /* __SUPPORT_LD_DEBUG__ */

#ifdef IS_IN_rtld
# ifdef __SUPPORT_LD_DEBUG__
#  define _dl_assert(expr)						\
	do {								\
		if (!(expr)) {						\
			__dl_debug_dprint("assert(%s)\n", #expr);	\
			_dl_exit(45);					\
		}							\
	} while (0)
# else
#  define _dl_assert(expr) ((void)0)
# endif
#else
# include <assert.h>
# define _dl_assert(expr) assert(expr)
#endif

#ifdef __SUPPORT_LD_DEBUG_EARLY__
# define _dl_debug_early(fmt, args...) __dl_debug_dprint(fmt, ## args)
#else
# define _dl_debug_early(fmt, args...) do {} while (0)
#endif /* __SUPPORT_LD_DEBUG_EARLY__ */

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifdef IS_IN_rtld
extern void *_dl_malloc(size_t size);
extern void *_dl_calloc(size_t __nmemb, size_t __size);
extern void *_dl_realloc(void *__ptr, size_t __size);
extern void _dl_free(void *);
#else
# include <stdlib.h>
# define _dl_malloc malloc
# define _dl_free free
# include <string.h>
# define _dl_strdup strdup
# include <stdio.h>
# ifdef __USE_GNU
#  define _dl_dprintf dprintf
# else
#  define _dl_dprintf(fd, fmt, args...) fprintf(stderr, fmt, ## args)
# endif
#endif

#ifndef DL_GET_READY_TO_RUN_EXTRA_PARMS
# define DL_GET_READY_TO_RUN_EXTRA_PARMS
#endif
#ifndef DL_GET_READY_TO_RUN_EXTRA_ARGS
# define DL_GET_READY_TO_RUN_EXTRA_ARGS
#endif

/* include it after _dl_assert is defined */
#ifdef HAVE_DL_INLINES_H
#include <dl-inlines.h>
#endif

#endif /* __ARCH_HAS_NO_SHARED__ */

#endif /* _LDSO_H */
