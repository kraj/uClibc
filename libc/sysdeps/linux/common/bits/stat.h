/* Copyright (C) 1992,95,96,97,98,99,2000,2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _SYS_STAT_H
# error "Never include <bits/stat.h> directly; use <sys/stat.h> instead."
#endif

#ifndef _BITS_STAT_H
#define _BITS_STAT_H

/* Versions of the `struct stat' data structure.  */
#define _STAT_VER_LINUX_OLD	1
#define _STAT_VER_KERNEL	1
#define _STAT_VER_SVR4		2
#define _STAT_VER_LINUX		3
#define _STAT_VER		_STAT_VER_LINUX	/* The one defined below.  */

/* Versions of the `xmknod' interface.  */
#define _MKNOD_VER_LINUX	1
#define _MKNOD_VER_SVR4		2
#define _MKNOD_VER		_MKNOD_VER_LINUX /* The bits defined below.  */

/* Pull in whatever this particular arch's kernel thinks that struct stat 
 * should look like.  It turns out that each arch has a different opinion 
 * on the subject, and different kernel revs use different names... */
#if defined(__alpha__)
struct stat {
	unsigned int	st_dev;
	unsigned int	st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned int	st_rdev;
	long		st_size;
	unsigned long	st_atime;
	unsigned long	st_mtime;
	unsigned long	st_ctime;
	unsigned int	st_blksize;
	int		st_blocks;
	unsigned int	st_flags;
	unsigned int	st_gen;
};
/* End of __alpha__ */
#elif defined(__arm__)
struct stat {
	unsigned short st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned short	st_dev;
	unsigned char	__pad0[10];
#define STAT64_HAS_BROKEN_ST_INO	1
	unsigned long	__st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned long	st_uid;
	unsigned long	st_gid;
	unsigned short	st_rdev;
	unsigned char	__pad3[10];
	long long	st_size;
	unsigned long	st_blksize;
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	__pad4;		/* future possible st_blocks high bits */
	unsigned long	st_atime;
	unsigned long	__pad5;
	unsigned long	st_mtime;
	unsigned long	__pad6;
	unsigned long	st_ctime;
	unsigned long	__pad7;		/* will be high 32 bits of ctime someday */
	unsigned long long	st_ino;
};
#endif
/* End of __arm__ */
#elif defined(__cris__)
struct stat {
	unsigned short st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned short	st_dev;
	unsigned char	__pad0[10];
#define STAT64_HAS_BROKEN_ST_INO	1
	unsigned long	__st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned long	st_uid;
	unsigned long	st_gid;
	unsigned short	st_rdev;
	unsigned char	__pad3[10];
	long long	st_size;
	unsigned long	st_blksize;
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	__pad4;		/* future possible st_blocks high bits */
	unsigned long	st_atime;
	unsigned long	__pad5;
	unsigned long	st_mtime;
	unsigned long	__pad6;
	unsigned long	st_ctime;
	unsigned long	__pad7;		/* will be high 32 bits of ctime someday */
	unsigned long long	st_ino;
};
#endif
/* End of __cris__ */
#elif defined(__ia64__)
struct stat {
	unsigned long	st_dev;
	unsigned long	st_ino;
	unsigned long	st_nlink;
	unsigned int	st_mode;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned int	__pad0;
	unsigned long	st_rdev;
	unsigned long	st_size;
	unsigned long	st_atime;
	unsigned long	__reserved0;	/* reserved for atime.nanoseconds */
	unsigned long	st_mtime;
	unsigned long	__reserved1;	/* reserved for mtime.nanoseconds */
	unsigned long	st_ctime;
	unsigned long	__reserved2;	/* reserved for ctime.nanoseconds */
	unsigned long	st_blksize;
	long		st_blocks;
	unsigned long	__unused[3];
};
/* End of __ia64__ */
#elif defined(__mc68000__)
struct stat {
	unsigned short st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned char	__pad0[6];
	unsigned short	st_dev;
	unsigned char	__pad1[2];
#define STAT64_HAS_BROKEN_ST_INO	1
	unsigned long	__st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned long	st_uid;
	unsigned long	st_gid;
	unsigned char	__pad2[6];
	unsigned short	st_rdev;
	unsigned char	__pad3[2];
	long long	st_size;
	unsigned long	st_blksize;
	unsigned long	__pad4;		/* future possible st_blocks high bits */
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	st_atime;
	unsigned long	__pad5;
	unsigned long	st_mtime;
	unsigned long	__pad6;
	unsigned long	st_ctime;
	unsigned long	__pad7;		/* will be high 32 bits of ctime someday */
	unsigned long long	st_ino;
};
#endif
/* End of __mc68000__ */
#elif defined(__mips__)
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
/* End of __mips__ */
#elif defined(__powerpc__)
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
/* End of __powerpc__ */
#elif defined (__s390__)
struct stat {
        unsigned short st_dev;
        unsigned short __pad1;
        unsigned long st_ino;
        unsigned short st_mode;
        unsigned short st_nlink;
        unsigned short st_uid;
        unsigned short st_gid;
        unsigned short st_rdev;
        unsigned short __pad2;
        unsigned long  st_size;
        unsigned long  st_blksize;
        unsigned long  st_blocks;
        unsigned long  st_atime;
        unsigned long  __unused1;
        unsigned long  st_mtime;
        unsigned long  __unused2;
        unsigned long  st_ctime;
        unsigned long  __unused3;
        unsigned long  __unused4;
        unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
        unsigned char   __pad0[6];
        unsigned short  st_dev;
        unsigned int    __pad1;
#define STAT64_HAS_BROKEN_ST_INO        1
        unsigned long   __st_ino;
        unsigned int    st_mode;
        unsigned int    st_nlink;
        unsigned long   st_uid;
        unsigned long   st_gid;
        unsigned char   __pad2[6];
        unsigned short  st_rdev;
        unsigned int    __pad3;
        long long       st_size;
        unsigned long   st_blksize;
        unsigned char   __pad4[4];
        unsigned long   __pad5;     /* future possible st_blocks high bits */
        unsigned long   st_blocks;  /* Number 512-byte blocks allocated. */
        unsigned long   st_atime;
        unsigned long   __pad6;
        unsigned long   st_mtime;
        unsigned long   __pad7;
        unsigned long   st_ctime;
        unsigned long   __pad8;     /* will be high 32 bits of ctime someday */
        unsigned long long      st_ino;
};
#endif
/* End of __s390__ */
#elif defined(__sh3__) || defined(__SH4__)
#include <endian.h>
struct stat {
	unsigned short st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
#if defined(__BIG_ENDIAN__)
	unsigned char   __pad0b[6];
	unsigned short	st_dev;
#elif defined(__LITTLE_ENDIAN__)
	unsigned short	st_dev;
	unsigned char	__pad0b[6];
#else
#error Must know endian to build stat64 structure!
#endif
	unsigned char	__pad0[4];

