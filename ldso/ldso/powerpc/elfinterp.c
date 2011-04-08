/* vi: set sw=4 ts=4: */
/* powerpc shared library loader suppport
 *
 * Copyright (C) 2001-2002 David A. Schleef
 * Copyright (C) 2003-2004 Erik Andersen
 * Copyright (C) 2004 Joakim Tjernlund
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

#include <ldso.h>

#define TLS_DTV_OFFSET 0x8000
#define TLS_TP_OFFSET 0x7000

void _dl_init_got(unsigned long *plt,struct elf_resolve *tpnt)
{
	ElfW(Word) *tramp;
	ElfW(Word) num_plt_entries;
	ElfW(Word) data_words;
	ElfW(Word) rel_offset_words;
	ElfW(Word) dlrr = (ElfW(Word))_dl_linux_resolve;

	if (tpnt->dynamic_info[DT_JMPREL] == 0)
		return;
	if (tpnt->dynamic_info[DT_PPC_GOT_IDX] != 0) {
		tpnt->dynamic_info[DT_PPC_GOT_IDX] += tpnt->loadaddr;
		return;
	}
	num_plt_entries = tpnt->dynamic_info[DT_PLTRELSZ] / sizeof(ELF_RELOC);
	rel_offset_words = PLT_DATA_START_WORDS(num_plt_entries);
	data_words = (ElfW(Word))(plt + rel_offset_words);
	tpnt->data_words = data_words;

	plt[PLT_LONGBRANCH_ENTRY_WORDS] = OPCODE_ADDIS_HI(11, 11, data_words);
	plt[PLT_LONGBRANCH_ENTRY_WORDS+1] = OPCODE_LWZ(11,data_words,11);

	plt[PLT_LONGBRANCH_ENTRY_WORDS+2] = OPCODE_MTCTR(11);
	plt[PLT_LONGBRANCH_ENTRY_WORDS+3] = OPCODE_BCTR();

	/* [4] */
	/* [5] */
	tramp = (ElfW(Word) *) (plt + PLT_TRAMPOLINE_ENTRY_WORDS);

	/* For the long entries, subtract off data_words.  */
	tramp[0] = OPCODE_ADDIS_HI(11,11,-data_words);
	tramp[1] = OPCODE_ADDI(11,11,-data_words);

	/* Multiply index of entry by 3 (in r11).  */
	tramp[2] = OPCODE_SLWI(12,11,1);
	tramp[3] = OPCODE_ADD(11,12,11);
	if (dlrr <= 0x01fffffc || dlrr >= 0xfe000000) {
		/* Load address of link map in r12.  */
		tramp[4] = OPCODE_LI (12, (ElfW(Word))tpnt);
		tramp[5] = OPCODE_ADDIS_HI (12, 12, (ElfW(Word))tpnt);

		/* Call _dl_linux_resolve .  */
		tramp[6] = OPCODE_BA (dlrr);
	} else {
		/* Get address of _dl_linux_resolve in CTR.  */
		tramp[4] = OPCODE_LI(12,dlrr);
		tramp[5] = OPCODE_ADDIS_HI(12,12,dlrr);
		tramp[6] = OPCODE_MTCTR(12);

		/* Load address of link map in r12.  */
		tramp[7] = OPCODE_LI(12,(ElfW(Word))tpnt);
		tramp[8] = OPCODE_ADDIS_HI(12,12,(ElfW(Word))tpnt);

		/* Call _dl_linux_resolve.  */
		tramp[9] = OPCODE_BCTR();
	}
	/* [16] unused */
	/* [17] unused */

	PPC_DCBST(plt);
	PPC_DCBST(plt+4);
	PPC_DCBST(plt+8);
	PPC_DCBST(plt+12);
	PPC_DCBST(plt+16-1);
	PPC_SYNC;
	PPC_ICBI(plt);
	PPC_ICBI(plt+16-1);
	PPC_ISYNC;
}

DL_RESOLVER_TYPE _dl_linux_resolver(struct elf_resolve *tpnt, const ElfW(Word) reloc_entry)
{
	ElfW(Addr) *reloc_addr;
	ElfW(Addr)  finaladdr;
	ElfW(Sword) delta;

	const ElfW(Sym) *const symtab = (const void *)tpnt->dynamic_info[DT_SYMTAB];
	const char *strtab = (const void *)tpnt->dynamic_info[DT_STRTAB];

	const ELF_RELOC *const this_reloc = (const void *)(tpnt->dynamic_info[DT_JMPREL] + reloc_entry);
	const int symtab_index = ELF_R_SYM(this_reloc->r_info);
	const char *symname = strtab + symtab[symtab_index].st_name;
	void *const instr_addr = (void *)DL_RELOC_ADDR(tpnt->loadaddr, this_reloc->r_offset);

	debug_sym(symtab, strtab, symtab_index);
	debug_reloc(symtab, strtab, this_reloc);

	/* Sanity check */
	_dl_assert(ELF_R_TYPE(this_reloc->r_info) != R_PPC_JMP_SLOT);

	reloc_addr = (ElfW(Addr) *)instr_addr;

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tResolving symbol '%s' %x --> ",
			    symname, (ElfW(Addr))reloc_addr);
