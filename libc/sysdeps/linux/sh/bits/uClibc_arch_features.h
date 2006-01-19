/*
 * Track misc arch-specific features that aren't config options
 */

#ifndef _BITS_UCLIBC_ARCH_FEATURES_H
#define _BITS_UCLIBC_ARCH_FEATURES_H

/* instruction used when calling abort() to kill yourself */
#if defined(__SH5__)
# define __UCLIBC_ABORT_INSTRUCTION__ "movi 0x10, r9; shori 0xff, r9; trapa r9"
#elif defined(__sh2__)
# define __UCLIBC_ABORT_INSTRUCTION__ "trapa #32"
#else /* defined(__sh__) */
# define __UCLIBC_ABORT_INSTRUCTION__ "trapa #0xff"
#endif

/* can your target use syscall6() for mmap ? */
#define __UCLIBC_MMAP_HAS_6_ARGS__

/* does your target use syscall4() for truncate64 ? (32bit arches only) */
#undef __UCLIBC_TRUNCATE64_HAS_4_ARGS__

/* does your target have a broken create_module() ? */
#undef __UCLIBC_BROKEN_CREATE_MODULE__

/* does your target prefix all symbols with an _ ? */
#define __UCLIBC_NO_UNDERSCORES__

/* does your target have an asm .set ? */
#define __UCLIBC_HAVE_ASM_SET_DIRECTIVE__

/* shouldn't need to change these ... */
#define __UCLIBC_ASM_GLOBAL_DIRECTIVE__
#define __UCLIBC_ASM_TYPE_DIRECTIVE_PREFIX__
#define __UCLIBC_HAVE_ASM_WEAK_DIRECTIVE__
#define __UCLIBC_HAVE_ASM_WEAKEXT_DIRECTIVE__
#define __UCLIBC_HAVE_ASM_GLOBAL_DOT_NAME__

#endif /* _BITS_UCLIBC_ARCH_FEATURES_H */
