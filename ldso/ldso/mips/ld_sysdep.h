/*
 * Various assmbly language/system dependent  hacks that are required
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
 * Initialization sequence for the application GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE)	_dl_init_got(GOT_BASE,MODULE)


/*
 * Here is a macro to perform the GOT relocation.  This is only
 * used when bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_GOT(got)					\
do {									\
	Elf32_Sym *sym;							\
	unsigned long i;						\
									\
	/* Add load address displacement to all local GOT entries */	\
	i = 2;								\
	while (i < mips_local_gotno)					\
		got[i++] += load_addr;					\
									\
	/* Handle global GOT entries */					\
	got += mips_local_gotno;					\
	sym = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] +		\
		 load_addr) + mips_gotsym;				\
	i = mips_symtabno - mips_gotsym;				\
									\
	while (i--) {							\
		if (sym->st_shndx == SHN_UNDEF ||			\
			sym->st_shndx == SHN_COMMON)			\
			*got = load_addr + sym->st_value;		\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC &&	\
			*got != sym->st_value)				\
			*got += load_addr;				\
		else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION) {	\
			if (sym->st_other == 0)				\
				*got += load_addr;			\
		}							\
		else							\
			*got = load_addr + sym->st_value;		\
									\
		got++;							\
		sym++;							\
	}								\
} while (0)


/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD)			\
	switch(ELF32_R_TYPE((RELP)->r_info)) {				\
	case R_MIPS_REL32:						\
		if (symtab_index) {					\
			if (symtab_index < mips_gotsym)			\
				*REL += SYMBOL + LOAD;			\
		}							\
		else {							\
			*REL += LOAD;					\
		}							\
		break;							\
	case R_MIPS_NONE:						\
		break;							\
	default:							\
		SEND_STDERR("Aiieeee!");				\
		_dl_exit(1);						\
	}


/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then 
 * call the _dl_elf_main function.
 */
#define START()   return (void) _dl_elf_main;      


/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_MIPS
#define MAGIC2 EM_MIPS_RS3_LE


/* Used for error messages */
#define ELF_TARGET "MIPS"


struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt,
	int reloc_entry);
void _dl_init_got(unsigned long *got, struct elf_resolve *tpnt);

#define do_rem(result, n, base)  result = (n % base)
