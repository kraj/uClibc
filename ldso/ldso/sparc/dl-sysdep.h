
/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */
#define LINUXBIN

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

/*
 * Initialization sequence for a GOT.  For the Sparc, this points to the
 * PLT, and we need to initialize a couple of the slots.  The PLT should
 * look like:
 *
 *		save %sp, -64, %sp
 *		call _dl_linux_resolve
 *		nop
 *		.word implementation_dependent
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
   GOT_BASE[0] = 0x9de3bfc0;  /* save %sp, -64, %sp */	\
   GOT_BASE[1] = 0x40000000 | (((unsigned int) _dl_linux_resolve - (unsigned int) GOT_BASE - 4) >> 2);	\
   GOT_BASE[2] = 0x01000000; /* nop */ 			\
   GOT_BASE[3] = (int) MODULE;					\
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_SPARC
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "Sparc"

#ifndef COMPILE_ASM
extern unsigned int _dl_linux_resolver(unsigned int reloc_entry,
					unsigned int * i);
#endif

/*
 * Define this if you want a dynamic loader that works on Solaris.
 */
#ifndef __linux__
#define SOLARIS_COMPATIBLE
#endif

#ifndef COMPILE_ASM
/* Cheap modulo implementation, taken from arm/ld_sysdep.h. */
static inline unsigned long
sparc_mod(unsigned long m, unsigned long p)
{
	unsigned long i, t, inc;

	i = p;
	t = 0;

	while (!(i & (1 << 31))) {
		i <<= 1;
		t++;
	}

	t--;

	for (inc = t; inc > 2; inc--) {
		i = p << inc;

		if (i & (1 << 31))
			break;

		while (m >= i) {
			m -= i;
			i <<= 1;
			if (i & (1 << 31))
				break;
			if (i < p)
				break;
		}
	}

	while (m >= p)
		m -= p;

	return m;
}

#define do_rem(result, n, base) ((result) = sparc_mod(n, base))
#endif

/*
 * dbx wants the binder to have a specific name.  Mustn't disappoint it.
 */
#ifdef SOLARIS_COMPATIBLE
#define _dl_linux_resolve _elf_rtbndr
#endif

/* 4096 bytes alignment */
/* ...but 8192 is required for mmap() on sparc64 kernel */
#define PAGE_ALIGN 0xffffe000
#define ADDR_ALIGN 0x1fff
#define OFFS_ALIGN 0x7fffe000

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_SPARC_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)			      \
   | (((type) == R_SPARC_COPY) * ELF_RTYPE_CLASS_COPY))

/* The SPARC overlaps DT_RELA and DT_PLTREL.  */
#define ELF_MACHINE_PLTREL_OVERLAP 1
