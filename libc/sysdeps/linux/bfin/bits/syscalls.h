#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#include <features.h>

/* This includes the `__NR_<name>' syscall numbers taken from the Linux kernel
 * header files.  It also defines the traditional `SYS_<name>' macros for older
 * programs.  */
#include <bits/sysnum.h>

/* This code is mostly cut & paste from the uClinux bfin port */

#ifndef __ASSEMBLER__

#define __syscall_return(type, res)					\
do {									\
	if ((unsigned long)(res) >= (unsigned long)(-125)) 		\
	{	__set_errno(-(res));					\
		res = -1;						\
	}								\
	return (type) (res);						\
} while (0)

#define _syscall0(type,name)						\
type name(void) {							\
  long __res;								\
  __asm__ __volatile__ (						\
  "p0 = %1;\n\t"							\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
  : "=da" (__res) 							\
  : "i" (__NR_##name)							\
  : "CC", "P0");							\
__syscall_return(type,__res);						\
}

#define _syscall1(type,name,type1,arg1)					\
type name(type1 arg1) {							\
  long __res;								\
  __asm__ __volatile__ (						\
  "r0=%2;\n\t"								\
  "p0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
        : "=da" (__res)							\
        : "i" (__NR_##name),						\
	  "a" ((long)(arg1))						\
	: "CC", "R0", "P0");						\
__syscall_return(type,__res);						\
}

#define _syscall2(type,name,type1,arg1,type2,arg2)			\
type name(type1 arg1,type2 arg2) {					\
  long __res;								\
  __asm__ __volatile__ (						\
  "r1=%3;\n\t"								\
  "r0=%2;\n\t"								\
  "p0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
        : "=da" (__res)							\
        : "i" (__NR_##name),						\
	  "a" ((long)(arg1)),						\
	  "a" ((long)(arg2))						\
	: "CC", "R0","R1", "P0");					\
__syscall_return(type,__res);						\
}


#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)		\
type name(type1 arg1,type2 arg2,type3 arg3) {				\
  long __res;								\
  __asm__ __volatile__ (						\
  "r2=%4;\n\t"								\
  "r1=%3;\n\t"								\
  "r0=%2;\n\t"								\
  "p0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
        : "=da" (__res)							\
        : "i"   (__NR_##name),						\
	  "a"   ((long)(arg1)),						\
	  "a"   ((long)(arg2)),						\
	  "a"   ((long)(arg3))						\
        : "CC", "R0","R1","R2", "P0");					\
__syscall_return(type,__res);						\
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) {		\
  long __res;								\
  __asm__ __volatile__ (						\
  "[--sp] = r3;\n\t"							\
  "r3=%5;\n\t"								\
  "r2=%4;\n\t"								\
  "r1=%3;\n\t"								\
  "r0=%2;\n\t"								\
  "p0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
  "r3 = [sp++];\n\t"							\
  	: "=da" (__res)							\
  	: "i"  (__NR_##name),						\
	  "a"  ((long)(arg1)),						\
	  "a"  ((long)(arg2)),						\
	  "a"  ((long)(arg3)),						\
	  "a"  ((long)(arg4))						\
  	: "CC", "R0","R1","R2","R3", "P0");				\
__syscall_return(type,__res);						\
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)	\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) {	\
  long __res;								\
  __asm__ __volatile__ (						\
  "[--sp] = r4;\n\t"                                                    \
  "[--sp] = r3;\n\t"                                                    \
  "r4=%6;\n\t"								\
  "r3=%5;\n\t"								\
  "r2=%4;\n\t"								\
  "r1=%3;\n\t"								\
  "r0=%2;\n\t"								\
  "P0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
  "r3 = [sp++];\n\t" 							\
  "r4 = [sp++];\n\t"                                                    \
  	: "=da" (__res)							\
  	: "i"  (__NR_##name),						\
	  "rm"  ((long)(arg1)),						\
	  "rm"  ((long)(arg2)),						\
	  "rm"  ((long)(arg3)),						\
	  "rm"  ((long)(arg4)),						\
	  "rm"  ((long)(arg5))						\
	: "CC","R0","R1","R2","R3","R4","P0");				\
__syscall_return(type,__res);						\
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)	\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) {	\
  long __res;								\
  __asm__ __volatile__ (						\
  "[--sp] = r5;\n\t"                                                    \
  "[--sp] = r4;\n\t"                                                    \
  "[--sp] = r3;\n\t"                                                    \
  "r4=%6;\n\t"								\
  "r3=%5;\n\t"								\
  "r2=%4;\n\t"								\
  "r1=%3;\n\t"								\
  "r0=%2;\n\t"								\
  "P0=%1;\n\t"								\
  "excpt 0;\n\t" 							\
  "%0=r0;\n\t"								\
  "r3 = [sp++];\n\t" 							\
  "r4 = [sp++];\n\t"                                                    \
  "r5 = [sp++];\n\t"                                                    \
  	: "=da" (__res)							\
  	: "i"  (__NR_##name),						\
	  "rm"  ((long)(arg1)),						\
	  "rm"  ((long)(arg2)),						\
	  "rm"  ((long)(arg3)),						\
	  "rm"  ((long)(arg4)),						\
	  "rm"  ((long)(arg5)),						\
	  "rm"  ((long)(arg6))						\
	: "CC","R0","R1","R2","R3","R4","R5","P0");				\
__syscall_return(type,__res);						\
}

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
