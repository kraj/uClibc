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
	{ "R_PPC_NONE", "R_PPC_ADDR32", "R_PPC_ADDR24", "R_PPC_ADDR16",
	"R_PPC_ADDR16_LO", "R_PPC_ADDR16_HI", "R_PPC_ADDR16_HA",
	"R_PPC_ADDR14", "R_PPC_ADDR14_BRTAKEN", "R_PPC_ADDR14_BRNTAKEN",
	"R_PPC_REL24", "R_PPC_REL14", "R_PPC_REL14_BRTAKEN",
	"R_PPC_REL14_BRNTAKEN", "R_PPC_GOT16", "R_PPC_GOT16_LO",
	"R_PPC_GOT16_HI", "R_PPC_GOT16_HA", "R_PPC_PLTREL24",
	"R_PPC_COPY", "R_PPC_GLOB_DAT", "R_PPC_JMP_SLOT", "R_PPC_RELATIVE",
	"R_PPC_LOCAL24PC", "R_PPC_UADDR32", "R_PPC_UADDR16", "R_PPC_REL32",
	"R_PPC_PLT32", "R_PPC_PLTREL32", "R_PPC_PLT16_LO", "R_PPC_PLT16_HI",
	"R_PPC_PLT16_HA", "R_PPC_SDAREL16", "R_PPC_SECTOFF",
	"R_PPC_SECTOFF_LO", "R_PPC_SECTOFF_HI", "R_PPC_SECTOFF_HA",
};
#define N_RELTYPES (sizeof(_dl_reltypes)/sizeof(_dl_reltypes[0]))
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
#include "sysdep.h"
#include <elf.h>
#include "linuxelf.h"
#include "hash.h"
#include "syscall.h"
#include "string.h"

extern char *_dl_progname;

extern int _dl_linux_resolve(void);

void _dl_init_got(unsigned long *plt,struct elf_resolve *tpnt)
{
	int i;
	unsigned long target_addr = (unsigned long)_dl_linux_resolve;
	unsigned int n_plt_entries;
	unsigned long *tramp;
	unsigned long data_words;
	unsigned int rel_offset_words;
	unsigned int offset;

	_dl_fdprintf(2,"init_got plt=%08lx, tpnt=%08lx\n",
		(unsigned long)plt,(unsigned long)tpnt);

	n_plt_entries = tpnt->dynamic_info[DT_PLTRELSZ] / sizeof(ELF_RELOC);
_dl_fdprintf(2,"n_plt_entries %d\n",n_plt_entries);

rel_offset_words = PLT_DATA_START_WORDS(n_plt_entries);
_dl_fdprintf(2,"rel_offset_words %08x\n",rel_offset_words);
data_words = (unsigned long)(plt + rel_offset_words);
_dl_fdprintf(2,"data_words %08x\n",data_words);

	//lpnt += PLT_INITIAL_ENTRY_WORDS;
	
	plt[PLT_LONGBRANCH_ENTRY_WORDS] = OPCODE_ADDIS_HI(11, 11, data_words);
	plt[PLT_LONGBRANCH_ENTRY_WORDS+1] = OPCODE_LWZ(11,data_words,11);

	plt[PLT_LONGBRANCH_ENTRY_WORDS+2] = OPCODE_MTCTR(11);
	plt[PLT_LONGBRANCH_ENTRY_WORDS+3] = OPCODE_BCTR();

	tramp = plt + PLT_TRAMPOLINE_ENTRY_WORDS;
	tramp[0] = OPCODE_ADDIS_HI(11,11,-data_words);
	tramp[1] = OPCODE_ADDI(11,11,-data_words);
	tramp[2] = OPCODE_SLWI(12,11,1);
	tramp[3] = OPCODE_ADD(11,12,11);
	tramp[4] = OPCODE_LI(12,target_addr);
	tramp[5] = OPCODE_ADDIS_HI(12,12,target_addr);
	tramp[6] = OPCODE_MTCTR(12);
	tramp[7] = OPCODE_LI(12,(unsigned long)tpnt);
	tramp[8] = OPCODE_ADDIS_HI(12,12,(unsigned long)tpnt);
	tramp[9] = OPCODE_BCTR();

#if 0
	offset = PLT_INITIAL_ENTRY_WORDS;
	i = 0;
	if(n_plt_entries >= PLT_DOUBLE_SIZE){
		_dl_fdprintf(2,"PLT table too large (%d>=%d)\n",
				n_plt_entries,PLT_DOUBLE_SIZE);
		_dl_exit(1);
	}
	for(i=0;i<n_plt_entries;i++){
		plt[offset] = OPCODE_LI (11,i*4);
		plt[offset+1] = OPCODE_B ((PLT_TRAMPOLINE_ENTRY_WORDS + 2
				- (offset + 1)) *4);
		offset+=2;
	}

	for(i=0;i<rel_offset_words;i+=4){
_dl_fdprintf(2,"%d %08lx\n",i,(unsigned long)(plt+i));
		//PPC_DCBST(plt+i);
		//PPC_SYNC;
		//PPC_ICBI(plt+i);
	}
#if 0
	PPC_DCBST(plt + rel_offset_words - 1);
	PPC_SYNC;
	PPC_ICBI(plt);
	PPC_ICBI(plt+rel_offset_words-1);
#endif
	//PPC_ISYNC;
#endif
}

unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry)
{
	int reloc_type;
	ELF_RELOC *this_reloc;
	char *strtab;
	Elf32_Sym *symtab;
	ELF_RELOC *rel_addr;
	int symtab_index;
	char *new_addr;
	char **got_addr;
	unsigned long instr_addr;

_dl_fdprintf(2,"linux_resolver tpnt=%08x reloc_entry=%08x\n",tpnt,reloc_entry);
	rel_addr = (ELF_RELOC *) (tpnt->dynamic_info[DT_JMPREL] + tpnt->loadaddr);

	this_reloc = (void *)rel_addr + reloc_entry;
	reloc_type = ELF32_R_TYPE(this_reloc->r_info);
	symtab_index = ELF32_R_SYM(this_reloc->r_info);

	symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);


	if (reloc_type != R_PPC_JMP_SLOT) {
		_dl_fdprintf(2, "%s: Incorrect relocation type [%s] in jump relocations\n",
			_dl_progname,
			(reloc_type<N_RELTYPES)?_dl_reltypes[reloc_type]:"unknown");
		_dl_exit(1);
	};

	/* Address of dump instruction to fix up */
	instr_addr = ((unsigned long) this_reloc->r_offset + 
		(unsigned long) tpnt->loadaddr);
	got_addr = (char **) instr_addr;

//#ifdef DEBUG
	_dl_fdprintf(2, "Resolving symbol %s\n", 
		strtab + symtab[symtab_index].st_name);
//#endif

	/* Get the address of the GOT entry */
	new_addr = _dl_find_hash(strtab + symtab[symtab_index].st_name, 
		tpnt->symbol_scope, (unsigned long) got_addr, tpnt, 0);
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
	ELF_RELOC *rpnt;
	unsigned long *reloc_addr;
	unsigned long *plt;
	int index;

