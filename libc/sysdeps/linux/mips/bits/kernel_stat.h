#ifndef _BITS_STAT_STRUCT_H
#define _BITS_STAT_STRUCT_H

/* This file provides whatever this particular arch's kernel thinks 
 * struct stat should look like...  It turns out each arch has a 
 * different opinion on the subject... */
#if __WORDSIZE == 64
struct stat {
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
#else
struct stat {
	unsigned int	st_dev;
	long		st_pad1[3];		/* Reserved for network id */
	unsigned long	st_ino;
	unsigned int	st_mode;
	int		st_nlink;
	int		st_uid;
	int		st_gid;
	unsigned int	st_rdev;
	long		st_pad2[2];
	long		st_size;
	long		st_pad3;
	long		st_atime;
	long		reserved0;
	long		st_mtime;
	long		reserved1;
	long		st_ctime;
	long		reserved2;
	long		st_blksize;
	long		st_blocks;
	long		st_pad4[14];
};
#ifdef __USE_LARGEFILE64
struct stat64 {
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
#endif

#endif	/*  _BITS_STAT_STRUCT_H */

