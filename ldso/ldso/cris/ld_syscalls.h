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
  register long __a __asm__ ("r10"); \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a), "r" (__b) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a), "r" (__b), "r" (__c) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a), "r" (__b), \
                          "r" (__c), "r" (__d) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
} 

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("move %6,$mof\n\t" \
                        "movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a), "r" (__b), \
                          "r" (__c), "r" (__d), "g" (arg5) \
                        : "r10", "r9"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
  register long __a __asm__ ("r10") = (long) arg1; \
  register long __b __asm__ ("r11") = (long) arg2; \
  register long __c __asm__ ("r12") = (long) arg3; \
  register long __d __asm__ ("r13") = (long) arg4; \
  __asm__ __volatile__ ("move %6,$mof\n\tmove %7,$srp\n\t" \
                        "movu.w %1,$r9\n\tbreak 13" \
                        : "=r" (__a) \
                        : "g" (__NR_##name), "0" (__a), "r" (__b), \
                          "r" (__c), "r" (__d), "g" (arg5), "g" (arg6)\
                        : "r10", "r9", "srp"); \
  if(__a >= 0) \
  	return (type) __a; \
  return (type) -1; \
}
