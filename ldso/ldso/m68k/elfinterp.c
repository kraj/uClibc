/* vi: set sw=4 ts=4: */
/* m68k ELF shared library loader suppport
 *
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald,
 *                         David Engel, Hongjiu Lu and Mitch D'Souza
 * Adapted to ELF/68k by Andreas Schwab.
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

extern int _dl_linux_resolve(void);

unsigned int
_dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry)
{
	int reloc_type;
	ELF_RELOC *this_reloc;
	char *strtab;
	Elf32_Sym *symtab;
	int symtab_index;
	ELF_RELOC *rel_addr;
	char *new_addr;
	char **got_addr;
	unsigned int instr_addr;
	char *symname;

	rel_addr = (ELF_RELOC *)tpnt->dynamic_info[DT_JMPREL];
	this_reloc = (ELF_RELOC *)(intptr_t)(rel_addr + reloc_entry);
	reloc_type = ELF32_R_TYPE(this_reloc->r_info);
	symtab_index = ELF32_R_SYM(this_reloc->r_info);

	symtab = (Elf32_Sym *)(intptr_t)tpnt->dynamic_info[DT_SYMTAB];
	strtab = (char *)tpnt->dynamic_info[DT_STRTAB];
	symname = strtab + symtab[symtab_index].st_name;

	if (unlikely(reloc_type != R_68K_JMP_SLOT)) {
		_dl_dprintf(2, "%s: Incorrect relocation type in jump relocations\n",
		            _dl_progname);
		_dl_exit(1);
	}

	/* Address of the jump instruction to fix up. */
	instr_addr = ((int)this_reloc->r_offset + (int)tpnt->loadaddr);
	got_addr = (char **)instr_addr;

	/* Get the address of the GOT entry. */
	new_addr = _dl_find_hash(symname, tpnt->symbol_scope, tpnt, ELF_RTYPE_CLASS_PLT);
	if (unlikely(!new_addr)) {
		_dl_dprintf(2, "%s: Can't resolve symbol '%s'\n", _dl_progname, symname);
		_dl_exit(1);
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if ((unsigned long)got_addr < 0x40000000) {
		if (_dl_debug_bindings) {
			_dl_dprintf(_dl_debug_file, "\nresolve function: %s", symname);
			if (_dl_debug_detail)
				_dl_dprintf(_dl_debug_file,
				            "\n\tpatched: %x ==> %x @ %x",
				            *got_addr, new_addr, got_addr);
		}
	}
	if (!_dl_debug_nofixups) {
		*got_addr = new_addr;
	}
#else
	*got_addr = new_addr;
#endif

  return (unsigned int)new_addr;
}

void
_dl_parse_lazy_relocation_information(struct dyn_elf *arg_rpnt,
	unsigned long rel_addr, unsigned long rel_size)
{
	int i;
	char *strtab;
	int reloc_type;
	int symtab_index;
	Elf32_Sym *symtab;
	Elf32_Rela *rpnt;
	unsigned int *reloc_addr;
	struct elf_resolve *tpnt = arg_rpnt->dyn;

	/* Now parse the relocation information.  */
	rpnt = (Elf32_Rela *)rel_addr;
	rel_size = rel_size / sizeof (Elf32_Rela);

	symtab = (Elf32_Sym *)tpnt->dynamic_info[DT_SYMTAB];
	strtab = (char *)tpnt->dynamic_info[DT_STRTAB];

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (int *) (tpnt->loadaddr + (int) rpnt->r_offset);
		reloc_type = ELF32_R_TYPE (rpnt->r_info);
		symtab_index = ELF32_R_SYM (rpnt->r_info);

		switch (reloc_type)
		{
		case R_68K_NONE:
			break;
		case R_68K_JMP_SLOT:
			*reloc_addr += (unsigned int) tpnt->loadaddr;
		break;
		default:
			_dl_dprintf (2, "%s: (LAZY) can't handle reloc type ", _dl_progname);
#if defined (__SUPPORT_LD_DEBUG__)
			_dl_dprintf (2, "%s ", _dl_reltypes_tab[reloc_type]);
#endif
			if (symtab_index)
				_dl_dprintf (2, "'%s'", strtab + symtab[symtab_index].st_name);
			_dl_dprintf (2, "\n");
			_dl_exit (1);
		}
	}
}