	unsigned long	st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;

	unsigned long	st_uid;
	unsigned long	st_gid;

#if defined(__BIG_ENDIAN__)
	unsigned char	__pad3b[6];
	unsigned short	st_rdev;
#else /* Must be little */
	unsigned short	st_rdev;
	unsigned char	__pad3b[6];
#endif
	unsigned char	__pad3[4];

	long long	st_size;
	unsigned long	st_blksize;

#if defined(__BIG_ENDIAN__)
	unsigned long	__pad4;		/* Future possible st_blocks hi bits */
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
#else /* Must be little */
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	__pad4;		/* Future possible st_blocks hi bits */
#endif

	unsigned long	st_atime;
	unsigned long	__pad5;

	unsigned long	st_mtime;
	unsigned long	__pad6;

	unsigned long	st_ctime;
	unsigned long	__pad7;		/* will be high 32 bits of ctime someday */

	unsigned long	__unused1;
	unsigned long	__unused2;
};
#endif
#elif defined (__sparc__)
#if __WORDSIZE == 64
struct stat {
	unsigned char	__pad0[6];
	unsigned short	st_dev;
	unsigned long long	st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned char	__pad2[6];
	unsigned short	st_rdev;
	unsigned char	__pad3[8];
	long long	st_size;
	unsigned int	st_blksize;
	unsigned char	__pad4[8];
	unsigned int	st_blocks;
	unsigned int	st_atime;
	unsigned int	__unused1;
	unsigned int	st_mtime;
	unsigned int	__unused2;
	unsigned int	st_ctime;
	unsigned int	__unused3;
	unsigned int	__unused4;
	unsigned int	__unused5;
};
#else
struct stat {
	unsigned short	st_dev;
	unsigned long	st_ino;
	unsigned short	st_mode;
	short		st_nlink;
	unsigned short	st_uid;
	unsigned short	st_gid;
	unsigned short	st_rdev;
	long		st_size;
	long		st_atime;
	unsigned long	__unused1;
	long		st_mtime;
	unsigned long	__unused2;
	long		st_ctime;
	unsigned long	__unused3;
	long		st_blksize;
	long		st_blocks;
	unsigned long	__unused4[2];
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned char	__pad0[6];
	unsigned short	st_dev;
	unsigned long long	st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned char	__pad2[6];
	unsigned short	st_rdev;
	unsigned char	__pad3[8];
	long long	st_size;
	unsigned int	st_blksize;
	unsigned char	__pad4[8];
	unsigned int	st_blocks;
	unsigned int	st_atime;
	unsigned int	__unused1;
	unsigned int	st_mtime;
	unsigned int	__unused2;
	unsigned int	st_ctime;
	unsigned int	__unused3;
	unsigned int	__unused4;
	unsigned int	__unused5;
};
#endif
#endif
/* End if __sparc__ */
#elif defined(__x86_64__)
struct stat {
	unsigned long	st_dev;
	unsigned long	st_ino;
	unsigned long	st_nlink;

