/* vi: set sw=4 ts=4: */
/*
 * Architecture specific code used by dl-startup.c
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 */

/* For x86 we do not need any special setup so go right to _dl_boot() */
#define DL_BOOT(X) __attribute_used__ void _dl_boot (X)

/* Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.  */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long*) & ARGS)

/* Handle relocation of the symbols in the dynamic loader. */
static inline
void PERFORM_BOOTSTRAP_RELOC(ELF_RELOC *rpnt, unsigned long *reloc_addr,
	unsigned long symbol_addr, unsigned long load_addr, Elf32_Sym *symtab)
{
	switch (ELF32_R_TYPE(rpnt->r_info))
	{
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
			*reloc_addr += load_addr;
			break;
		default:
			_dl_exit(1);
	}
}


/* Transfer control to the user's application, once the dynamic loader is
 * done.  This routine has to exit the current function, then call the
 * _dl_elf_main function.  */
#define START()											\
	__asm__ volatile ("leave\n\t"						\
		    "jmp *%%eax\n\t"							\
		    : "=a" (status) :	"a" (_dl_elf_main))




