#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscall.h> directly; include <sys/syscall.h> instead."
#endif

#include <features.h>

/* Do something very evil for now.  Until we create our own syscall
 * macros, short circuit bits/syscall.h  and use asm/unistd.h instead */
#include <asm/unistd.h>

/* This includes the `__NR_<name>' syscall numbers taken from the Linux kernel
 * header files.  It also defines the traditional `SYS_<name>' macros for older
 * programs.  */
#include <bits/syscall.h>

/* The kernel includes don't provide _syscall6, so provide our own */
#undef _syscall6
#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6)	\
{									\
	unsigned long __sc_ret, __sc_err;				\
	{								\
		register unsigned long __sc_0 __asm__ ("r0");		\
		register unsigned long __sc_3 __asm__ ("r3");		\
		register unsigned long __sc_4 __asm__ ("r4");		\
		register unsigned long __sc_5 __asm__ ("r5");		\
		register unsigned long __sc_6 __asm__ ("r6");		\
		register unsigned long __sc_7 __asm__ ("r7");		\
		register unsigned long __sc_8 __asm__ ("r8");		\
									\
		__sc_3 = (unsigned long) (arg1);			\
		__sc_4 = (unsigned long) (arg2);			\
		__sc_5 = (unsigned long) (arg3);			\
		__sc_6 = (unsigned long) (arg4);			\
		__sc_7 = (unsigned long) (arg5);			\
		__sc_8 = (unsigned long) (arg6);			\
		__sc_0 = __NR_##name;					\
		__asm__ __volatile__					\
			("sc           \n\t"				\
			 "mfcr %1      "				\
			: "=&r" (__sc_3), "=&r" (__sc_0)		\
			: "0"   (__sc_3), "1"   (__sc_0),		\
			  "r"   (__sc_4),				\
			  "r"   (__sc_5),				\
			  "r"   (__sc_6),				\
			  "r"   (__sc_7),				\
			  "r"   (__sc_8)				\
			: __syscall_clobbers);				\
		__sc_ret = __sc_3;					\
		__sc_err = __sc_0;					\
	}								\
	__syscall_return (type);					\
}



#endif /* _BITS_SYSCALLS_H */

