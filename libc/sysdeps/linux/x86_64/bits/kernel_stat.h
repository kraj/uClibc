/* Ripped from linux/include/asm-x86_64/stat.h
 * and renamed 'struct stat' to 'struct kernel_stat' */

#ifndef _ASM_X86_64_STAT_H
#define _ASM_X86_64_STAT_H

#ifndef _LIBC
#error bits/kernel_stat.h is for internal uClibc use only!
#endif

struct kernel_stat {
	unsigned long  st_dev;
	unsigned long  st_ino;
	unsigned long  st_nlink;

	unsigned int   st_mode;
	unsigned int   st_uid;
	unsigned int   st_gid;
	unsigned int   __pad0;
	unsigned long  st_rdev;
	long           st_size;
	long           st_blksize;
	long           st_blocks;    /* Number 512-byte blocks allocated. */

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long           __unused[3];
};

/* x86-64 stat64 is same as stat */
#define kernel_stat64 kernel_stat

#endif
