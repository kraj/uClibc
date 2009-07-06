#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#include <bits/wordsize.h>

#ifndef __ASSEMBLER__

#include <errno.h>

#define SYS_ify(syscall_name)  (__NR_##syscall_name)

#undef __SYSCALL_STRING
#if __WORDSIZE == 32
# define __SYSCALL_STRING \
	"t 0x10\n\t" \
	"bcc 1f\n\t" \
	"mov %%o0, %0\n\t" \
	"sub %%g0, %%o0, %0\n\t" \
	"1:\n\t"
# define __SYSCALL_RES_CHECK (__res < -255 || __res >= 0)
#elif __WORDSIZE == 64
# define __SYSCALL_STRING \
	"t 0x6d\n\t" \
	"sub %%g0, %%o0, %0\n\t" \
	"movcc %%xcc, %%o0, %0\n\t"
# define __SYSCALL_RES_CHECK (__res >= 0)
#else
# error unknown __WORDSIZE
#endif

#define __SYSCALL_CLOBBERS "cc", "memory"

#define __SYSCALL_RETURN(type) \
	if (__SYSCALL_RES_CHECK) \
		return (type) __res; \
	__set_errno (-__res); \
	return (type) -1;

#undef _syscall0
#define _syscall0(type,name) \
type name(void) \
{ \
    return (type)(INLINE_SYSCALL(name,0)); \
}

#undef _syscall1
#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
    return (type)(INLINE_SYSCALL(name,1,arg1)); \
}

#undef _syscall2
#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
    return (type)(INLINE_SYSCALL(name,2,arg1,arg2)); \
}

#undef _syscall3
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
    return (type)(INLINE_SYSCALL(name,3,arg1,arg2,arg3)); \
}

#undef _syscall4
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
    return (type)(INLINE_SYSCALL(name,4,arg1,arg2,arg3,arg4)); \
}

#undef _syscall5
#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
    return (type)(INLINE_SYSCALL(name,5,arg1,arg2,arg3,arg4,arg5)); \
}

#undef _syscall6
#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
	  type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5, type6 arg6) \
{ \
    return (type)(INLINE_SYSCALL(name,6,arg1,arg2,arg3,arg4,arg5,arg6)); \
}

#ifndef NOT_IN_libc
#define DEBUG_SYSCALL(name) { \
      char d[64];\
      write( 2, d, snprintf( d, 64, "syscall %d error %d\n", __NR_##name, _inline_sys_result)); \
}
#else
#define DEBUG_SYSCALL(name) do{} while(0)
#endif

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
  ({ unsigned int _inline_sys_result = INTERNAL_SYSCALL (name, , nr, args);	\
     if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P (_inline_sys_result, ), 0))	\
       {								\
	 __set_errno (INTERNAL_SYSCALL_ERRNO (_inline_sys_result, ));		\
	 _inline_sys_result = (unsigned int) -1;				\
       }								\
     (int) _inline_sys_result; })

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)


#define INTERNAL_SYSCALL( name, err, nr, args...) \
    INTERNAL_SYSCALL_NCS( __NR_##name, err, nr, args )


#define INTERNAL_SYSCALL_NCS(sys_num, err, nr, args...) \
    ({                                                      \
        unsigned int __res;                                 \
        {                                                   \
            register long __o0 __asm__("o0");               \
            register long __g1 __asm__("g1") = sys_num;     \
            LOAD_ARGS_##nr(args)                            \
            __asm__ __volatile__( __SYSCALL_STRING          \
                : "=r" (__res), "=&r" (__o0)                \
                : "1" (__o0) ASM_ARGS_##nr, "r" (__g1)     \
                : __SYSCALL_CLOBBERS );                                   \
        }                                                   \
        (int)__res;                                         \
     })

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned int) (val) >= 0xfffff001u)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

# define CALL_ERRNO_LOCATION "call   __errno_location;"
#define __CLONE_SYSCALL_STRING						\
	"ta	0x10;"							\
	"bcs	2f;"							\
	" sub	%%o1, 1, %%o1;"						\
	"and	%%o0, %%o1, %%o0;"					\
	"1:"								\
	".subsection 2;"						\
	"2:"								\
	"save	%%sp, -192, %%sp;"					\
	CALL_ERRNO_LOCATION						\
	" nop;"								\
	"st	%%i0, [%%o0];"						\
	"ba	1b;"							\
	" restore %%g0, -1, %%o0;"					\
	".previous;"

#define INLINE_CLONE_SYSCALL(arg1,arg2,arg3,arg4,arg5)			\
({									\
	register long __o0 __asm__ ("o0") = (long)(arg1);		\
	register long __o1 __asm__ ("o1") = (long)(arg2);		\
	register long __o2 __asm__ ("o2") = (long)(arg3);		\
	register long __o3 __asm__ ("o3") = (long)(arg4);		\
	register long __o4 __asm__ ("o4") = (long)(arg5);		\
	register long __g1 __asm__ ("g1") = __NR_clone;			\
	__asm __volatile (__CLONE_SYSCALL_STRING :			\
			  "=r" (__g1), "=r" (__o0), "=r" (__o1)	:	\
			  "0" (__g1), "1" (__o0), "2" (__o1),		\
			  "r" (__o2), "r" (__o3), "r" (__o4) :		\
			  __SYSCALL_CLOBBERS);				\
	__o0;								\
})

#define LOAD_ARGS_0()
#define ASM_ARGS_0
#define LOAD_ARGS_1(o0) \
    __o0 = (int)o0;     \
    LOAD_ARGS_0()
#define ASM_ARGS_1 ASM_ARGS_0, "r" (__o0)
#define LOAD_ARGS_2(o0, o1)			\
  register int __o1 __asm__ ("o1") = (int) (o1);	\
  LOAD_ARGS_1 (o0)
#define ASM_ARGS_2	ASM_ARGS_1, "r" (__o1)
#define LOAD_ARGS_3(o0, o1, o2)			\
  register int __o2 __asm__ ("o2") = (int) (o2);	\
  LOAD_ARGS_2 (o0, o1)
#define ASM_ARGS_3	ASM_ARGS_2, "r" (__o2)
#define LOAD_ARGS_4(o0, o1, o2, o3)		\
  register int __o3 __asm__ ("o3") = (int) (o3);	\
  LOAD_ARGS_3 (o0, o1, o2)
#define ASM_ARGS_4	ASM_ARGS_3, "r" (__o3)
#define LOAD_ARGS_5(o0, o1, o2, o3, o4)		\
  register int __o4 __asm__ ("o4") = (int) (o4);	\
  LOAD_ARGS_4 (o0, o1, o2, o3)
#define ASM_ARGS_5	ASM_ARGS_4, "r" (__o4)
#define LOAD_ARGS_6(o0, o1, o2, o3, o4, o5)	\
  register int __o5 __asm__ ("o5") = (int) (o5);	\
  LOAD_ARGS_5 (o0, o1, o2, o3, o4)
#define ASM_ARGS_6	ASM_ARGS_5, "r" (__o5)


#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
