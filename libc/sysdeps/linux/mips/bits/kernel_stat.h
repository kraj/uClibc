#ifndef _BITS_STAT_STRUCT_H
#define _BITS_STAT_STRUCT_H

/* This file provides whatever this particular arch's kernel thinks 
 * struct kernel_stat should look like...  It turns out each arch has a 
 * different opinion on the subject... */

#if __WORDSIZE == 64
#define kernel_stat kernel_stat64
#else
struct kernel_stat {
	unsigned long	st_dev;
	unsigned long	st_pad0[3];	/* Reserved for st_dev expansion  */
	unsigned long long	st_ino;
	unsigned int	st_mode;
	int		st_nlink;
	int		st_uid;
	int		st_gid;
	unsigned long	st_rdev;
	unsigned long	st_pad1[3];	/* Reserved for st_rdev expansion  */
	long long	st_size;
	long		st_atime;
	unsigned long	reserved0;	/* Reserved for st_atime expansion  */
	long		st_mtime;
	unsigned long	reserved1;	/* Reserved for st_mtime expansion  */
	long		st_ctime;
	unsigned long	reserved2;	/* Reserved for st_ctime expansion  */
	unsigned long	st_blksize;
	unsigned long	st_pad2;
	long long	st_blocks;
};
#endif

struct kernel_stat64 {
	unsigned long	st_dev;
	unsigned long	st_pad0[3];	/* Reserved for st_dev expansion  */
	unsigned long long	st_ino;
	unsigned int	st_mode;
	int		st_nlink;
	int		st_uid;
	int		st_gid;
	unsigned long	st_rdev;
	unsigned long	st_pad1[3];	/* Reserved for st_rdev expansion  */
	long long	st_size;
	long		st_atime;
	unsigned long	reserved0;	/* Reserved for st_atime expansion  */
	long		st_mtime;
	unsigned long	reserved1;	/* Reserved for st_mtime expansion  */
	long		st_ctime;
	unsigned long	reserved2;	/* Reserved for st_ctime expansion  */
	unsigned long	st_blksize;
	unsigned long	st_pad2;
	long long	st_blocks;
};

#endif	/*  _BITS_STAT_STRUCT_H */

