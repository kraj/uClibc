/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

/*
 * Initialization sequence for a GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  GOT_BASE[2] = (unsigned long) _dl_linux_resolve; \
  GOT_BASE[1] = (unsigned long) (MODULE); \
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_SH
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "sh"

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);

static __inline__ unsigned int
_dl_urem(unsigned int n, unsigned int base)
{
  int res;

	__asm__ (""\
		"mov	#0, r0\n\t" \
		"div0u\n\t" \
		"" \
		"! get one bit from the msb of the numerator into the T\n\t" \
		"! bit and divide it by whats in %2.  Put the answer bit\n\t" \
		"! into the T bit so it can come out again at the bottom\n\t" \
		""				\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		""				\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		""				\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
 		""				\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1 ; div1 %2, r0\n\t"	\
		"rotcl	%1\n\t"
		: "=r" (res)
		: "0" (n), "r" (base)
		: "r0","cc");

	return n - (base * res);
}

#define do_rem(result, n, base)     ((result) = _dl_urem((n), (base)))

/* 4096 bytes alignment */
#define PAGE_ALIGN 0xfffff000
#define ADDR_ALIGN 0xfff
#define OFFS_ALIGN 0x7ffff000

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry or
   TLS variable, so undefined references should not be allowed to
   define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_SH_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_SH_COPY) * ELF_RTYPE_CLASS_COPY))