int
_dl_parse_relocation_information(struct dyn_elf *arg_rpnt,
	unsigned long rel_addr, unsigned long rel_size)
{
	int i;
	char *strtab;
	int reloc_type;
	int goof = 0;
	Elf32_Sym *symtab;
	Elf32_Rela *rpnt;
	unsigned int *reloc_addr;
	unsigned int symbol_addr;
	int symtab_index;
	struct elf_resolve *tpnt = arg_rpnt->dyn;
	/* Now parse the relocation information */

	rpnt = (Elf32_Rela *)rel_addr;
	rel_size = rel_size / sizeof (Elf32_Rela);

	symtab = (Elf32_Sym *)tpnt->dynamic_info[DT_SYMTAB];
	strtab = (char *)tpnt->dynamic_info[DT_STRTAB];

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (int *) (tpnt->loadaddr + (int) rpnt->r_offset);
		reloc_type = ELF32_R_TYPE (rpnt->r_info);
		symtab_index = ELF32_R_SYM (rpnt->r_info);
		symbol_addr = 0;
		if (symtab_index) {
			symbol_addr = (unsigned int)
			_dl_find_hash (strtab + symtab[symtab_index].st_name,
			               tpnt->symbol_scope, tpnt,
			               elf_machine_type_class(reloc_type));

			/* We want to allow undefined references to weak symbols -
			   this might have been intentional.  We should not be
			   linking local symbols here, so all bases should be
			   covered.  */
			if (!symbol_addr
			    && ELF32_ST_BIND (symtab[symtab_index].st_info) != STB_WEAK)
			{
				_dl_dprintf (2, "%s: can't resolve symbol '%s'\n",
				             _dl_progname, strtab + symtab[symtab_index].st_name);
				_dl_exit (1);
			}
		}
		switch (reloc_type)
		{
			case R_68K_NONE:
				break;
			case R_68K_8:
				*(char *) reloc_addr = symbol_addr + rpnt->r_addend;
				break;
			case R_68K_16:
				*(short *) reloc_addr = symbol_addr + rpnt->r_addend;
				break;
			case R_68K_32:
				*reloc_addr = symbol_addr + rpnt->r_addend;
				break;
			case R_68K_PC8:
				*(char *) reloc_addr = (symbol_addr + rpnt->r_addend
				                       - (unsigned int) reloc_addr);
				break;
			case R_68K_PC16:
				*(short *) reloc_addr = (symbol_addr + rpnt->r_addend
				                        - (unsigned int) reloc_addr);
				break;
			case R_68K_PC32:
				*reloc_addr = (symbol_addr + rpnt->r_addend
				              - (unsigned int) reloc_addr);
				break;
			case R_68K_GLOB_DAT:
			case R_68K_JMP_SLOT:
				*reloc_addr = symbol_addr;
				break;
			case R_68K_RELATIVE:
				*reloc_addr = ((unsigned int) tpnt->loadaddr
				              /* Compatibility kludge.  */
				              + (rpnt->r_addend ? : *reloc_addr));
				break;
			case R_68K_COPY:
				_dl_memcpy ((void *) reloc_addr,
				            (void *) symbol_addr,
				            symtab[symtab_index].st_size);
				break;
			default:
				_dl_dprintf (2, "%s: can't handle reloc type ", _dl_progname);
#if defined (__SUPPORT_LD_DEBUG__)
				_dl_dprintf (2, "%s ", _dl_reltypes_tab[reloc_type]);
#endif
				if (symtab_index)
					_dl_dprintf (2, "'%s'", strtab + symtab[symtab_index].st_name);
				_dl_dprintf (2, "\n");
				_dl_exit (1);
		}
	}
	return goof;
}
