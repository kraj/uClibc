/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.  */

/* Overrive the default _dl_boot function, and replace it with a bit of asm.
 * Then call the real _dl_boot function, which is now named _dl_boot2. */

asm("" \
"	.text\n"			\
"	.globl	_dl_boot\n"		\
"_dl_boot:\n"				\
"	mr	3,1\n"		\
"	li	4,0\n"			\
"	addi	1,1,-16\n"		\
"	stw	4,0(1)\n"		\
"	bl      _dl_boot2\n"		\
".previous\n"				\
);

#define DL_BOOT(X) static void __attribute_used__ _dl_boot2(X)

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = (((unsigned long*) ARGS)+1)

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.  RELP is the relocation that we
 * are performing, REL is the pointer to the address we are relocating.
 * SYMBOL is the symbol involved in the relocation, and LOAD is the
 * load address.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB) \
	{int type=ELF32_R_TYPE((RELP)->r_info);		\
	 Elf32_Addr finaladdr=(SYMBOL)+(RELP)->r_addend;\
	if (type==R_PPC_RELATIVE) {			\
		*REL=(Elf32_Word)(LOAD)+(RELP)->r_addend;\
	} else if (type==R_PPC_JMP_SLOT) {		\
		Elf32_Sword delta=finaladdr-(Elf32_Word)(REL);\
		*REL=OPCODE_B(delta);			\
	} else if (type==R_PPC_ADDR32) {		\
		*REL=finaladdr;				\
	} else {					\
	  _dl_exit(100+ELF32_R_TYPE((RELP)->r_info));	\
	}						\
	PPC_DCBST(REL); PPC_SYNC; PPC_ICBI(REL);	\
	}
/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then
 * call the _dl_elf_main function.
 */

/*
 * Use "b"(Address base register) operand for %1 since "b" excludes
 * r0 which is important for the addi instruction in this case. 
 */
#define START()		\
	__asm__ volatile ( \
		    "addi 1,%1,0\n\t" \
		    "mtlr %0\n\t" \
		    "blrl\n\t"	\
		    : : "r" (_dl_elf_main), "b" (args))



