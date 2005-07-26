/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.  See arm/boot1_arch.h for an example of what
 * can be done.
 */
asm(
	"	.text\n"
	"	.global _start\n"
	"	.type   _start,%function\n"
	"_start:\n"
	"	.set _start,_dl_start\n"
	"	.size _start,.-_start\n"
	"	.previous\n"
);

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.  We assume that argc is stored
 * at the word just below the argvp that we return here.
 */
#define GET_ARGV(ARGVP, ARGS) __asm__("\tadd %%fp,68,%0\n" : "=r" (ARGVP));

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB) \
	switch(ELF32_R_TYPE((RELP)->r_info)) { \
	case R_SPARC_32: \
		*REL = SYMBOL + (RELP)->r_addend; \
		break; \
	case R_SPARC_GLOB_DAT: \
		*REL = SYMBOL + (RELP)->r_addend; \
		break; \
	case R_SPARC_JMP_SLOT: \
		REL[1] = 0x03000000 | ((SYMBOL >> 10) & 0x3fffff); \
		REL[2] = 0x81c06000 | (SYMBOL & 0x3ff); \
		break; \
	case R_SPARC_NONE: \
		break; \
	case R_SPARC_WDISP30: \
		break; \
	case R_SPARC_RELATIVE: \
		*REL += (unsigned int) LOAD + (RELP)->r_addend; \
		break; \
	default: \
		_dl_exit(1); \
	}

/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  The crt calls atexit with $g1 if not null, so we need to
 * ensure that it contains NULL.
 */

#define START() \
	__asm__ volatile ( \
		"add %%g0,%%g0,%%g1\n\t" \
		"jmpl %0, %%o7\n\t"	\
		"restore %%g0,%%g0,%%g0\n\t" \
		: /*"=r" (status) */ : \
		"r" (_dl_elf_main): "g1", "o0", "o1")
