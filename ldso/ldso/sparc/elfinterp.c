/* vi: set sw=4 ts=4: */
/* sparc ELF shared library loader suppport
 *
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald,
 *				David Engel, Hongjiu Lu and Mitch D'Souza
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

/* Some SPARC opcodes we need to use for self-modifying code.  */
#define OPCODE_NOP	0x01000000 /* nop */
#define OPCODE_CALL	0x40000000 /* call ?; add PC-rel word address */
#define OPCODE_SETHI_G1	0x03000000 /* sethi ?, %g1; add value>>10 */
#define OPCODE_JMP_G1	0x81c06000 /* jmp %g1+?; add lo 10 bits of value */
#define OPCODE_SAVE_SP	0x9de3bfa8 /* save %sp, -(16+6)*4, %sp */
#define OPCODE_BA	0x30800000 /* b,a ?; add PC-rel word address */

static int
_dl_do_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope,
	     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)(tpnt->loadaddr + rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	struct elf_resolve *tls_tpnt = NULL;
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
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

	symbol_addr += rpnt->r_addend;	/* Assume copy relocs have zero addend.  */

	switch (reloc_type) {
		case R_SPARC_NONE:
			break;
		case R_SPARC_DISP32:
			*reloc_addr = symbol_addr - (unsigned int)reloc_addr;
			break;
		case R_SPARC_LO10:
			if (!symbol_addr)
				symbol_addr = tpnt->loadaddr + rpnt->r_addend;
			else
				symbol_addr += rpnt->r_addend;
			*reloc_addr = (*reloc_addr & ~0x3ff) | (symbol_addr & 0x3ff);
			break;
		case R_SPARC_GLOB_DAT:
		case R_SPARC_32:
			*reloc_addr = symbol_addr;
			break;
		case R_SPARC_JMP_SLOT:
			reloc_addr[1] = OPCODE_SETHI_G1 | (( symbol_addr >> 10 ) & 0x3fffff);
			reloc_addr[2] = OPCODE_JMP_G1 | ( symbol_addr & 0x3ff );
			break;
		case R_SPARC_RELATIVE:
			*reloc_addr += tpnt->loadaddr + rpnt->r_addend;
			break;
		case R_SPARC_WDISP30:
			*reloc_addr = (*reloc_addr & 0xc0000000)|
			         ((symbol_addr - (unsigned int) reloc_addr) >> 2);
			break;
		case R_SPARC_HI22:
			if (!symbol_addr)
				symbol_addr = tpnt->loadaddr + rpnt->r_addend;
			else
				/* psm: I can't see, how this is useful, we already added rpnt->r_addend */
				symbol_addr += rpnt->r_addend;
			*reloc_addr = (*reloc_addr & 0xffc00000) | (symbol_addr >> 10);
			break;
		case R_SPARC_COPY:
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
#if defined (__UCLIBC_HAS_TLS__)
		case R_SPARC_TLS_DTPMOD32:
			*reloc_addr = tls_tpnt->l_tls_modid;
			break;
		case R_SPARC_TLS_DTPOFF32:
			/* During relocation all TLS symbols are defined and used.
			 * Therefore the offset is already correct.  */
			*reloc_addr = sym_ref.sym->st_value + rpnt->r_addend;
			break;
		case R_SPARC_TLS_TPOFF32:
			/* The offset is negative, forward from the thread pointer.
			 * We know the offset of the object the symbol is contained in.
			 * It is a negative value which will be added to the
			 * thread pointer.  */
			CHECK_STATIC_TLS((struct link_map *)tls_tpnt);
			*reloc_addr = sym_ref.sym->st_value - tls_tpnt->l_tls_offset + rpnt->r_addend;
			break;
#endif
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

#include "elfinterp_common.c"
