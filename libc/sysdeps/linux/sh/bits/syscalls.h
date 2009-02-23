#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/* The Linux kernel uses different trap numbers on sh-2.  */
#if defined __sh2__ || defined __SH2A__
# define __SH_SYSCALL_TRAP_BASE 0x20
#else
# define __SH_SYSCALL_TRAP_BASE 0x10
#endif

#ifndef __ASSEMBLER__

#include <errno.h>

#define SYS_ify(syscall_name)  (__NR_##syscall_name)

/* user-visible error numbers are in the range -1 - -125: see <asm-sh/errno.h> */
#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-125)) { \
	/* Avoid using "res" which is declared to be in register r0; \
	   errno might expand to a function call and clobber it.  */ \
		int __err = -(res); \
		__set_errno(__err); \
		res = -1; \
	} \
	return (type) (res); \
} while (0)

/* XXX - _foo needs to be __foo, while __NR_bar could be _NR_bar. */
#define _syscall0(type,name) \
type name(void) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE), "0" (__sc0) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE + 1), "0" (__sc0), "r" (__sc4) \
	: "memory"); \
__syscall_return(type,__sc0); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
register long __sc0 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE + 2), "0" (__sc0), "r" (__sc4), \
          "r" (__sc5) \
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
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE + 3), "0" (__sc0), "r" (__sc4), \
          "r" (__sc5), "r" (__sc6) \
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
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE + 4), "0" (__sc0), "r" (__sc4), \
          "r" (__sc5), "r" (__sc6),  \
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
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL_TRAP_BASE + 5), "0" (__sc0), "r" (__sc4), \
          "r" (__sc5), "r" (__sc6), "r" (__sc7), "r" (__sc3) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

#ifndef __SH_SYSCALL6_TRAPA
#define __SH_SYSCALL6_TRAPA __SH_SYSCALL_TRAP_BASE + 6
#endif

/* Add in _syscall6 which is not in the kernel header */
#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
{ \
register long __sc3 __asm__ ("r3") = __NR_##name; \
register long __sc4 __asm__ ("r4") = (long) arg1; \
register long __sc5 __asm__ ("r5") = (long) arg2; \
register long __sc6 __asm__ ("r6") = (long) arg3; \
register long __sc7 __asm__ ("r7") = (long) arg4; \
register long __sc0 __asm__ ("r0") = (long) arg5; \
register long __sc1 __asm__ ("r1") = (long) arg6; \
__asm__ __volatile__ ("trapa	%1" \
	: "=z" (__sc0) \
	: "i" (__SH_SYSCALL6_TRAPA), "0" (__sc0), "r" (__sc4), \
          "r" (__sc5), "r" (__sc6), "r" (__sc7), "r" (__sc3), "r" (__sc1) \
	: "memory" ); \
__syscall_return(type,__sc0); \
}

#define SYSCALL_INST_STR(x)	"trapa #"__stringify(__SH_SYSCALL_TRAP_BASE + x)"\n\t"
#define SYSCALL_INST_STR0	SYSCALL_INST_STR(0)
#define SYSCALL_INST_STR1	SYSCALL_INST_STR(1)
#define SYSCALL_INST_STR2	SYSCALL_INST_STR(2)
#define SYSCALL_INST_STR3	SYSCALL_INST_STR(3)
#define SYSCALL_INST_STR4	SYSCALL_INST_STR(4)
#define SYSCALL_INST_STR5	SYSCALL_INST_STR(5)
#define SYSCALL_INST_STR6	SYSCALL_INST_STR(6)

# ifdef NEED_SYSCALL_INST_PAD
#  define SYSCALL_INST_PAD "\
	or r0,r0; or r0,r0; or r0,r0; or r0,r0; or r0,r0"
# else
#  define SYSCALL_INST_PAD
# endif

#define ASMFMT_0
#define ASMFMT_1 \
	, "r" (r4)
#define ASMFMT_2 \
	, "r" (r4), "r" (r5)
#define ASMFMT_3 \
	, "r" (r4), "r" (r5), "r" (r6)
#define ASMFMT_4 \
	, "r" (r4), "r" (r5), "r" (r6), "r" (r7)
#define ASMFMT_5 \
	, "r" (r4), "r" (r5), "r" (r6), "r" (r7), "0" (r0)
#define ASMFMT_6 \
	, "r" (r4), "r" (r5), "r" (r6), "r" (r7), "0" (r0), "r" (r1)
#define ASMFMT_7 \
	, "r" (r4), "r" (r5), "r" (r6), "r" (r7), "0" (r0), "r" (r1), "r" (r2)

