/* Run an ELF binary on a linux system.

   Copyright (C) 1993, Eric Youngdale.

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

#ifndef VERBOSE_DLINKER
#define VERBOSE_DLINKER
#endif
#ifdef VERBOSE_DLINKER
static char *_dl_reltypes[] =
	{ "R_386_NONE", "R_386_32", "R_386_PC32", "R_386_GOT32",
	"R_386_PLT32", "R_386_COPY", "R_386_GLOB_DAT",
	"R_386_JMP_SLOT", "R_386_RELATIVE", "R_386_GOTOFF",
	"R_386_GOTPC", "R_386_NUM"
};
#endif

/* Program to load an ELF binary on a linux system, and run it.
   References to symbols in sharable libraries can be resolved by either
   an ELF sharable library or a linux style of shared library. */

/* Disclaimer:  I have never seen any AT&T source code for SVr4, nor have
   I ever taken any courses on internals.  This program was developed using
   information available through the book "UNIX SYSTEM V RELEASE 4,
   Programmers guide: Ansi C and Programming Support Tools", which did
   a more than adequate job of explaining everything required to get this
   working. */

#include <sys/types.h>
#include <errno.h>
#include "elf.h"
#include "hash.h"
#include "syscall.h"
#include "string.h"
#include "sysdep.h"

extern char *_dl_progname;

extern int _dl_linux_resolve(void);

unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry)
{
	int reloc_type;
	Elf32_Rel *this_reloc;
	char *strtab;
	Elf32_Sym *symtab;
	Elf32_Rel *rel_addr;
	int symtab_index;
	char *new_addr;
	char **got_addr;
	unsigned long instr_addr;

	rel_addr = (Elf32_Rel *) (tpnt->dynamic_info[DT_JMPREL] + tpnt->loadaddr);

	this_reloc = rel_addr + (reloc_entry >> 3);
	reloc_type = ELF32_R_TYPE(this_reloc->r_info);
	symtab_index = ELF32_R_SYM(this_reloc->r_info);

	symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);


	if (reloc_type != R_386_JMP_SLOT) {
		_dl_fdprintf(2, "%s: Incorrect relocation type in jump relocations\n", 
			_dl_progname);
		_dl_exit(1);
	};

	/* Address of jump instruction to fix up */
	instr_addr = ((int) this_reloc->r_offset + (int) tpnt->loadaddr);
	got_addr = (char **) instr_addr;

#ifdef DEBUG
	_dl_fdprintf(2, "Resolving symbol %s\n", 
		strtab + symtab[symtab_index].st_name);
#endif

	/* Get the address of the GOT entry */
	new_addr = _dl_find_hash(strtab + symtab[symtab_index].st_name, 
		tpnt->symbol_scope, (int) got_addr, tpnt, 0);
	if (!new_addr) {
		_dl_fdprintf(2, "%s: can't resolve symbol '%s'\n", 
			_dl_progname, strtab + symtab[symtab_index].st_name);
		_dl_exit(1);
	};
/* #define DEBUG_LIBRARY */
#ifdef DEBUG_LIBRARY
	if ((unsigned long) got_addr < 0x40000000) {
		_dl_fdprintf(2, "Calling library function: %s\n", 
			strtab + symtab[symtab_index].st_name);
	} else {
		*got_addr = new_addr;
	}
#else
	*got_addr = new_addr;
#endif
	return (unsigned long) new_addr;
}