#endif

	/* Get the address of the GOT entry */
	finaladdr = (ElfW(Addr))_dl_find_hash(symname, tpnt->symbol_scope, tpnt, ELF_RTYPE_CLASS_PLT, NULL);
	if (unlikely(!finaladdr)) {
#if defined (__SUPPORT_LD_DEBUG__)
		_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
			    _dl_progname, symname, tpnt->libname);
#else
		_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
			    _dl_progname, symname);
#endif
		_dl_exit(1);
	}

	finaladdr += this_reloc->r_addend;

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "%x\n", finaladdr);
#endif
	if (tpnt->dynamic_info[DT_PPC_GOT_IDX] != 0) {
		*reloc_addr = finaladdr;
	} else {
		delta = finaladdr - (ElfW(Word))reloc_addr;
		if (delta<<6>>6 == delta) {
			*reloc_addr = OPCODE_B(delta);
		} else if (finaladdr <= 0x01fffffc) {
			*reloc_addr = OPCODE_BA(finaladdr);
		} else {
			/* Warning: we don't handle double-sized PLT entries */
			ElfW(Word) *plt, *data_words, idx, offset;

			plt = (ElfW(Word) *)tpnt->dynamic_info[DT_PLTGOT];
			offset = reloc_addr - plt;
			idx = (offset - PLT_INITIAL_ENTRY_WORDS)/2;
			data_words = (ElfW(Word) *)tpnt->data_words;
			reloc_addr += 1;

			data_words[idx] = finaladdr;
			PPC_SYNC;
			*reloc_addr =  OPCODE_B ((PLT_LONGBRANCH_ENTRY_WORDS - (offset+1)) * 4);
		}

		/* instructions were modified */
		PPC_DCBST(reloc_addr);
		PPC_SYNC;
		PPC_ICBI(reloc_addr);
		PPC_ISYNC;
	}
	return finaladdr;
}

static __inline__ int
_dl_do_reloc(struct elf_resolve *tpnt,struct dyn_elf *scope,
	     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)(tpnt->loadaddr + rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	struct elf_resolve *tls_tpnt = NULL;
	struct symbol_ref sym_ref;
	sym_ref.sym  = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
#if defined (__SUPPORT_LD_DEBUG__)
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
#if defined (__UCLIBC_HAS_TLS__)
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
		tls_tpnt = sym_ref.tpnt;
	} else {
		/*
		 * Relocs against STN_UNDEF are usually treated as using a
		 * symbol value of zero, and using the module containing the
		 * reloc itself.
		 */
		symbol_addr = sym_ref.sym->st_value;
		tls_tpnt = tpnt;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	old_val = *reloc_addr;
#endif

	symbol_addr += rpnt->r_addend;

	switch (reloc_type) {
#if 1 /* handled by elf_machine_relative */
	case R_PPC_RELATIVE:
#endif
	case R_PPC_ADDR32:
	case R_PPC_GLOB_DAT:
		*reloc_addr = symbol_addr;
		goto out_nocode; /* No code modified */
	case R_PPC_JMP_SLOT:
	{
		if (tpnt->dynamic_info[DT_PPC_GOT_IDX] != 0) {
			*reloc_addr = symbol_addr;
			goto out_nocode; /* No code modified */
		} else {
			ElfW(Sword) delta = symbol_addr - (ElfW(Word))reloc_addr;
			if (delta<<6>>6 == delta) {
				*reloc_addr = OPCODE_B(delta);
			} else if (symbol_addr <= 0x01fffffc) {
				*reloc_addr = OPCODE_BA (symbol_addr);
			} else {
				/* Warning: we don't handle double-sized PLT entries */
				ElfW(Word) *plt, *data_words, idx, offset;

				plt = (ElfW(Word) *)tpnt->dynamic_info[DT_PLTGOT];
				offset = reloc_addr - plt;
				idx = (offset - PLT_INITIAL_ENTRY_WORDS)/2;
				data_words = (ElfW(Word) *)tpnt->data_words;

				data_words[idx] = symbol_addr;
				reloc_addr[0] = OPCODE_LI(11,idx*4);
				reloc_addr[1] = OPCODE_B((PLT_LONGBRANCH_ENTRY_WORDS - (offset+1)) * 4);

				/* instructions were modified */
				PPC_DCBST(reloc_addr+1);
				PPC_SYNC;
				PPC_ICBI(reloc_addr+1);
			}
		}
		break;
	}
	case R_PPC_COPY:
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
		goto out_nocode; /* No code modified */
	case R_PPC_ADDR16_HA:
		symbol_addr += 0x8000; /* fall through. */
	case R_PPC_ADDR16_HI:
		symbol_addr >>= 16; /* fall through. */
	case R_PPC_ADDR16_LO:
		*(short *)reloc_addr = symbol_addr;
		break;
#if defined (__UCLIBC_HAS_TLS__)
	case R_PPC_DTPMOD32:
		*reloc_addr = tls_tpnt->l_tls_modid;
		break;
	case R_PPC_DTPREL32:
		/* During relocation all TLS symbols are defined and used.
		   Therefore the offset is already correct.  */
		*reloc_addr = symbol_addr - TLS_DTV_OFFSET;
		break;
	case R_PPC_TPREL32:
		*reloc_addr = tls_tpnt->l_tls_offset + symbol_addr - TLS_TP_OFFSET;
		break;
#endif
	case R_PPC_REL24:
#if 0
		{
			ElfW(Sword)delta = symbol_addr - (ElfW(Word))reloc_addr;
			if (unlikely(delta<<6>>6 != delta)) {
				_dl_dprintf(2, "%s: symbol '%s' R_PPC_REL24 is out of range.\n\t"
						"Compile shared libraries with -fPIC!\n",
						_dl_progname, symname);
				_dl_exit(1);
			}
			*reloc_addr = (*reloc_addr & 0xfc000003) | (delta & 0x3fffffc);
			break;
		}
#else
		_dl_dprintf(2,"R_PPC_REL24: Compile shared libraries with -fPIC!\n");
		return -1;
#endif
	case R_PPC_NONE:
		goto out_nocode; /* No code modified */
	default:
		_dl_dprintf(2, "%s: can't handle reloc type ", _dl_progname);
#if defined (__SUPPORT_LD_DEBUG__)
		_dl_dprintf(2, "%s ", _dl_reltypes(reloc_type));
#endif
		if (symtab_index)
			_dl_dprintf(2, "'%s'\n", symname);
		return -1;
	}

	/* instructions were modified */
	PPC_DCBST(reloc_addr);
	PPC_SYNC;
	PPC_ICBI(reloc_addr);
	PPC_ISYNC;
 out_nocode:
#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p\n",
			    old_val, *reloc_addr, reloc_addr);