#define SUBSTITUTE_ARGS_0()
#define SUBSTITUTE_ARGS_1(arg1) \
	long int _arg1 = (long int) (arg1);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1)
#define SUBSTITUTE_ARGS_2(arg1, arg2) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2)
#define SUBSTITUTE_ARGS_3(arg1, arg2, arg3) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	long int _arg3 = (long int) (arg3);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2);		      \
	register long int r6 __asm__ ("%r6") = (long int) (_arg3)
#define SUBSTITUTE_ARGS_4(arg1, arg2, arg3, arg4) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	long int _arg3 = (long int) (arg3);				      \
	long int _arg4 = (long int) (arg4);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2);		      \
	register long int r6 __asm__ ("%r6") = (long int) (_arg3);		      \
	register long int r7 __asm__ ("%r7") = (long int) (_arg4)
#define SUBSTITUTE_ARGS_5(arg1, arg2, arg3, arg4, arg5) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	long int _arg3 = (long int) (arg3);				      \
	long int _arg4 = (long int) (arg4);				      \
	long int _arg5 = (long int) (arg5);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2);		      \
	register long int r6 __asm__ ("%r6") = (long int) (_arg3);		      \
	register long int r7 __asm__ ("%r7") = (long int) (_arg4);		      \
	register long int r0 __asm__ ("%r0") = (long int) (_arg5)
#define SUBSTITUTE_ARGS_6(arg1, arg2, arg3, arg4, arg5, arg6) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	long int _arg3 = (long int) (arg3);				      \
	long int _arg4 = (long int) (arg4);				      \
	long int _arg5 = (long int) (arg5);				      \
	long int _arg6 = (long int) (arg6);				      \
	register long int r4 __asm__ ("%r4") = (long int)(_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2);		      \
	register long int r6 __asm__ ("%r6") = (long int) (_arg3);		      \
	register long int r7 __asm__ ("%r7") = (long int) (_arg4);		      \
	register long int r0 __asm__ ("%r0") = (long int) (_arg5);		      \
	register long int r1 __asm__ ("%r1") = (long int) (_arg6)
#define SUBSTITUTE_ARGS_7(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	long int _arg1 = (long int) (arg1);				      \
	long int _arg2 = (long int) (arg2);				      \
	long int _arg3 = (long int) (arg3);				      \
	long int _arg4 = (long int) (arg4);				      \
	long int _arg5 = (long int) (arg5);				      \
	long int _arg6 = (long int) (arg6);				      \
	long int _arg7 = (long int) (arg7);				      \
	register long int r4 __asm__ ("%r4") = (long int) (_arg1);		      \
	register long int r5 __asm__ ("%r5") = (long int) (_arg2);		      \
	register long int r6 __asm__ ("%r6") = (long int) (_arg3);		      \
	register long int r7 __asm__ ("%r7") = (long int) (_arg4);		      \
	register long int r0 __asm__ ("%r0") = (long int) (_arg5);		      \
	register long int r1 __asm__ ("%r1") = (long int) (_arg6);		      \
	register long int r2 __asm__ ("%r2") = (long int) (_arg7)

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) \
  ({                                                                          \
    unsigned int __resultvar = INTERNAL_SYSCALL (name, , nr, args);             \
    if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P (__resultvar, ), 0))         \
      {                                                                       \
        __set_errno (INTERNAL_SYSCALL_ERRNO (__resultvar, ));                   \
        __resultvar = 0xffffffff;                                               \
      }                                                                       \
    (int) __resultvar; })

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) \
  ({									      \
    unsigned long int resultvar;					      \
    register long int r3 __asm__ ("%r3") = SYS_ify (name);			      \
    SUBSTITUTE_ARGS_##nr(args);						      \
									      \
    __asm__ volatile (SYSCALL_INST_STR##nr SYSCALL_INST_PAD			      \
		  : "=z" (resultvar)					      \
		  : "r" (r3) ASMFMT_##nr				      \
		  : "memory");						      \
									      \
    (int) resultvar; })

/* The _NCS variant allows non-constant syscall numbers.  */
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) \
  ({									      \
    unsigned long int resultvar;					      \
    register long int r3 __asm__ ("%r3") = (name);			 	      \
    SUBSTITUTE_ARGS_##nr(args);						      \
									      \
    __asm__ volatile (SYSCALL_INST_STR##nr SYSCALL_INST_PAD			      \
		  : "=z" (resultvar)					      \
		  : "r" (r3) ASMFMT_##nr				      \
		  : "memory");						      \
									      \
    (int) resultvar; })

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned int) (val) >= 0xfffff001u)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)        (-(val))

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */

