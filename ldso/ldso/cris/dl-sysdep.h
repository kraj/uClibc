/* CRIS can never use Elf32_Rel relocations. */
#define ELF_USES_RELOCA

/* Initialization sequence for the GOT.  */
#define INIT_GOT(GOT_BASE,MODULE)				\
{								\
	GOT_BASE[1] = (unsigned long) MODULE; 			\
	GOT_BASE[2] = (unsigned long) _dl_linux_resolve; 	\
}

/* Defined some magic numbers that this ld.so should accept. */
#define MAGIC1 EM_CRIS
#undef MAGIC2
#define ELF_TARGET "CRIS"

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry);

/* Cheap modulo implementation, taken from arm/dl-sysdep.h. */
static inline unsigned long
cris_mod(unsigned long m, unsigned long p)
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
#define do_rem(result, n, base) ((result) = cris_mod(n, base))

/* 8192 bytes alignment */
#define PAGE_ALIGN 0xffffe000
#define ADDR_ALIGN 0x1fff
#define OFFS_ALIGN 0xffffe000

/* The union of reloc-type-classes where the reloc TYPE is a member.

   TYPE is in the class ELF_RTYPE_CLASS_PLT if it can describe a
   relocation for a PLT entry, that is, for which a PLT entry should not
   be allowed to define the value.  The GNU linker for CRIS can merge a
   .got.plt entry (R_CRIS_JUMP_SLOT) with a .got entry (R_CRIS_GLOB_DAT),
   so we need to match both these reloc types.

   TYPE is in the class ELF_RTYPE_CLASS_NOCOPY if it should not be allowed
   to resolve to one of the main executable's symbols, as for a COPY
   reloc.  */
#define elf_machine_type_class(type)				\
  ((((((type) == R_CRIS_JUMP_SLOT))				\
     || ((type) == R_CRIS_GLOB_DAT)) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_CRIS_COPY) * ELF_RTYPE_CLASS_COPY))
