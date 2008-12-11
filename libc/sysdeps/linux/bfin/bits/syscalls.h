#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#ifndef __ASSEMBLER__

#include <errno.h>

#define SYS_ify(syscall_name)  (__NR_##syscall_name)

/* user-visible error numbers are in the range -1 - -4095: see <asm-frv/errno.h> */
#if defined _LIBC && !defined __set_errno
# define __syscall_return(type, res) \
do { \
        unsigned long __sr2 = (res);		    			    \
	if (__builtin_expect ((unsigned long)(__sr2)			    \
			      >= (unsigned long)(-4095), 0)) {		    \
		extern int __syscall_error (int);			    \
		return (type) __syscall_error (__sr2);		    	    \
	}								    \
	return (type) (__sr2); 						    \
} while (0)
#else
# define __syscall_return(type, res) \
do { \
        unsigned long __sr2 = (res);		    			    \
	if (__builtin_expect ((unsigned long)(__sr2)			    \
			      >= (unsigned long)(-4095), 0)) {		    \
		__set_errno (-__sr2);				    	    \
		__sr2 = -1; 						    \
	}								    \
	return (type) (__sr2); 						    \
} while (0)
#endif

#define _syscall0(type,name)						\
type name(void) {							\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA" (__NR_##name)					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall1(type,name,type1,arg1)					\
type name(type1 arg1) {							\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA" (__NR_##name),					\
		  "q0" ((long)(arg1))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall2(type,name,type1,arg1,type2,arg2)			\
type name(type1 arg1,type2 arg2) {					\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		"%0=r0;\n\t"						\
		: "=q0" (__res)						\
		: "qA" (__NR_##name),					\
		  "q0" ((long)(arg1)),					\
		  "q1" ((long)(arg2))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)		\
type name(type1 arg1,type2 arg2,type3 arg3) {				\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA"   (__NR_##name),					\
		  "q0"   ((long)(arg1)),				\
		  "q1"   ((long)(arg2)),				\
		  "q2"   ((long)(arg3))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) {		\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA"  (__NR_##name),					\
		  "q0"  ((long)(arg1)),					\
		  "q1"  ((long)(arg2)),					\
		  "q2"  ((long)(arg3)),					\
		  "q3"  ((long)(arg4))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)	\
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) {	\
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA"  (__NR_##name),					\
		  "q0"  ((long)(arg1)),					\
		  "q1"  ((long)(arg2)),					\
		  "q2"  ((long)(arg3)),					\
		  "q3"  ((long)(arg4)),					\
		  "q4"  ((long)(arg5))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) { \
	long __res;							\
	__asm__ __volatile__ (						\
		"excpt 0;\n\t"						\
		: "=q0" (__res)						\
		: "qA"  (__NR_##name),					\
		  "q0"  ((long)(arg1)),					\
		  "q1"  ((long)(arg2)),					\
		  "q2"  ((long)(arg3)),					\
		  "q3"  ((long)(arg4)),					\
		  "q4"  ((long)(arg5)),					\
		  "q5"  ((long)(arg6))					\
		: "memory","CC");					\
	__syscall_return(type,__res);					\
}


/* Define a macro which expands into the inline wrapper code for a system call */
#define INLINE_SYSCALL(name, nr, args...)				\
({									\
	INTERNAL_SYSCALL_DECL(err);					\
	long result_var = INTERNAL_SYSCALL(name, err, nr, args);	\
	if (INTERNAL_SYSCALL_ERROR_P(result_var, err)) {		\
		__set_errno(INTERNAL_SYSCALL_ERRNO(result_var, err));	\
		result_var = -1L;					\
	}								\
	result_var;							\
})

#define INTERNAL_SYSCALL_DECL(err)         do { } while (0)
#define INTERNAL_SYSCALL_ERROR_P(val, err) ((unsigned long)val >= (unsigned long)(-4095))
#define INTERNAL_SYSCALL_ERRNO(val, err)   (-(val))

#define INTERNAL_SYSCALL(name, err, nr, args...)	\
({							\
	long __res;					\
	__asm__ __volatile__ (				\
		"excpt 0;\n\t"				\
		: "=q0" (__res)				\
		: "qA"  (__NR_##name) ASMFMT_##nr(args)	\
		: "memory","CC");			\
	__res;						\
})

#define ASMFMT_0()

#define ASMFMT_1(arg1) \
	, "q0" ((long)(arg1))

#define ASMFMT_2(arg1, arg2) \
	ASMFMT_1(arg1) \
	, "q1" ((long)(arg2))

#define ASMFMT_3(arg1, arg2, arg3) \
	ASMFMT_2(arg1, arg2) \
	, "q2" ((long)(arg3))

#define ASMFMT_4(arg1, arg2, arg3, arg4) \
	ASMFMT_3(arg1, arg2, arg3) \
	, "q3" ((long)(arg4))

#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	ASMFMT_4(arg1, arg2, arg3, arg4) \
	, "q4" ((long)(arg5))

#define ASMFMT_6(arg1, arg2, arg3, arg4, arg5, arg6) \
	ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "q5" ((long)(arg6))

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
