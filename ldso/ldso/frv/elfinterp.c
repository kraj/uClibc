/* FR-V FDPIC ELF shared library loader suppport
 * Copyright (C) 2003, 2004 Red Hat, Inc.
 * Contributed by Alexandre Oliva <aoliva@redhat.com>
 * Lots of code copied from ../i386/elfinterp.c, so:
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald,
 *               David Engel, Hongjiu Lu and Mitch D'Souza
 * Copyright (C) 2001-2002, Erik Andersen
 * All rights reserved.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* Program to load an ELF binary on a linux system, and run it.
   References to symbols in sharable libraries can be resolved by either
   an ELF sharable library or a linux style of shared library. */

/* Disclaimer:  I have never seen any AT&T source code for SVr4, nor have
   I ever taken any courses on internals.  This program was developed using
   information available through the book "UNIX SYSTEM V RELEASE 4,
   Programmers guide: Ansi C and Programming Support Tools", which did
   a more than adequate job of explaining everything required to get this
   working. */

static int
_dl_do_reloc (struct elf_resolve *tpnt,struct dyn_elf *scope,
	      ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)DL_RELOC_ADDR(tpnt->loadaddr, rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	struct elf_resolve *symbol_tpnt;
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
#if defined (__SUPPORT_LD_DEBUG__)
	ElfW(Addr) old_val = 0;
#endif

	struct funcdesc_value funcval;
	ElfW(Addr) reloc_value = 0;
	struct { ElfW(Addr) v; } __attribute__((__packed__))
					    *reloc_addr_packed;
	__asm__ ("" : "=r" (reloc_addr_packed) : "0" (reloc_addr));

	if (ELF_ST_BIND(sym_ref.sym->st_info) == STB_LOCAL) {
		symbol_addr = (ElfW(Addr))DL_RELOC_ADDR(tpnt->loadaddr, sym_ref.sym->st_value);
		symbol_tpnt = tpnt;
	} else {
		symbol_addr = (ElfW(Addr))_dl_find_hash(symname, scope, NULL, 0, &sym_ref);

		/*
		 * We want to allow undefined references to weak symbols - this
		 * might have been intentional.  We should not be linking local
		 * symbols here, so all bases should be covered.
		 */
		if (unlikely(!symbol_addr
			&& (ELF_ST_BIND(sym_ref.sym->st_info) != STB_WEAK))) {
#if defined (__SUPPORT_LD_DEBUG__)
			_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
				    _dl_progname, symname, tpnt->libname);
#else
			_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
				    _dl_progname, symname);
#endif
			_dl_exit(1);
		}
		symbol_tpnt = sym_ref.tpnt;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail) {
		if ((ElfW(Addr))reloc_addr_packed & 3)
			old_val = reloc_addr_packed->v;
		else
			old_val = *reloc_addr;
	}
#endif

	switch (reloc_type) {
	case R_FRV_NONE:
		break;
	case R_FRV_32:
		if ((ElfW(Addr))reloc_addr_packed & 3)
			reloc_value = reloc_addr_packed->v += symbol_addr;
		else
			reloc_value = *reloc_addr += symbol_addr;
		break;
	case R_FRV_FUNCDESC_VALUE:
		funcval.entry_point = (void *)symbol_addr;
		/* The addend of FUNCDESC_VALUE
		   relocations referencing global
		   symbols must be ignored, because it
		   may hold the address of a lazy PLT
		   entry.  */
		if (ELF_ST_BIND(sym_ref.sym->st_info) == STB_LOCAL)
			funcval.entry_point += *reloc_addr;
		reloc_value = (ElfW(Addr))funcval.entry_point;
		if (symbol_addr)
			funcval.got_value
				= symbol_tpnt->loadaddr.got_value;
		else
			funcval.got_value = 0;
		__asm__ ("std%I0\t%1, %M0"
		     : "=m" (*(struct funcdesc_value *)reloc_addr)
		     : "e" (funcval));
		break;
	case R_FRV_FUNCDESC:
		if ((ElfW(Addr))reloc_addr_packed & 3)
			reloc_value = reloc_addr_packed->v;
		else
			reloc_value = *reloc_addr;
		if (symbol_addr)
			reloc_value = (ElfW(Addr))_dl_funcdesc_for
				((char *)symbol_addr + reloc_value,
				 symbol_tpnt->loadaddr.got_value);
		else
			reloc_value = 0;
		if ((ElfW(Addr))reloc_addr_packed & 3)
			reloc_addr_packed->v = reloc_value;
		else
			*reloc_addr = reloc_value;
		break;
	default:
		return -1;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail) {
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p\n",
			    old_val, reloc_value, reloc_addr);
		switch (reloc_type) {
		case R_FRV_FUNCDESC_VALUE:
			_dl_dprintf(_dl_debug_file, " got %x", ((struct funcdesc_value *)reloc_value)->got_value);
			break;
		case R_FRV_FUNCDESC:
			if (! reloc_value)
				break;
			_dl_dprintf(_dl_debug_file, " funcdesc (%x,%x)",
				    ((struct funcdesc_value *)reloc_value)->entry_point,
				    ((struct funcdesc_value *)reloc_value)->got_value);
			break;
		}
	}
#endif

	return 0;
}

#include "elfinterp_common.c"

#ifdef IS_IN_rtld
# include "../../libc/sysdeps/linux/frv/crtreloc.c"
#endif
