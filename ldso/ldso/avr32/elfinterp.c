/*
 * AVR32 ELF shared library loader suppport
 *
 * Copyright (C) 2004-2006 Atmel Corporation
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

/*unsigned long _dl_linux_resolver(unsigned long got_offset, unsigned long *got)*/
DL_RESOLVER_TYPE _dl_linux_resolver(struct elf_resolve *tpnt, const ElfW(Word) reloc_entry)
{
	/*
	 * AVR32 currently does not do lazy relocation.
	 */
#if 0
	struct elf_resolve *tpnt = (struct elf_resolve *)got[1];
	Elf32_Sym *sym;
	unsigned long local_gotno;
	unsigned long gotsym;
	unsigned long new_addr;
	char *strtab, *symname;
	unsigned long *entry;
	unsigned long sym_index = got_offset / 4;

	local_gotno = tpnt->dynamic_info[DT_AVR32_LOCAL_GOTNO];
	gotsym = tpnt->dynamic_info[DT_AVR32_GOTSYM];

	sym = ((Elf32_Sym *)(tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr))
		+ sym_index;
	strtab = (char *)(tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);
	symname = strtab + sym->st_name;

	new_addr = (unsigned long) _dl_find_hash(symname,
						 tpnt->symbol_scope, tpnt, ELF_RTYPE_CLASS_PLT, NULL);

	entry = (unsigned long *)(got + local_gotno + sym_index - gotsym);
	*entry = new_addr;

	return new_addr;
#endif
	return 0;
}

static int _dl_do_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope,
			ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)(tpnt->loadaddr + rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	/* struct elf_resolve *tls_tpnt = NULL; */
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
#if defined(__SUPPORT_LD_DEBUG__)
	ElfW(Addr) old_val;
#endif

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

#if defined(__SUPPORT_LD_DEBUG__)
	old_val = *reloc_addr;
#endif

	symbol_addr += rpnt_r_addend;

	switch (reloc_type) {
	case R_AVR32_NONE:
		break;
	case R_AVR32_GLOB_DAT:
	case R_AVR32_JMP_SLOT:
		*reloc_addr = symbol_addr;
		break;
#if 1 /* handled by elf_machine_relative */
	case R_AVR32_RELATIVE:
		*reloc_addr = tpnt->loadaddr + rpnt->r_addend;
		break;
#endif
	default:
		return -1;
	}

#if defined(__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p\n",
			    old_val, *reloc_addr, reloc_addr);
#endif

	return 0;
}

#include "elfinterp_common.c"
