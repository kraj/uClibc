#include "syscalls.h"

/* Here are the definitions for some syscalls that are used
   by the dynamic linker.  The idea is that we want to be able
   to call these before the errno symbol is dynamicly linked, so
   we use our own version here.  Note that we cannot assume any
   dynamic linking at all, so we cannot return any error codes.
   We just punt if there is an error. */


#define __NR__dl_exit __NR_exit
static inline _syscall1(void, _dl_exit, int, status);


#define __NR__dl_close __NR_close
static inline _syscall1(int, _dl_close, int, fd);


#define __NR__dl_mmap_real __NR_mmap
static inline _syscall1(void *, _dl_mmap_real, unsigned long *, buffer);

static inline void * _dl_mmap(void * addr, unsigned long size, int prot,
		int flags, int fd, unsigned long offset)
{
	unsigned long buffer[6];

	buffer[0] = (unsigned long) addr;
	buffer[1] = (unsigned long) size;
	buffer[2] = (unsigned long) prot;
	buffer[3] = (unsigned long) flags;
	buffer[4] = (unsigned long) fd;
	buffer[5] = (unsigned long) offset;
	return (void *) _dl_mmap_real(buffer);
}
#ifndef _dl_MAX_ERRNO
#define _dl_MAX_ERRNO 4096
#endif
#define _dl_mmap_check_error(__res)	\
	(((int)__res) < 0 && ((int)__res) >= -_dl_MAX_ERRNO)



#define __NR__dl_open __NR_open
#define O_RDONLY        0x0000
static inline _syscall2(int, _dl_open, const char *, fn, int, flags);

#define __NR__dl_write __NR_write
static inline _syscall3(unsigned long, _dl_write, int, fd, 
	    const void *, buf, unsigned long, count);


#define __NR__dl_read __NR_read
static inline _syscall3(unsigned long, _dl_read, int, fd, 
	    const void *, buf, unsigned long, count);

#define __NR__dl_mprotect __NR_mprotect
static inline _syscall3(int, _dl_mprotect, const void *, addr, unsigned long, len, int, prot);



/* Pull in whatever this particular arch's kernel thinks the kernel version of
 * struct stat should look like.  It turns out that each arch has a different
 * opinion on the subject, and different kernel revs use different names... */
#define __NR__dl_stat	__NR_stat
#define stat kernel_stat
#define new_stat kernel_stat
#include <asm/stat.h> 
#undef new_stat
#undef stat
#define S_ISUID       04000   /* Set user ID on execution.  */
static inline _syscall2(int, _dl_stat, const char *, file_name, struct kernel_stat *, buf);


#define __NR__dl_munmap __NR_munmap
static inline _syscall2(int, _dl_munmap, void *, start, unsigned long, length);

#define __NR__dl_getuid __NR_getuid
static inline _syscall0(gid_t, _dl_getuid);

#define __NR__dl_geteuid __NR_geteuid
static inline _syscall0(uid_t, _dl_geteuid);

#define __NR__dl_getgid __NR_getgid
static inline _syscall0(gid_t, _dl_getgid);

#define __NR__dl_getegid __NR_getegid
static inline _syscall0(gid_t, _dl_getegid);

/*
 * Not an actual syscall, but we need something in assembly to say whether
 * this is OK or not.
 */
extern inline int _dl_suid_ok(void)
{
    uid_t uid, euid, gid, egid;

    uid = _dl_getuid();
    euid = _dl_geteuid();
    gid = _dl_getgid();
    egid = _dl_getegid();

    if(uid == euid && gid == egid)
	return 1;
    else
	return 0;
}

