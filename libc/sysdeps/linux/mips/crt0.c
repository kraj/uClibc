/*
 * uC-libc/sysdeps/linux/powerpc/crt0.S
 * process init code for powerpc
 *
 * Copyright (C) 2001 by Lineo, Inc.
 * Author: David A. Schleef <ds@schleef.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#if 0
asm(
	"\t.global _start\n"
	"\t_start:\n"
//	"\tstwu	1,-32(1)\n"
//	"\tb _start2\n"
	);
#endif

void __uClibc_main(int argc,void *argv,void *envp);

void __start(void)
{
	void **p;
	int argc;

	p=__builtin_frame_address(2);

	argc=*(int *)p;

	__uClibc_main(argc,p+1,p+2+argc);
}

