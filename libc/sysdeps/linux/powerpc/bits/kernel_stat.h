#ifndef _BITS_STAT_STRUCT_H
#define _BITS_STAT_STRUCT_H

/* This file provides whatever this particular arch's kernel thinks 
 * struct stat should look like...  It turns out each arch has a 
 * different opinion on the subject... */
#if __WORDSIZE == 64
struct stat {
	unsigned long long st_dev; 	/* Device.  */
	unsigned long long st_ino;	/* File serial number.  */
	unsigned int st_mode;		/* File mode.  */
	unsigned int st_nlink;		/* Link count.  */
	unsigned int st_uid;		/* User ID of the file's owner.  */
	unsigned int st_gid;		/* Group ID of the file's group. */
	unsigned long long st_rdev; 	/* Device number, if device.  */
	unsigned short int __pad2;
	long long st_size;		/* Size of file, in bytes.  */
	long st_blksize;		/* Optimal block size for I/O.  */
	long long st_blocks;		/* Number 512-byte blocks allocated. */
	long st_atime;			/* Time of last access.  */
	unsigned long int __unused1;
	long st_mtime;			/* Time of last modification.  */
	unsigned long int __unused2;
	long st_ctime;			/* Time of last status change.  */
	unsigned long int __unused3;
	unsigned long int __unused4;
	unsigned long int __unused5;
};
#else
struct stat {
	unsigned int	st_dev;
	unsigned int	st_ino;
	unsigned int	st_mode;
	unsigned short	st_nlink;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned int	st_rdev;
	unsigned long int	st_size;
	unsigned long  	st_blksize;
	unsigned long  	st_blocks;
	unsigned long  	st_atime;
	unsigned long  	__unused1;
	unsigned long  	st_mtime;
	unsigned long  	__unused2;
	unsigned long  	st_ctime;
	unsigned long  	__unused3;
	unsigned long  	__unused4;
	unsigned long  	__unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned long long st_dev; 	/* Device.  */
	unsigned long long st_ino;	/* File serial number.  */
	unsigned int st_mode;		/* File mode.  */
	unsigned int st_nlink;		/* Link count.  */
	unsigned int st_uid;		/* User ID of the file's owner.  */
	unsigned int st_gid;		/* Group ID of the file's group. */
	unsigned long long st_rdev; 	/* Device number, if device.  */
	unsigned short int __pad2;
	long long st_size;		/* Size of file, in bytes.  */
	long st_blksize;		/* Optimal block size for I/O.  */
	long long st_blocks;		/* Number 512-byte blocks allocated. */
	long st_atime;			/* Time of last access.  */
	unsigned long int __unused1;
	long st_mtime;			/* Time of last modification.  */
	unsigned long int __unused2;
	long st_ctime;			/* Time of last status change.  */
	unsigned long int __unused3;
	unsigned long int __unused4;
	unsigned long int __unused5;
};
#endif
#endif

#endif	/*  _BITS_STAT_STRUCT_H */

