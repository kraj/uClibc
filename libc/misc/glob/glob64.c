#include <features.h>

#ifdef __UCLIBC_HAS_LFS__

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS != 64 
#undef _FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS   64
#endif
#ifndef __USE_LARGEFILE64
# define __USE_LARGEFILE64	1
#endif
/* We absolutely do _NOT_ want interfaces silently
 * renamed under us or very bad things will happen... */
#ifdef __USE_FILE_OFFSET64
# undef __USE_FILE_OFFSET64
#endif
#include <dirent.h>
#include <glob.h>
#include <sys/stat.h>

extern struct dirent64 *__readdir64 (DIR *__dirp) __nonnull ((1)) attribute_hidden;
extern int __glob64 (__const char *__restrict __pattern, int __flags,
		   int (*__errfunc) (__const char *, int),
		   glob64_t *__restrict __pglob) __THROW attribute_hidden;
extern void __globfree (glob_t *__pglob) __THROW attribute_hidden;
extern void __globfree64 (glob64_t *__pglob) __THROW attribute_hidden;

#define dirent dirent64
#define __readdir(dirp) __readdir64(dirp)

#define glob_t glob64_t
#define __glob(pattern, flags, errfunc, pglob) \
  __glob64 (pattern, flags, errfunc, pglob)
#define glob(pattern, flags, errfunc, pglob) \
  glob64 (pattern, flags, errfunc, pglob)
#define __globfree(pglob) __globfree64 (pglob)
#define globfree(pglob) globfree64 (pglob)

#undef stat
#define stat stat64
#define __lstat __lstat64

#define __GLOB64    1
    
#include "glob.c"

#endif
