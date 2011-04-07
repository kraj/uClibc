/* vi: set sw=8 ts=8: */
/*
 * ldso/ldso/sh64/elfinterp.c
 *
 * SuperH (sh64) ELF shared library loader suppport
 *
 * Copyright (C) 2003, 2004, 2005  Paul Mundt <lethal@linux-sh.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

#include <ldso.h>

static int _dl_do_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope,
			ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)(tpnt->loadaddr + rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr  = 0;
	/*struct elf_resolve *tls_tpnt = NULL;*/
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
#if defined (__SUPPORT_LD_DEBUG__)
	ElfW(Addr) old_val;
#endif

	const int lsb = !!(sym_ref.sym->st_other & STO_SH5_ISA32);

	if (symtab_index) {
		symbol_addr = (ElfW(Addr))_dl_find_hash(symname, scope, tpnt,
							elf_machine_type_class(reloc_type), &sym_ref);

		/*
		 * We want to allow undefined references to weak symbols - this
		 * might have been intentional.  We should not be linking local
		 * symbols here, so all bases should be covered.
		 */
		if (unlikely(!symbol_addr
#if 0 /*defined (__UCLIBC_HAS_TLS__)*/
			&& (ELF_ST_TYPE(sym_ref.sym->st_info) != STT_TLS)
#endif
			&& (ELF_ST_BIND(sym_ref.sym->st_info) != STB_WEAK))) {
#if defined (__SUPPORT_LD_DEBUG__)
			_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
				    _dl_progname, symname, tpnt->libname);
#else
			_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
				    _dl_progname, symname);
#endif

			/* Let the caller handle the error: it may be non fatal if called from dlopen */
			return 1;
		}
		/*tls_tpnt = sym_ref.tpnt;*/
	} else {
		/*
		 * Relocs against STN_UNDEF are usually treated as using a
		 * symbol value of zero, and using the module containing the
		 * reloc itself.
		 */
		symbol_addr = sym_ref.sym->st_value;
		/*tls_tpnt = tpnt;*/
	}

#if defined (__SUPPORT_LD_DEBUG__)
	old_val = *reloc_addr;
#endif

	symbol_addr += rpnt->r_addend;

	switch (reloc_type) {
	case R_SH_NONE:
		break;
	case R_SH_DIR32:
	case R_SH_GLOB_DAT:
	case R_SH_JMP_SLOT:
		*reloc_addr = symbol_addr | lsb;
		break;
#if 1 /* handled by elf_machine_relative */
	case R_SH_RELATIVE:
		*reloc_addr = tpnt->loadaddr + rpnt->r_addend;
		break;
#endif
	case R_SH_COPY:
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
	case R_SH_REL32:
		*reloc_addr = symbol_addr - reloc_addr;
		break;
	case R_SH_RELATIVE_LOW16:
	case R_SH_RELATIVE_MEDLOW16:
	    {
		unsigned long word, value;

		word = (unsigned long)reloc_addr & ~0x3fffc00;
		value = (unsigned long)tpnt->loadaddr + rpnt->r_addend;

		if (reloc_type == R_SH_RELATIVE_MEDLOW16)
			value >>= 16;

		word |= (value & 0xffff) << 10;
		*reloc_addr = word;

		break;
	    }
	case R_SH_IMM_LOW16:
	case R_SH_IMM_MEDLOW16:
	    {
		unsigned long word, value;

		word = (unsigned long)reloc_addr & ~0x3fffc00;
		value = symbol_addr | lsb;

		if (reloc_type == R_SH_IMM_MEDLOW16)
			value >>= 16;

		word |= (value & 0xffff) << 10;
		*reloc_addr = word;

		break;
	    }
	case R_SH_IMM_LOW16_PCREL:
	case R_SH_IMM_MEDLOW16_PCREL:
	    {
		unsigned long word, value;

		word = (unsigned long)reloc_addr & ~0x3fffc00;
		value = symbol_addr - (unsigned long)reloc_addr;

		if (reloc_type == R_SH_IMM_MEDLOW16_PCREL)
			value >>= 16;

		word |= (value & 0xffff) << 10;
		*reloc_addr = word;

		break;
	    }
	default:
		return -1;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p\n",
			    old_val, *reloc_addr, reloc_addr);
#endif

	return 0;
}

static int _dl_do_lazy_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope attribute_unused,
			     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab,
			     const char *strtab attribute_unused)
{
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)DL_RELOC_ADDR(tpnt->loadaddr, rpnt->r_offset);
#if defined (__SUPPORT_LD_DEBUG__)
	ElfW(Addr) old_val = *reloc_addr;
#endif

	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	const int lsb = !!(symtab[symtab_index].st_other & STO_SH5_ISA32);

	switch (reloc_type) {
		case R_SH_NONE:
			break;
		case R_SH_JMP_SLOT:
			*reloc_addr += tpnt->loadaddr | lsb;
			break;
		default:
			return -1;
	}

#ifdef __SUPPORT_LD_DEBUG__
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p",
			    old_val, *reloc_addr, reloc_addr);
#endif

	return 0;
}
