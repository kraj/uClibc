/*
 * This file contains the system call macros and syscall
 * numbers used by the shared library loader. Taken from
 * Linux/CRIS 2.4.17 version kernel.
 */

#define __NR_exit                 1
#define __NR_read                 3
#define __NR_write                4
#define __NR_open                 5
#define __NR_close                6
#define __NR_getpid              20
#define __NR_getuid              24
#define __NR_getgid              47
#define __NR_geteuid             49
#define __NR_getegid             50
#define __NR_readlink            85
#define __NR_mmap                90
#define __NR_munmap              91
#define __NR_stat               106
#define __NR_mprotect           125

/* 
 * Here are the macros which define how this platform makes
 * system calls.  This particular variant does _not_ set 
 * errno since these will get called before the errno symbol 
 * is dynamicly linked. 
 */
#define _syscall0(type,name) \
type name(void) \
{ \
  register long __r10 __asm__ ("r10"); \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  register long __r11 __asm__ ("r11") = (long) arg2; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10), "r" (__r11) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  register long __r11 __asm__ ("r11") = (long) arg2; \
  register long __r12 __asm__ ("r12") = (long) arg3; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10), "r" (__r11), "r" (__r12) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  register long __r11 __asm__ ("r11") = (long) arg2; \
  register long __r12 __asm__ ("r12") = (long) arg3; \
  register long __r13 __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10), "r" (__r11), \
                          "r" (__r12), "r" (__r13) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
} 

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  register long __r11 __asm__ ("r11") = (long) arg2; \
  register long __r12 __asm__ ("r12") = (long) arg3; \
  register long __r13 __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("move %6,$mof\n\t" \
                        "movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10), "r" (__r11), \
                          "r" (__r12), "r" (__r13), "g" (arg5) \
                        : "r9"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
  register long __r10 __asm__ ("r10") = (long) arg1; \
  register long __r11 __asm__ ("r11") = (long) arg2; \
  register long __r12 __asm__ ("r12") = (long) arg3; \
  register long __r13 __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("move %6,$mof\n\tmove %7,$srp\n\t" \
                        "movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__r10) \
                        : "g" (__NR_##name), "0" (__r10), "r" (__r11), \
                          "r" (__r12), "r" (__r13), "g" (arg5), "g" (arg6)\
                        : "r9", "srp"); \
  if(__r10 >= 0) \
  	return (type) __r10; \
  return (type) -1; \
}
