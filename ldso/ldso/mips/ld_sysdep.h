/* vi: set sw=4 ts=4: */

/*
 * Various assmbly language/system dependent hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/* 
 * Define this if the system uses RELOCA.
 */
#undef ELF_USES_RELOCA


/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long *) ARGS)


/*
 * Initialization sequence for the application or library GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE)										\
do {																	\
	Elf32_Sym *sym;														\
	char *strtab;														\
	unsigned long i;													\
																		\
	/* Check if this is the dynamic linker itself */					\
	if (MODULE->libtype == program_interpreter)							\
		continue;														\
																		\
	/* Fill in first two GOT entries according to the ABI */			\
	GOT_BASE[0] = (unsigned long) _dl_linux_resolve;					\
	GOT_BASE[1] = (unsigned long) MODULE;								\
																		\
	/* Add load address displacement to all local GOT entries */		\
	i = 2;																\
	while (i < MODULE->mips_local_gotno)								\
		GOT_BASE[i++] += (unsigned long) MODULE->loadaddr;				\
																		\
	/* Handle global GOT entries */										\
	GOT_BASE += MODULE->mips_local_gotno;								\
	sym = (Elf32_Sym *) (MODULE->dynamic_info[DT_SYMTAB] +				\
		(unsigned long) MODULE->loadaddr) + MODULE->mips_gotsym;		\
	strtab = (char *) (MODULE->dynamic_info[DT_STRTAB] +				\
		(unsigned long) MODULE->loadaddr);								\
	i = MODULE->mips_symtabno - MODULE->mips_gotsym;					\
	while (i--) {														\
		if (sym->st_shndx == SHN_UNDEF) {								\
			if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&				\
				sym->st_value)											\
				*GOT_BASE = sym->st_value +								\
					(unsigned long) MODULE->loadaddr;					\
			else {														\
				*GOT_BASE = (unsigned long) _dl_find_hash(strtab +		\
					sym->st_name, MODULE->symbol_scope, NULL, 1);		\
			}															\
		}																\
		else if (sym->st_shndx == SHN_COMMON) {							\
			*GOT_BASE = (unsigned long) _dl_find_hash(strtab +			\
				sym->st_name, MODULE->symbol_scope, NULL, 1);			\
		}																\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&				\
			*GOT_BASE != sym->st_value)									\
			*GOT_BASE += (unsigned long) MODULE->loadaddr;				\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION) {			\
			if (sym->st_other == 0)										\
				*GOT_BASE += (unsigned long) MODULE->loadaddr;			\
		}																\
		else {															\
			*GOT_BASE = (unsigned long) _dl_find_hash(strtab +			\
				sym->st_name, MODULE->symbol_scope, NULL, 1);			\
		}																\
																		\
		++GOT_BASE;														\
		++sym;															\
	}																	\
} while (0)


/*
 * Here is a macro to perform the GOT relocation. This is only
 * used when bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_GOT(got)										\
do {																	\
	Elf32_Sym *sym;														\
	unsigned long i;													\
																		\
	/* Add load address displacement to all local GOT entries */		\
	i = 2;																\
	while (i < tpnt->mips_local_gotno)									\
		got[i++] += load_addr;											\
																		\
	/* Handle global GOT entries */										\
	got += tpnt->mips_local_gotno;										\
	sym = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] +				\
		 load_addr) + tpnt->mips_gotsym;								\
	i = tpnt->mips_symtabno - tpnt->mips_gotsym;						\
																		\
	while (i--) {														\
		if (sym->st_shndx == SHN_UNDEF ||								\
			sym->st_shndx == SHN_COMMON)								\
			*got = load_addr + sym->st_value;							\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&				\
			*got != sym->st_value)										\
			*got += load_addr;											\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION) {			\
			if (sym->st_other == 0)										\
				*got += load_addr;										\
		}																\
		else															\
			*got = load_addr + sym->st_value;							\
																		\
		got++;															\
		sym++;															\
	}																	\
} while (0)


/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD)					\
	switch(ELF32_R_TYPE((RELP)->r_info)) {								\
	case R_MIPS_REL32:													\
		if (symtab_index) {												\
			if (symtab_index < tpnt->mips_gotsym)						\
				*REL += SYMBOL;											\
		}																\
		else {															\
			*REL += LOAD;												\
		}																\
		break;															\
	case R_MIPS_NONE:													\
		break;															\
	default:															\
		SEND_STDERR("Aiieeee!");										\
		_dl_exit(1);													\
	}


/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then 
 * call the _dl_elf_main function. For MIPS, we do it in assembly
 * because the stack doesn't get properly restored otherwise. Got look
 * at boot1_arch.h
 */
#define START()


/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_MIPS
#define MAGIC2 EM_MIPS_RS3_LE


/* Used for error messages */
#define ELF_TARGET "MIPS"


unsigned long _dl_linux_resolver(unsigned long sym_index,
	unsigned long old_gpreg);


#define do_rem(result, n, base)  result = (n % base)
