/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2000-2006 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Lesser General Public License version 2.1 or later.
 */

#ifndef _DL_HASH_H
#define _DL_HASH_H

#include <dlfcn.h>
#ifndef RTLD_NEXT
#define RTLD_NEXT	((void*)-1)
#endif

#define RELOCS_DONE	    0x000001
#define JMP_RELOCS_DONE	    0x000002
#define INIT_FUNCS_CALLED   0x000004
#define FINI_FUNCS_CALLED   0x000008
#define DL_OPENED	    0x000010

#define LD_ERROR_NOFILE 1
#define LD_ERROR_NOZERO 2
#define LD_ERROR_NOTELF 3
#define LD_ERROR_NOTMAGIC 4
#define LD_ERROR_NOTDYN 5
#define LD_ERROR_MMAP_FAILED 6
#define LD_ERROR_NODYNAMIC 7
#define LD_ERROR_TLS_FAILED 8
#define LD_WRONG_RELOCS 9
#define LD_BAD_HANDLE 10
#define LD_NO_SYMBOL 11

#endif /* _DL_HASH_H */