	unsigned int	st_mode;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned int	__pad0;
	unsigned long	st_rdev;
	long		st_size;
	long		st_blksize;
	long		st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	st_atime;
	unsigned long	__reserved0;	/* reserved for atime.nanoseconds */
	unsigned long	st_mtime;
	unsigned long	__reserved1;	/* reserved for atime.nanoseconds */
	unsigned long	st_ctime;
	unsigned long	__reserved2;	/* reserved for atime.nanoseconds */
  	long		__unused[3];
};
/* End if __x86_64__ */
//#elif defined(__i386__)
#else

#if ! defined(__i386__)
#warning please verify that struct stat for your architecture exactly matches struct stat for x86
#endif

struct stat {
	unsigned short st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};
#ifdef __USE_LARGEFILE64
struct stat64 {
	unsigned short	st_dev;
	unsigned char	__pad0[10];
#define STAT64_HAS_BROKEN_ST_INO	1
	unsigned long	__st_ino;
	unsigned int	st_mode;
	unsigned int	st_nlink;
	unsigned long	st_uid;
	unsigned long	st_gid;
	unsigned short	st_rdev;
	unsigned char	__pad3[10];
	long long	st_size;
	unsigned long	st_blksize;
	unsigned long	st_blocks;	/* Number 512-byte blocks allocated. */
	unsigned long	__pad4;		/* future possible st_blocks high bits */
	unsigned long	st_atime;
	unsigned long	__pad5;
	unsigned long	st_mtime;
	unsigned long	__pad6;
	unsigned long	st_ctime;
	unsigned long	__pad7;		/* will be high 32 bits of ctime someday */
	unsigned long long	st_ino;
};
#endif
/* End of __i386__ */
#endif

#ifdef __USE_LARGEFILE64
#  ifdef __USE_FILE_OFFSET64
#    define stat   stat64
#  endif
#endif


/* Tell code we have these members.  */
#define	_STATBUF_ST_BLKSIZE
#define _STATBUF_ST_RDEV

/* Encoding of the file mode.  */

#define	__S_IFMT	0170000	/* These bits determine file type.  */

/* File types.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define	__S_IFIFO	0010000	/* FIFO.  */
#define	__S_IFLNK	0120000	/* Symbolic link.  */
#define	__S_IFSOCK	0140000	/* Socket.  */

/* POSIX.1b objects.  Note that these macros always evaluate to zero.  But
   they do it by enforcing the correct use of the macros.  */
#define __S_TYPEISMQ(buf)  ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSEM(buf) ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSHM(buf) ((buf)->st_mode - (buf)->st_mode)

/* Protection bits.  */

#define	__S_ISUID	04000	/* Set user ID on execution.  */
#define	__S_ISGID	02000	/* Set group ID on execution.  */
#define	__S_ISVTX	01000	/* Save swapped text after use (sticky).  */
#define	__S_IREAD	0400	/* Read by owner.  */
#define	__S_IWRITE	0200	/* Write by owner.  */
#define	__S_IEXEC	0100	/* Execute by owner.  */

#endif	/* _BITS_STAT_H */

