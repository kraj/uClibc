
/* Various assmbly language/system dependent hacks that are required
   so that we can minimize the amount of platform specific code. */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

/* Initialization sequence for a GOT.  */
#define INIT_GOT(GOT_BASE,MODULE)		\
{						\
  GOT_BASE[2] = (int) _dl_linux_resolve;	\
  GOT_BASE[1] = (int) (MODULE);			\
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_68K
#undef MAGIC2
/* Used for error messages */
#define ELF_TARGET "m68k"

struct elf_resolve;
extern unsigned int _dl_linux_resolver (int, int, struct elf_resolve *, int);

/* Define this because we do not want to call .udiv in the library.
   Not needed for m68k.  */
#define do_rem(result, n, base)  ((result) = (n) % (base))

/* 4096 bytes alignment */
#define PAGE_ALIGN 0xfffff000
#define ADDR_ALIGN 0xfff
#define OFFS_ALIGN 0x7ffff000

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_68K_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_68K_COPY) * ELF_RTYPE_CLASS_COPY))