#endif
	return 0;
}

static void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
	ElfW(Addr) rel_addr, ElfW(Word) rel_size)
{
	struct elf_resolve *tpnt = rpnt->dyn;
	ElfW(Word) *plt, offset, i,  num_plt_entries, rel_offset_words;

	num_plt_entries = rel_size / sizeof(ELF_RELOC);
	plt = (ElfW(Word) *)tpnt->dynamic_info[DT_PLTGOT];
	if (tpnt->dynamic_info[DT_PPC_GOT_IDX] != 0) {
		/* Secure PLT */
		ElfW(Addr) *got = (ElfW(Addr) *)tpnt->dynamic_info[DT_PPC_GOT_IDX];
		ElfW(Word) dlrr = (ElfW(Word)) _dl_linux_resolve;

		got[1] = (ElfW(Addr)) dlrr;
		got[2] = (ElfW(Addr)) tpnt;

		/* Relocate everything in .plt by the load address offset.  */
		while (num_plt_entries-- != 0)
			*plt++ += tpnt->loadaddr;
		return;
	}

	rel_offset_words = PLT_DATA_START_WORDS(num_plt_entries);

	/* Set up the lazy PLT entries.  */
	offset = PLT_INITIAL_ENTRY_WORDS;
	i = 0;
	/* Warning: we don't handle double-sized PLT entries */
	while (i < num_plt_entries) {
		plt[offset  ] = OPCODE_LI(11, i * 4);
		plt[offset+1] = OPCODE_B((PLT_TRAMPOLINE_ENTRY_WORDS + 2 - (offset+1)) * 4);
		i++;
		offset += 2;
	}
	/* Now, we've modified code.  We need to write the changes from
	   the data cache to a second-level unified cache, then make
	   sure that stale data in the instruction cache is removed.
	   (In a multiprocessor system, the effect is more complex.)
	   Most of the PLT shouldn't be in the instruction cache, but
	   there may be a little overlap at the start and the end.

	   Assumes that dcbst and icbi apply to lines of 16 bytes or
	   more.  Current known line sizes are 16, 32, and 128 bytes.  */
	for (i = 0; i < rel_offset_words; i += 4)
		PPC_DCBST (plt + i);
	PPC_DCBST (plt + rel_offset_words - 1);
	PPC_SYNC;
	PPC_ICBI (plt);
	PPC_ICBI (plt + rel_offset_words - 1);
	PPC_ISYNC;
}
