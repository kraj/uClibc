/* Run an ELF binary on a linux system.

   Copyright (C) 1993, Eric Youngdale.
   Copyright (C) 2002, Steven J. Hill (sjhill@realitydiluted.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* Program to load an ELF binary on a linux system, and run it.
   References to symbols in sharable libraries can be resolved by either
   an ELF sharable library or a linux style of shared library. */

/* Disclaimer:  I have never seen any AT&T source code for SVr4, nor have
   I ever taken any courses on internals.  This program was developed using
   information available through the book "UNIX SYSTEM V RELEASE 4,
   Programmers guide: Ansi C and Programming Support Tools", which did
   a more than adequate job of explaining everything required to get this
   working. */


extern int _dl_linux_resolve(void);

void _dl_init_got(unsigned long *got, struct elf_resolve *tpnt)
{
	return;
}

unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry)
{
	return 0;
}

int _dl_parse_relocation_information(struct elf_resolve *tpnt, 
	unsigned long rel_addr, unsigned long rel_size, int type)
{
	return 1;
}
