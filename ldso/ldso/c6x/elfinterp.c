/* TI C64X DSBT ELF shared library loader suppport
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Contributed by Mark Salter <msalter@redhat.com>
 *
 * Borrowed heavily from frv arch:
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

extern void __c6x_cache_sync(unsigned long start, unsigned long end)
	attribute_hidden;

static void
_dl_c6x_flush_relocs(struct elf32_dsbt_loadmap *map)
{
	unsigned long s, e;
	s = map->segs[0].addr;
	e = s + map->segs[0].p_memsz;
	__c6x_cache_sync(s, e);
	s = map->segs[1].addr;
	e = s + map->segs[1].p_memsz;
	__c6x_cache_sync(s, e);
}

static int
_dl_do_reloc(struct elf_resolve *tpnt,struct dyn_elf *scope,
	     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) sym_val;
	ElfW(Addr) old_val, new_val;

	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)DL_RELOC_ADDR(tpnt->loadaddr, rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	const char *symname = strtab + sym_ref.sym->st_name;

	if (ELF_ST_BIND(sym_ref.sym->st_info) == STB_LOCAL) {
		symbol_addr = (ElfW(Addr))DL_RELOC_ADDR(tpnt->loadaddr, sym_ref.sym->st_value);
	} else {
		symbol_addr = (ElfW(Addr))GLRO_F(dl_find_hash)(symname, scope, tpnt,
							elf_machine_type_class(reloc_type), NULL);

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
	}

	old_val = *reloc_addr;
	sym_val = symbol_addr + rpnt->r_addend;

	switch (reloc_type) {
	case R_C6000_NONE:
		break;
	case R_C6000_ABS32:
	case R_C6000_JUMP_SLOT:
		new_val = sym_val;
		*reloc_addr = sym_val;
		break;
	case R_C6000_ABS_L16:
		new_val = (old_val & ~0x007fff80) | ((sym_val & 0xffff) << 7);
		*reloc_addr = new_val;
		break;
	case R_C6000_ABS_H16:
		new_val = (old_val & ~0x007fff80) | ((sym_val >> 9) & 0x007fff80);
		*reloc_addr = new_val;
		break;
	case R_C6000_PCR_S21:
		new_val = sym_val - ((ElfW(Addr))reloc_addr & ~31);
		*reloc_addr = (old_val & ~0x0fffff80) | (((new_val >> 2) & 0x1fffff) << 7);
		break;
	case R_C6000_COPY:
		if (symbol_addr) {
#if defined (__SUPPORT_LD_DEBUG__)
			if (_dl_debug_move)
				_dl_dprintf(_dl_debug_file,
					    "\n%s move %d bytes from %x to %p",
					    symname, sym_ref.sym->st_size,
					    symbol_addr, reloc_addr);
#endif
			_dl_memcpy((char *)reloc_addr,
				   (char *)symbol_addr,
				   sym_ref.sym->st_size);
		}
		break;
	default:
		return -1;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail && reloc_type != R_C6000_NONE)
		_dl_dprintf(_dl_debug_file, "\tpatched: %x ==> %x @ %p\n",
			    old_val, new_val, reloc_addr);
#endif

	return 0;
}
