#ifndef _LDSO_H_
#define _LDSO_H_

#include <features.h>
/* Pull in compiler and arch stuff */
#include <stdlib.h>
#include <stdarg.h>
/* Pull in the arch specific type information */
#include <sys/types.h>
/* Now the ldso specific headers */
#include <ld_elf.h>
#include <ld_syscall.h>
#include <ld_hash.h>
#include <ld_string.h>
/* Pull in the arch specific page size */
#include <asm/page.h>
#ifndef PAGE_SIZE
#  define PAGE_SHIFT		12
#  define PAGE_SIZE		(1UL << PAGE_SHIFT)
#endif

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


/* Global variables used within the shared library loader */
extern char *_dl_library_path;         /* Where we look for libraries */
extern char *_dl_preload;              /* Things to be loaded before the libs */
extern char *_dl_ldsopath;             /* Where the shared lib loader was found */
extern const char *_dl_progname;       /* The name of the executable being run */
extern unsigned char *_dl_malloc_addr; /* Lets _dl_malloc use the already allocated memory page */
extern unsigned char *_dl_mmap_zero;   /* Also used by _dl_malloc */
extern unsigned long *_dl_brkp;        /* The end of the data segment for brk and sbrk */
extern unsigned long *_dl_envp;        /* The environment address */
extern int _dl_secure;                 /* Are we dealing with setuid stuff? */

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

extern void _dl_get_ready_to_run(struct elf_resolve *tpnt, struct elf_resolve *app_tpnt,
		unsigned long load_addr, unsigned long *hash_addr,
		Elf32_auxv_t auxvt[AT_EGID + 1], char **envp, struct r_debug *debug_addr,
		unsigned char *malloc_buffer, unsigned char *mmap_zero, char **argv);


#endif /* _LDSO_H_ */

