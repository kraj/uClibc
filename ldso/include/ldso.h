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
