/* Unlike the asm/unistd.h kernel header file (which this is partly based on),
 * this file must be able to cope with PIC and non-PIC code.  For some arches
 * there is no difference.  For x86 (which has far too few registers) there is
 * a difference.   Regardless, including asm/unistd.h is hereby officially
 * forbidden.  Don't do it.  It is bad for you.  */ 

#include <features.h>

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/*
 * System calls under the Sparc.
 *
 * Don't be scared by the ugly clobbers, it is the only way I can
 * think of right now to force the arguments into fixed registers
 * before the trap into the system call with gcc 'asm' statements.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *
 * SunOS compatibility based upon preliminary work which is:
 *
 * Copyright (C) 1995 Adrian M. Rodriguez (adrian@remus.rutgers.edu)
 */

#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res)\
		      : "r" (__g1) \
		      : "o0", "cc"); \
if (__res < -255 || __res >= 0) \
    return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
register long __o0 __asm__ ("o0") = (long)(arg1); \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res), "=&r" (__o0) \
		      : "1" (__o0), "r" (__g1) \
		      : "cc"); \
if (__res < -255 || __res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
register long __o0 __asm__ ("o0") = (long)(arg1); \
register long __o1 __asm__ ("o1") = (long)(arg2); \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res), "=&r" (__o0) \
		      : "1" (__o0), "r" (__o1), "r" (__g1) \
		      : "cc"); \
if (__res < -255 || __res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
register long __o0 __asm__ ("o0") = (long)(arg1); \
register long __o1 __asm__ ("o1") = (long)(arg2); \
register long __o2 __asm__ ("o2") = (long)(arg3); \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res), "=&r" (__o0) \
		      : "1" (__o0), "r" (__o1), "r" (__o2), "r" (__g1) \
		      : "cc"); \
if (__res < -255 || __res>=0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
register long __o0 __asm__ ("o0") = (long)(arg1); \
register long __o1 __asm__ ("o1") = (long)(arg2); \
register long __o2 __asm__ ("o2") = (long)(arg3); \
register long __o3 __asm__ ("o3") = (long)(arg4); \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res), "=&r" (__o0) \
		      : "1" (__o0), "r" (__o1), "r" (__o2), "r" (__o3), "r" (__g1) \
		      : "cc"); \
if (__res < -255 || __res>=0) \
	return (type) __res; \
errno = -__res; \
return -1; \
} 

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
long __res; \
register long __g1 __asm__ ("g1") = __NR_##name; \
register long __o0 __asm__ ("o0") = (long)(arg1); \
register long __o1 __asm__ ("o1") = (long)(arg2); \
register long __o2 __asm__ ("o2") = (long)(arg3); \
register long __o3 __asm__ ("o3") = (long)(arg4); \
register long __o4 __asm__ ("o4") = (long)(arg5); \
__asm__ __volatile__ ("t 0x10\n\t" \
		      "bcc 1f\n\t" \
		      "mov %%o0, %0\n\t" \
		      "sub %%g0, %%o0, %0\n\t" \
		      "1:\n\t" \
		      : "=r" (__res), "=&r" (__o0) \
		      : "1" (__o0), "r" (__o1), "r" (__o2), "r" (__o3), "r" (__o4), "r" (__g1) \
		      : "cc"); \
if (__res < -255 || __res>=0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}

#endif /* _SYSCALLS_H_ */
