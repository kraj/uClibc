/*
 * sysdeps/linux/v850/bits/kernel_types.h -- Kernel versions of standard types
 *
 *  Copyright (C) 2001,2002  NEC Corporation
 *  Copyright (C) 2001,2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file COPYING in the main directory of this
 * archive for more details.
 *
 * Written by Miles Bader <miles@gnu.org>
 */
#ifndef __ASM_V850_POSIX_TYPES_H
#define __ASM_V850_POSIX_TYPES_H

typedef unsigned int	__kernel_dev_t;
typedef unsigned long	__kernel_ino_t;
typedef unsigned int	__kernel_mode_t;
typedef unsigned int	__kernel_nlink_t;
typedef long		__kernel_off_t;
typedef int		__kernel_pid_t;
typedef unsigned short	__kernel_ipc_pid_t;
typedef unsigned int	__kernel_uid_t;
typedef unsigned int	__kernel_gid_t;
typedef unsigned int	__kernel_size_t;
typedef int		__kernel_ssize_t;
typedef int		__kernel_ptrdiff_t;
typedef long		__kernel_time_t;
typedef long		__kernel_suseconds_t;
typedef long		__kernel_clock_t;
typedef int		__kernel_daddr_t;
typedef char *		__kernel_caddr_t;
typedef unsigned short	__kernel_uid16_t;
typedef unsigned short	__kernel_gid16_t;
typedef unsigned int	__kernel_uid32_t;
typedef unsigned int	__kernel_gid32_t;

typedef unsigned short	__kernel_old_uid_t;
typedef unsigned short	__kernel_old_gid_t;

#ifdef __GNUC__
typedef long long	__kernel_loff_t;
#endif

#endif /* __ASM_V850_POSIX_TYPES_H */
