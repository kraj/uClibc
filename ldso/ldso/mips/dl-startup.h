/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.
 * Copyright (C) 2005 by Joakim Tjernlund
 */

asm(""
    "	.text\n"
    "	.globl	_start\n"
    "	.type	_start,@function\n"
    "_start:\n"
    "	.set noreorder\n"
    "	bltzal $0, 0f\n"
    "	nop\n"
    "0:	.cpload $31\n"
    "	.set reorder\n"
    "	la $4, _DYNAMIC\n"
    "	sw $4, -0x7ff0($28)\n"
    "	move $4, $29\n"
    "	la $8, coff\n"
    "	.set noreorder\n"
    "	bltzal $0, coff\n"
    "	nop\n"
    "coff:	subu $8, $31, $8\n"
    "	.set reorder\n"
    "	la $25, _dl_start\n"
    "	addu $25, $8\n"
    "	jalr $25\n"
    "	move $17, $2\n"
    "	lw $4, 0($29)\n"
    "	la $5, 4($29)\n"
    "	sll $6, $4, 2\n"
    "	addu $6, $6, $5\n"
    "	addu $6, $6, 4\n"
    "	move $25, $17\n"
    "	jr $25\n"
    "	.size	_start,.-_start\n"
    "	.previous\n"
);

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = (((unsigned long *) ARGS)+1)


/*
 * Here is a macro to perform the GOT relocation. This is only
 * used when bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_GOT(tpnt)						\
do {										\
	Elf32_Sym *sym;								\
	Elf32_Addr i;								\
	register ElfW(Addr) gp __asm__ ("$28");					\
	Elf32_Addr *mipsgot = elf_mips_got_from_gpreg (gp);			\
										\
	/* Add load address displacement to all local GOT entries */		\
	i = 2;									\
	while (i < tpnt->dynamic_info[DT_MIPS_LOCAL_GOTNO_IDX])			\
		mipsgot[i++] += tpnt->loadaddr;					\
										\
	/* Handle global GOT entries */						\
	mipsgot += tpnt->dynamic_info[DT_MIPS_LOCAL_GOTNO_IDX];			\
	sym = (Elf32_Sym *) tpnt->dynamic_info[DT_SYMTAB] +			\
		 	tpnt->dynamic_info[DT_MIPS_GOTSYM_IDX];			\
	i = tpnt->dynamic_info[DT_MIPS_SYMTABNO_IDX] - tpnt->dynamic_info[DT_MIPS_GOTSYM_IDX];\
										\
	while (i--) {								\
		if (sym->st_shndx == SHN_UNDEF ||				\
			sym->st_shndx == SHN_COMMON)				\
			*mipsgot = tpnt->loadaddr + sym->st_value;		\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&		\
			*mipsgot != sym->st_value)				\
			*mipsgot += tpnt->loadaddr;				\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION) {		\
			if (sym->st_other == 0)					\
				*mipsgot += tpnt->loadaddr;			\
		}								\
		else								\
			*mipsgot = tpnt->loadaddr + sym->st_value;		\
										\
		mipsgot++;							\
		sym++;								\
	}									\
} while (0)

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB)			\
	switch(ELF32_R_TYPE((RELP)->r_info)) {					\
	case R_MIPS_REL32:							\
		if (SYMTAB) {							\
			if (symtab_index<tpnt->dynamic_info[DT_MIPS_GOTSYM_IDX])\
				*REL += SYMBOL;					\
		}								\
		else {								\
			*REL += LOAD;						\
		}								\
		break;								\
	case R_MIPS_NONE:							\
		break;								\
	default:								\
		SEND_STDERR("Aiieeee!");					\
		_dl_exit(1);							\
	}


/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then
 * call the _dl_elf_main function. For MIPS, we do it in assembly
 * because the stack doesn't get properly restored otherwise. Got look
 * at boot1_arch.h
 */
#define START() return _dl_elf_main
