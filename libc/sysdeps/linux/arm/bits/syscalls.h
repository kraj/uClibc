#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/*
   Some of the sneaky macros in the code were taken from
   glibc-2.3.2/sysdeps/unix/sysv/linux/arm/sysdep.h
*/

#ifdef __ASSEMBLER__

/* Call a given syscall, with arguments loaded.  For EABI, we must
   save and restore r7 for the syscall number.  Unlike the DO_CALL
   macro in glibc, this macro does not load syscall arguments.  */
#undef DO_CALL
#if defined(__ARM_EABI__)
#define DO_CALL(syscall_name)			\
    mov ip, r7;					\
    ldr r7, =SYS_ify (syscall_name);		\
    swi 0x0;					\
    mov r7, ip;
#else
#define DO_CALL(syscall_name)			\
    swi SYS_ify (syscall_name);
#endif

#else

#include <errno.h>

#define INLINE_SYSCALL_NCS(name, nr, args...)				\
  ({ unsigned int _inline_sys_result = INTERNAL_SYSCALL_NCS (name, , nr, args);	\
     if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P (_inline_sys_result, ), 0))	\
       {								\
	 __set_errno (INTERNAL_SYSCALL_ERRNO (_inline_sys_result, ));		\
	 _inline_sys_result = (unsigned int) -1;				\
       }								\
     (int) _inline_sys_result; })

#if !defined(__thumb__)
#if defined(__ARM_EABI__)
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({unsigned int __sys_result;						\
     {									\
       register int _a1 __asm__ ("r0"), _nr __asm__ ("r7");		\
       LOAD_ARGS_##nr (args)						\
       _nr = (name);							\
       __asm__ __volatile__ ("swi	0x0	@ syscall " #name	\
			     : "=r" (_a1)				\
			     : "r" (_nr) ASM_ARGS_##nr			\
			     : "memory");				\
	       __sys_result = _a1;					\
     }									\
     (int) __sys_result; })
#else /* defined(__ARM_EABI__) */

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({ unsigned int __sys_result;						\
     {									\
       register int _a1 __asm__ ("a1");					\
       LOAD_ARGS_##nr (args)						\
       __asm__ __volatile__ ("swi	%1	@ syscall " #name	\
		     : "=r" (_a1)					\
		     : "i" (name) ASM_ARGS_##nr				\
		     : "memory");					\
       __sys_result = _a1;						\
     }									\
     (int) __sys_result; })
#endif
#else /* !defined(__thumb__) */
/* We can't use push/pop inside the asm because that breaks
   unwinding (ie. thread cancellation).
 */
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({ unsigned int __sys_result;						\
    {									\
      int _sys_buf[2];							\
      register int _a1 __asm__ ("a1");					\
      register int *_v3 __asm__ ("v3") = _sys_buf;			\
      *_v3 = (int) (name);						\
      LOAD_ARGS_##nr (args)						\
      __asm__ __volatile__ ("str	r7, [v3, #4]\n"			\
		    "\tldr	r7, [v3]\n"				\
		    "\tswi	0	@ syscall " #name "\n"		\
		    "\tldr	r7, [v3, #4]"				\
		    : "=r" (_a1)					\
		    : "r" (_v3) ASM_ARGS_##nr				\
                    : "memory");					\
	__sys_result = _a1;						\
    }									\
    (int) __sys_result; })
#endif /*!defined(__thumb__)*/

#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((unsigned int) (val) >= 0xfffff001u)

#define LOAD_ARGS_0()
#define ASM_ARGS_0
#define LOAD_ARGS_1(a1)				\
  int _a1tmp = (int) (a1);			\
  LOAD_ARGS_0 ()				\
  _a1 = _a1tmp;
#define ASM_ARGS_1	ASM_ARGS_0, "r" (_a1)
#define LOAD_ARGS_2(a1, a2)			\
  int _a2tmp = (int) (a2);			\
  LOAD_ARGS_1 (a1)				\
  register int _a2 __asm__ ("a2") = _a2tmp;
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_a2)
#define LOAD_ARGS_3(a1, a2, a3)			\
  int _a3tmp = (int) (a3);			\
  LOAD_ARGS_2 (a1, a2)				\
  register int _a3 __asm__ ("a3") = _a3tmp;
#define ASM_ARGS_3	ASM_ARGS_2, "r" (_a3)
#define LOAD_ARGS_4(a1, a2, a3, a4)		\
  int _a4tmp = (int) (a4);			\
  LOAD_ARGS_3 (a1, a2, a3)			\
  register int _a4 __asm__ ("a4") = _a4tmp;
#define ASM_ARGS_4	ASM_ARGS_3, "r" (_a4)
#define LOAD_ARGS_5(a1, a2, a3, a4, a5)		\
  int _v1tmp = (int) (a5);			\
  LOAD_ARGS_4 (a1, a2, a3, a4)			\
  register int _v1 __asm__ ("v1") = _v1tmp;
#define ASM_ARGS_5	ASM_ARGS_4, "r" (_v1)
#define LOAD_ARGS_6(a1, a2, a3, a4, a5, a6)	\
  int _v2tmp = (int) (a6);			\
  LOAD_ARGS_5 (a1, a2, a3, a4, a5)		\
  register int _v2 __asm__ ("v2") = _v2tmp;
#define ASM_ARGS_6	ASM_ARGS_5, "r" (_v2)
#define LOAD_ARGS_7(a1, a2, a3, a4, a5, a6, a7)	\
  int _v3tmp = (int) (a7);			\
  LOAD_ARGS_6 (a1, a2, a3, a4, a5, a6)		\
  register int _v3 __asm__ ("v3") = _v3tmp;
#define ASM_ARGS_7	ASM_ARGS_6, "r" (_v3)


#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
