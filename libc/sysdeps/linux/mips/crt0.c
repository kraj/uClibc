/*
 * uC-libc/sysdeps/linux/mipsel/crt0.c
 * process init code for mipsel
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

/* a little bit of stuff to support C++ */
asm(
	"\t.section .ctors,\"aw\"\n"
	"\t.align 4\n"
	"\t.global __CTOR_LIST__\n"
	"__CTOR_LIST__:\n"
	"\t.long -1\n"
	"\t.section .dtors,\"aw\"\n"
	"\t.align 4\n"
	"\t.global __DTOR_LIST__\n"
	"__DTOR_LIST__:\n"
	"\t.long -1\n"
	);

void __uClibc_main(int argc,void *argv,void *envp);

void __start(void)
{
	void **p;
	int argc;

	p=__builtin_frame_address(2);

	argc=*(int *)p;

	__uClibc_main(argc,p+1,p+2+argc);
}