_dl_fdprintf(2,"parse_lazy tpnt=%08x rel_addr=%08x rel_size=%08x, type=%d\n",
		tpnt,rel_addr,rel_size,type);
	/* Now parse the relocation information */
	rpnt = (ELF_RELOC *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(ELF_RELOC);

	symtab =
		(Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);
	plt = (unsigned long *)(tpnt->dynamic_info[DT_PLTGOT] + tpnt->loadaddr);

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

#if 0
_dl_fdprintf(2, "(lazy) resolving ");
		if (symtab_index)
			_dl_fdprintf(2, "'%s'\n", strtab + symtab[symtab_index].st_name);
_dl_fdprintf(2, "reloc_addr %08x addr %08x old %08x\n", reloc_addr, symtab[symtab_index].st_value, *reloc_addr);
_dl_fdprintf(2, "plt %08x\n",(unsigned long)plt);
#endif


		switch (reloc_type) {
		case R_PPC_NONE:
			break;
		case R_PPC_JMP_SLOT:
			{
			int delta;
			
			delta = (unsigned long)(plt+PLT_TRAMPOLINE_ENTRY_WORDS+2)
				- (unsigned long)(reloc_addr+1);

			index = ((unsigned long)reloc_addr -
				(unsigned long)(plt+PLT_INITIAL_ENTRY_WORDS))
				/sizeof(unsigned long);
			index /= 2;
//_dl_fdprintf(2, "index %08x\n",index);
//_dl_fdprintf(2, "delta %08x\n",delta);
			reloc_addr[0] = OPCODE_LI(11,index*4);
			reloc_addr[1] = OPCODE_B(delta);
			break;
			}
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

		/* instructions were modified */
		PPC_DCBST(reloc_addr);
		PPC_SYNC;
		PPC_ICBI(reloc_addr);
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
	ELF_RELOC *rpnt;
	unsigned long *reloc_addr;
	unsigned long symbol_addr;
	int symtab_index;
	unsigned long addend;
	unsigned long *plt;

//_dl_fdprintf(2,"parse_reloc tpnt=%08x rel_addr=%08x rel_size=%08x, type=%d\n",
//		tpnt,rel_addr,rel_size,type);
	/* Now parse the relocation information */

	rpnt = (ELF_RELOC *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(ELF_RELOC);
//_dl_fdprintf(2,"rpnt=%08x\n",rpnt);

	symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
	strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);
//_dl_fdprintf(2,"symtab=%08x\n",symtab);
//_dl_fdprintf(2,"strtab=%08x\n",strtab);
	plt = (unsigned long *)(tpnt->dynamic_info[DT_PLTGOT] + tpnt->loadaddr);

	for (i = 0; i < rel_size; i++, rpnt++) {
		reloc_addr = (unsigned long *) (tpnt->loadaddr + (unsigned long) rpnt->r_offset);
//_dl_fdprintf(2,"reloc_addr=%08x\n",reloc_addr);
		reloc_type = ELF32_R_TYPE(rpnt->r_info);
//_dl_fdprintf(2,"reloc_type=%08x\n",reloc_type);
		symtab_index = ELF32_R_SYM(rpnt->r_info);
//_dl_fdprintf(2,"symtab_index=%08x\n",symtab_index);
		addend = rpnt->r_addend;
//_dl_fdprintf(2,"addend=%08x\n",rpnt->r_addend);
		symbol_addr = 0;

		if (!symtab_index && tpnt->libtype == program_interpreter)
			continue;

		if (symtab_index) {

			if (tpnt->libtype == program_interpreter &&
				_dl_symbol(strtab + symtab[symtab_index].st_name))
				continue;

			symbol_addr = (unsigned long) _dl_find_hash(strtab + symtab[symtab_index].st_name, 
					tpnt->symbol_scope, (unsigned long) reloc_addr, 
					(reloc_type == R_PPC_JMP_SLOT ? tpnt : NULL), 0);

			/*
			 * We want to allow undefined references to weak symbols - this might
			 * have been intentional.  We should not be linking local symbols
			 * here, so all bases should be covered.
			 */
			if (!symbol_addr &&
				ELF32_ST_BIND(symtab[symtab_index].st_info) == STB_GLOBAL) {
				_dl_fdprintf(2, "%s: can't resolve symbol '%s'\n", 
					_dl_progname, strtab + symtab[symtab_index].st_name);
				goof++;
			}
		}
		switch (reloc_type) {
		case R_PPC_NONE:
			break;
		case R_PPC_REL24:
			{
			int delta = symbol_addr - (unsigned long)reloc_addr;
			*reloc_addr &= 0xfc000003;
			*reloc_addr |= delta&0x03fffffc;
			}
			break;
		case R_PPC_RELATIVE:
			*reloc_addr += (unsigned long)tpnt->loadaddr + addend;
			break;
		case R_PPC_ADDR32:
			*reloc_addr += symbol_addr;
			break;
		case R_PPC_JMP_SLOT:
			{
			unsigned long targ_addr = (unsigned long)_dl_linux_resolve;
			int delta = targ_addr - (unsigned long)reloc_addr;
			if(delta<<6>>6 == delta){
				*reloc_addr = OPCODE_B(delta);
			}else if (targ_addr <= 0x01fffffc || targ_addr >= 0xfe000000){
				*reloc_addr = OPCODE_BA (targ_addr);
			}else{
	{
	int delta;
	int index;
	
	delta = (unsigned long)(plt+PLT_TRAMPOLINE_ENTRY_WORDS+2)
		- (unsigned long)(reloc_addr+1);

	index = ((unsigned long)reloc_addr -
		(unsigned long)(plt+PLT_INITIAL_ENTRY_WORDS))
		/sizeof(unsigned long);
	index /= 2;
//_dl_fdprintf(2, "index %08x\n",index);
//_dl_fdprintf(2, "delta %08x\n",delta);
	reloc_addr[0] = OPCODE_LI(11,index*4);
	reloc_addr[1] = OPCODE_B(delta);
#if 0
_dl_fdprintf(2, "resolving ");
		if (symtab_index)
			_dl_fdprintf(2, "'%s'\n", strtab + symtab[symtab_index].st_name);
_dl_fdprintf(2, "type %d reloc_addr %08x addr %08x addend %08x old %08x\n",
	reloc_type, reloc_addr, symbol_addr, addend, *reloc_addr);
				_dl_fdprintf(2, "need to create PLT\n");
#endif
	}
			}
			break;
			}
		default:
			_dl_fdprintf(2, "%s: can't handle reloc type ", _dl_progname);
#ifdef VERBOSE_DLINKER
			_dl_fdprintf(2, "%s ", _dl_reltypes[reloc_type]);
#endif
			if (symtab_index)
				_dl_fdprintf(2, "'%s'\n", strtab + symtab[symtab_index].st_name);
			_dl_exit(1);
		};

		/* instructions were modified */
		PPC_DCBST(reloc_addr);
		PPC_SYNC;
		PPC_ICBI(reloc_addr);

//_dl_fdprintf(2,"reloc_addr %08x: %08x\n",reloc_addr,*reloc_addr);
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
	ELF_RELOC *rpnt;
	unsigned long *reloc_addr;
	unsigned long symbol_addr;
	struct elf_resolve *tpnt;
	int symtab_index;

_dl_fdprintf(2,"parse_copy xpnt=%08x rel_addr=%08x rel_size=%08x type=%d\n",
		(int)xpnt,rel_addr,rel_size,type);

	/* Now parse the relocation information */

	tpnt = xpnt->dyn;

	rpnt = (ELF_RELOC *) (rel_addr + tpnt->loadaddr);
	rel_size = rel_size / sizeof(ELF_RELOC);

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
