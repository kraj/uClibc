/* vi: set sw=8 ts=8: */
/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

/*
 * Initialization sequence for a GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE)				\
{								\
	GOT_BASE[2] = (unsigned long)_dl_linux_resolve;		\
	GOT_BASE[1] = (unsigned long)(MODULE);			\
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_SH
#undef  MAGIC2
/* Used for error messages */
#define ELF_TARGET "sh64"

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);

#define do_rem(result, n, base) ((result) = (n) % (base))

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
