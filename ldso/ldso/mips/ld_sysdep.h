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
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long*)   ARGS)

/*
 * Initialization sequence for a GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  GOT_BASE[0] = (unsigned long) _dl_linux_resolve; \
  GOT_BASE[1] = (unsigned long) MODULE; \
}

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.  RELP is the relocation that we
 * are performing, REL is the pointer to the address we are relocating.
 * SYMBOL is the symbol involved in the relocation, and LOAD is the
 * load address.
 *
 * !!!NOTE!!!
 *
 * For MIPS, we don't have any DT_JMPREL or DT_PLTRELSZ dynamic
 * entries, so this macro function is empty. The code contained
 * in elfinterp.c does the real relocation work.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD)


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
unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);

#define do_rem(result, n, base)  result = (n % base)
