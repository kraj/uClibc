#include <sys/types.h>

/*
 * This file contains the system call macros and syscall 
 * numbers used by the shared library loader.
 */

#define __NR_SYSCALL_BASE	0x900000

#define __NR_exit			(__NR_SYSCALL_BASE+  1)
#define __NR_read			(__NR_SYSCALL_BASE+  3)
#define __NR_write			(__NR_SYSCALL_BASE+  4)
#define __NR_open			(__NR_SYSCALL_BASE+  5)
#define __NR_close			(__NR_SYSCALL_BASE+  6)
#define __NR_getuid			(__NR_SYSCALL_BASE+ 24)
#define __NR_geteuid			(__NR_SYSCALL_BASE+ 49)
#define __NR_getgid			(__NR_SYSCALL_BASE+ 47)
#define __NR_getegid			(__NR_SYSCALL_BASE+ 50)
#define __NR_mmap			(__NR_SYSCALL_BASE+ 90)
#define __NR_munmap			(__NR_SYSCALL_BASE+ 91)
#define __NR_stat			(__NR_SYSCALL_BASE+106)
#define __NR_mprotect			(__NR_SYSCALL_BASE+125)


/* Here are the macros which define how this platform makes
 * system calls.  This particular variant does _not_ set 
 * errno (note how it is disabled in __syscall_return) since
 * these will get called before the errno symbol is dynamicly 
 * linked. */

/* These are Erik's versions of the syscall routines.  His were
 * cleaner than mine, so I adopted them instead with some
 * reformating.  Shane Nay.
 */

#define __sys2(x) #x
#define __sys1(x) __sys2(x)

#ifndef __syscall
#define __syscall(name) "swi\t" __sys1(__NR_##name) "\n\t"
#endif

#undef __syscall_return
#define __syscall_return(type, res)					\
do {									\
	if ((unsigned long)(res) >= (unsigned long)(-125)) {		\
		/*errno = -(res);*/					\
		res = -1;						\
	}								\
	return (type) (res);						\
} while (0)

#define _syscall0(type,name)						\
type name(void) {							\
  long __res;								\
  __asm__ __volatile__ (						\
  __syscall(name)							\
  "mov %0,r0"								\
  :"=r" (__res) : : "r0","lr");						\
  __syscall_return(type,__res);						\
}

#define _syscall1(type,name,type1,arg1)					\
type name(type1 arg1) {							\
  long __res;								\
  __asm__ __volatile__ (						\
  "mov\tr0,%1\n\t"							\
  __syscall(name)							\
  "mov %0,r0"								\
        : "=r" (__res)							\
        : "r" ((long)(arg1))						\
	: "r0","lr");							\
  __syscall_return(type,__res);						\
}

#define _syscall2(type,name,type1,arg1,type2,arg2)			\
type name(type1 arg1,type2 arg2) {					\
  long __res;								\
  __asm__ __volatile__ (						\
  "mov\tr0,%1\n\t"							\
  "mov\tr1,%2\n\t"							\
  __syscall(name)							\
  "mov\t%0,r0"								\
        : "=r" (__res)							\
        : "r" ((long)(arg1)),"r" ((long)(arg2))				\
	: "r0","r1","lr");						\
  __syscall_return(type,__res);						\
}


#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)		\
type name(type1 arg1,type2 arg2,type3 arg3) {				\
  long __res;								\
  __asm__ __volatile__ (						\
  "mov\tr0,%1\n\t"							\
  "mov\tr1,%2\n\t"							\
  "mov\tr2,%3\n\t"							\
  __syscall(name)							\
  "mov\t%0,r0"								\
        : "=r" (__res)							\
        : "r" ((long)(arg1)),"r" ((long)(arg2)),"r" ((long)(arg3))	\
        : "r0","r1","r2","lr");						\
  __syscall_return(type,__res);						\
}

#undef _syscall4
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) {		\
  long __res;								\
  __asm__ __volatile__ (						\
  "mov\tr0,%1\n\t"							\
  "mov\tr1,%2\n\t"							\
  "mov\tr2,%3\n\t"							\
  "mov\tr3,%4\n\t"							\
  __syscall(name)							\
  "mov\t%0,r0"								\
  	: "=r" (__res)							\
  	: "r" ((long)(arg1)),"r" ((long)(arg2)),                        \
	  "r" ((long)(arg3)),"r" ((long)(arg4))	                        \
  	: "r0","r1","r2","r3","lr");					\
  __syscall_return(type,__res);						\
}
  

