#ifndef _LD_DEFS_H
#define _LD_DEFS_H

#define LIB_ANY	     -1
#define LIB_DLL       0
#define LIB_ELF       1
#define LIB_ELF64     0x80
#define LIB_ELF_LIBC5 2
#define LIB_ELF_LIBC6 3
#define LIB_ELF_LIBC0 4

#if defined(__LDSO_PRELOAD_FILE_SUPPORT__) || defined(__LDSO_CACHE_SUPPORT__)
#ifndef __LDSO_BASE_FILENAME__
#define __LDSO_BASE_FILENAME__ "ld.so"
#endif
#define LDSO_BASE_PATH UCLIBC_RUNTIME_PREFIX "etc/" __LDSO_BASE_FILENAME__

#ifdef __LDSO_PRELOAD_FILE_SUPPORT__
#define LDSO_PRELOAD LDSO_BASE_PATH ".preload"
#endif

#ifdef __LDSO_CACHE_SUPPORT__
#define LDSO_CONF    LDSO_BASE_PATH ".conf"
#define LDSO_CACHE   LDSO_BASE_PATH ".cache"

#define LDSO_CACHE_MAGIC "ld.so-"
#define LDSO_CACHE_MAGIC_LEN (sizeof LDSO_CACHE_MAGIC -1)
#define LDSO_CACHE_VER "1.7.0"
#define LDSO_CACHE_VER_LEN (sizeof LDSO_CACHE_VER -1)

typedef struct {
	char magic   [LDSO_CACHE_MAGIC_LEN];
	char version [LDSO_CACHE_VER_LEN];
	int nlibs;
} header_t;

typedef struct {
	int flags;
	int sooffset;
	int liboffset;
} libentry_t;
#endif	/* __LDSO_CACHE_SUPPORT__ */

#endif

#endif	/* _LD_DEFS_H */
