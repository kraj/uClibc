/* vi: set sw=4 ts=4: */
/*
 * Architecture specific code used by dl-startup.c
 * Copyright (C) 2005 by Erik Andersen <andersen@codepoet.org>
 */

asm(
    "	.text\n"
    "	.globl	_start\n"
    "	.type	_start,@function\n"
    "_start:\n"
    "	.set	_start,_dl_start\n"
    "	.size	_start,.-_start\n"
    "	.previous\n"
);

/* Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.  */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned int *) & ARGS)

/* Handle relocation of the symbols in the dynamic loader. */
static inline
void PERFORM_BOOTSTRAP_RELOC(ELF_RELOC *rpnt, unsigned long *reloc_addr,
	unsigned long symbol_addr, unsigned long load_addr, Elf32_Sym *symtab)
{
	switch (ELF32_R_TYPE(rpnt->r_info))
	{
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
			*reloc_addr = ((unsigned int) load_addr +
			              (rpnt->r_addend ? : *reloc_addr));
			break;
		default:
			_dl_exit (1);
	}
}

/* Transfer control to the user's application, once the dynamic loader is
 * done.  This routine has to exit the current function, then call the
 * _dl_elf_main function.  */
#define START() \
	__asm__ volatile ( \
		"unlk %%a6\n\t" \
		"jmp %0@" \
		: : "a" (_dl_elf_main));
