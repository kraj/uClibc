/* This file is based on the asm/unistd.h kernel header file.
 * Including asm/unistd.h is hereby officially forbidden.  
 * Don't do it.  It is bad for you.  */ 

#include <features.h>

/* FIXME: perhaps sh needs separate PIC syscalls? */ 


#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-125)) { \
	/* Avoid using "res" which is declared to be in register r0; \
	   errno might expand to a function call and clobber it.  */ \
		int __err = -(res); \
		errno = __err; \
		res = -1; \
	} \
	return (type) (res); \
} while (0)

/* XXX - _foo needs to be __foo, while __NR_bar could be _NR_bar. */
#define _syscall0(type,name) \
type name(void) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
__asm__ __volatile__ ("trapa	#0x10" \
	: "=z" (__sc0) \
	: "0" (__sc0) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
__asm__ __volatile__ ("trapa	#0x11" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4) \
	: "memory"); \
__syscall_return(type,__sc0); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
__asm__ __volatile__ ("trapa	#0x12" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4), "r" (__sc5) \
	: "memory"); \
__syscall_return(type,__sc0); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
register long __sc6 __asm__ ("r6") = (long) arg3; \
__asm__ __volatile__ ("trapa	#0x13" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4), "r" (__sc5), "r" (__sc6) \
	: "memory"); \
__syscall_return(type,__sc0); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
register long __sc6 __asm__ ("r6") = (long) arg3; \
register long __sc7 __asm__ ("r7") = (long) arg4; \
__asm__ __volatile__ ("trapa	#0x14" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4), "r" (__sc5), "r" (__sc6),  \
	  "r" (__sc7) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
{ \
register long __sc3 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
register long __sc6 __asm__ ("r6") = (long) arg3; \
register long __sc7 __asm__ ("r7") = (long) arg4; \
register long __sc0 __asm__ ("r0") = (long) arg5; \
__asm__ __volatile__ ("trapa	#0x15" \
	: "=z" (__sc0) \
	: "0" (__sc0), "r" (__sc4), "r" (__sc5), "r" (__sc6), "r" (__sc7),  \
	  "r" (__sc3) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}


