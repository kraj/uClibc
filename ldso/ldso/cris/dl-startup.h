/*
 * Architecture specific code used by dl-startup.c
 */

/* This code fixes the stack pointer so that the dynamic linker
 * can find argc, argv and auxvt (Auxillary Vector Table).  */
asm(""					\
"	.text\n"			\
"	.globl _start\n"		\
"	.type _start,@function\n"	\
"_start:\n"				\
"	move.d $sp,$r10\n"		\
"	move.d $pc,$r9\n"		\
"	add.d _dl_start - ., $r9\n"	\
"	jsr $r9\n"			\
"	moveq 0,$r8\n"			\
"	move $r8,$srp\n"		\
"	jump $r10\n"			\
"	.size _start,.-_start\n"	\
"	.previous\n"			\
);


/* Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.  */
#define GET_ARGV(ARGVP, ARGS) ARGVP = (((unsigned long *) ARGS)+1)


/* We can't call functions earlier in the dl startup process */
#define NO_FUNCS_BEFORE_BOOTSTRAP


/* Handle relocation of the symbols in the dynamic loader. */
static inline
void PERFORM_BOOTSTRAP_RELOC(ELF_RELOC *rpnt, unsigned long *reloc_addr,
	unsigned long symbol_addr, unsigned long load_addr, Elf32_Sym *symtab)
{
	switch (ELF32_R_TYPE(rpnt->r_info)) {
		case R_CRIS_GLOB_DAT:
		case R_CRIS_JUMP_SLOT:
		case R_CRIS_32:
			*reloc_addr = symbol_addr;
			break;
		case R_CRIS_16_PCREL:
			*(short *) *reloc_addr = symbol_addr + rpnt->r_addend - *reloc_addr - 2;
			break;
		case R_CRIS_32_PCREL:
			*reloc_addr = symbol_addr + rpnt->r_addend - *reloc_addr - 4;
			break;
		case R_CRIS_NONE:
			break;
		case R_CRIS_RELATIVE:
			*reloc_addr = load_addr + rpnt->r_addend;
			break;
		default:
			_dl_exit(1);
			break;
	}
}

/* Transfer control to the user's application, once the dynamic loader is
 * done.  This routine has to exit the current function, then call the
 * _dl_elf_main function.  */
#define START()     return _dl_elf_main
