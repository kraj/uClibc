#ifndef _LDSO_H_
#define _LDSO_H_

#include <features.h>

/* Prepare for the case that `__builtin_expect' is not available.  */
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif
#ifndef likely
# define likely(x)	__builtin_expect((!!(x)),1)
#endif
#ifndef unlikely
# define unlikely(x)	__builtin_expect((!!(x)),0)
#endif
#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H
#endif

/* Pull in compiler and arch stuff */
#include <stdlib.h>
#include <stdarg.h>
/* Pull in the arch specific type information */
#include <sys/types.h>
/* Pull in the ldso syscalls and string functions */
#include <dl-syscall.h>
#include <dl-string.h>
/* Pull in the arch specific page size */
#include <bits/uClibc_page.h>
/* Now the ldso specific headers */
#include <dl-elf.h>
#include <dl-hash.h>

/* For INIT/FINI dependency sorting. */
struct init_fini_list {
	struct init_fini_list *next;
	struct elf_resolve *tpnt;
};

/* Global variables used within the shared library loader */
extern char *_dl_library_path;         /* Where we look for libraries */
extern char *_dl_preload;              /* Things to be loaded before the libs */
extern char *_dl_ldsopath;             /* Where the shared lib loader was found */
extern const char *_dl_progname;       /* The name of the executable being run */
extern unsigned char *_dl_malloc_addr; /* Lets _dl_malloc use the already allocated memory page */
extern unsigned char *_dl_mmap_zero;   /* Also used by _dl_malloc */
extern int _dl_secure;                 /* Are we dealing with setuid stuff? */
extern size_t _dl_pagesize;            /* Store the page size for use later */
extern const char *_dl_progname;       /* The name of the shared library loader */

#ifdef __SUPPORT_LD_DEBUG__
extern char *_dl_debug;
extern char *_dl_debug_symbols;
extern char *_dl_debug_move;
extern char *_dl_debug_reloc;
extern char *_dl_debug_detail;
extern char *_dl_debug_nofixups;
extern char *_dl_debug_bindings;
extern int   _dl_debug_file;
#else
#define _dl_debug_file 2
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

extern void *_dl_malloc(int size);
extern char *_dl_getenv(const char *symbol, char **envp);
extern void _dl_unsetenv(const char *symbol, char **envp);
extern char *_dl_strdup(const char *string);
extern void _dl_dprintf(int, const char *, ...);

extern void _dl_get_ready_to_run(struct elf_resolve *tpnt, unsigned long load_addr,
		Elf32_auxv_t auxvt[AT_EGID + 1], char **envp, char **argv);


#endif /* _LDSO_H_ */