void _dl_parse_lazy_relocation_information(struct elf_resolve *tpnt, 
	unsigned long rel_addr, unsigned long rel_size, int type)
{
	int i;
	char *strtab;
	int reloc_type;
	int symtab_index;
	Elf32_Sym *symtab;
	Elf32_Rel *rpnt;
	unsigned long *reloc_addr;

	/* Now parse the relocation information */
	rpnt = (Elf32_Rel *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(Elf32_Rel);

	symtab =
		(Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (unsigned long *) (tpnt->loadaddr + (unsigned long) rpnt->r_offset);
		reloc_type = ELF32_R_TYPE(rpnt->r_info);
		symtab_index = ELF32_R_SYM(rpnt->r_info);

		/* When the dynamic linker bootstrapped itself, it resolved some symbols.
		   Make sure we do not do them again */
		if (!symtab_index && tpnt->libtype == program_interpreter)
			continue;
		if (symtab_index && tpnt->libtype == program_interpreter &&
			_dl_symbol(strtab + symtab[symtab_index].st_name))
			continue;

		switch (reloc_type) {
		case R_386_NONE:
			break;
		case R_386_JMP_SLOT:
			*reloc_addr += (unsigned long) tpnt->loadaddr;
			break;
		default:
			_dl_fdprintf(2, "%s: (LAZY) can't handle reloc type ", 
				_dl_progname);
#ifdef VERBOSE_DLINKER
			_dl_fdprintf(2, "%s ", _dl_reltypes[reloc_type]);
#endif
			if (symtab_index)
				_dl_fdprintf(2, "'%s'\n", strtab + symtab[symtab_index].st_name);
			_dl_exit(1);
		};
	};
}

int _dl_parse_relocation_information(struct elf_resolve *tpnt, 
	unsigned long rel_addr, unsigned long rel_size, int type)
{
	int i;
	char *strtab;
	int reloc_type;
	int goof = 0;
	Elf32_Sym *symtab;
	Elf32_Rel *rpnt;
	unsigned long *reloc_addr;
	unsigned long symbol_addr;
	int symtab_index;

	/* Now parse the relocation information */

	rpnt = (Elf32_Rel *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(Elf32_Rel);

	symtab =
		(Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (unsigned long *) (tpnt->loadaddr + (unsigned long) rpnt->r_offset);
		reloc_type = ELF32_R_TYPE(rpnt->r_info);
		symtab_index = ELF32_R_SYM(rpnt->r_info);
		symbol_addr = 0;

		if (!symtab_index && tpnt->libtype == program_interpreter)
			continue;

		if (symtab_index) {

			if (tpnt->libtype == program_interpreter &&
				_dl_symbol(strtab + symtab[symtab_index].st_name))
				continue;

			symbol_addr = (unsigned long)
				_dl_find_hash(strtab + symtab[symtab_index].st_name, 
					tpnt->symbol_scope, (unsigned long) reloc_addr, 
					(reloc_type == R_386_JMP_SLOT ? tpnt : NULL), 0);

			/*
			 * We want to allow undefined references to weak symbols - this might
			 * have been intentional.  We should not be linking local symbols
			 * here, so all bases should be covered.
			 */
			if (!symbol_addr &&
				ELF32_ST_BIND(symtab[symtab_index].st_info) ==
				STB_GLOBAL) {
				_dl_fdprintf(2, "%s: can't resolve symbol '%s'\n", 
					_dl_progname, strtab + symtab[symtab_index].st_name);
				goof++;
			}
		}
		switch (reloc_type) {
		case R_386_NONE:
			break;
		case R_386_32:
			*reloc_addr += symbol_addr;
			break;
		case R_386_PC32:
			*reloc_addr += symbol_addr - (unsigned long) reloc_addr;
			break;
		case R_386_GLOB_DAT:
		case R_386_JMP_SLOT:
			*reloc_addr = symbol_addr;
			break;
		case R_386_RELATIVE:
			*reloc_addr += (unsigned long) tpnt->loadaddr;
			break;
		case R_386_COPY:
#if 0							
			/* Do this later */
			_dl_fdprintf(2, "Doing copy for symbol ");
			if (symtab_index) _dl_fdprintf(2, strtab + symtab[symtab_index].st_name);
			_dl_fdprintf(2, "\n");
			_dl_memcpy((void *) symtab[symtab_index].st_value, 
				(void *) symbol_addr, symtab[symtab_index].st_size);
#endif
			break;
		default:
			_dl_fdprintf(2, "%s: can't handle reloc type ", _dl_progname);
#ifdef VERBOSE_DLINKER
			_dl_fdprintf(2, "%s ", _dl_reltypes[reloc_type]);
#endif
			if (symtab_index)
				_dl_fdprintf(2, "'%s'\n", strtab + symtab[symtab_index].st_name);
			_dl_exit(1);
		};

	};
	return goof;
}


/* This is done as a separate step, because there are cases where
   information is first copied and later initialized.  This results in
   the wrong information being copied.  Someone at Sun was complaining about
   a bug in the handling of _COPY by SVr4, and this may in fact be what he
   was talking about.  Sigh. */

/* No, there are cases where the SVr4 linker fails to emit COPY relocs
   at all */

int _dl_parse_copy_information(struct dyn_elf *xpnt, unsigned long rel_addr, 
	unsigned long rel_size, int type)
{
	int i;
	char *strtab;
	int reloc_type;
	int goof = 0;
	Elf32_Sym *symtab;
	Elf32_Rel *rpnt;
	unsigned long *reloc_addr;
	unsigned long symbol_addr;
	struct elf_resolve *tpnt;
	int symtab_index;

	/* Now parse the relocation information */

	tpnt = xpnt->dyn;

	rpnt = (Elf32_Rel *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(Elf32_Rel);

	symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (unsigned long *) (tpnt->loadaddr + (unsigned long) rpnt->r_offset);
		reloc_type = ELF32_R_TYPE(rpnt->r_info);
		if (reloc_type != R_386_COPY)
			continue;
		symtab_index = ELF32_R_SYM(rpnt->r_info);
		symbol_addr = 0;
		if (!symtab_index && tpnt->libtype == program_interpreter)
			continue;
		if (symtab_index) {

			if (tpnt->libtype == program_interpreter &&
				_dl_symbol(strtab + symtab[symtab_index].st_name))
				continue;

			symbol_addr = (unsigned long) _dl_find_hash(strtab + 
				symtab[symtab_index].st_name, xpnt->next, 
				(unsigned long) reloc_addr, NULL, 1);
			if (!symbol_addr) {
				_dl_fdprintf(2, "%s: can't resolve symbol '%s'\n", 
					_dl_progname, strtab + symtab[symtab_index].st_name);
				goof++;
			};
		};
		if (!goof) {
			_dl_memcpy((char *) symtab[symtab_index].st_value, 
				(char *) symbol_addr, symtab[symtab_index].st_size);
		}
	};
	return goof;
}
