/* CRIS can never use Elf32_Rel relocations. */
#define ELF_USES_RELOCA

#include <elf.h>

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

#define do_rem(result, n, base) ((result) = (n) % (base))

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

static inline Elf32_Addr
elf_machine_dynamic(void)
{
	/* Don't just set this to an asm variable "r0" since that's not logical
	   (like, the variable is uninitialized and the register is fixed) and
	   may make GCC trip over itself doing register allocation.  Yes, I'm
	   paranoid.  Why do you ask?  */
	Elf32_Addr *got;

	__asm__ ("move.d $r0,%0" : "=rm" (got));
	return *got;
}

/* Return the run-time load address of the shared object.  We do it like
   m68k and i386, by taking an arbitrary local symbol, forcing a GOT entry
   for it, and peeking into the GOT table, which is set to the link-time
   file-relative symbol value (regardless of whether the target is REL or
   RELA).  We subtract this link-time file-relative value from the "local"
   value we calculate from GOT position and GOT offset.  FIXME: Perhaps
   there's some other symbol we could use, that we don't *have* to force a
   GOT entry for.  */

static inline Elf32_Addr
elf_machine_load_address(void)
{
	Elf32_Addr gotaddr_diff;

	__asm__ ("sub.d [$r0+_dl_start:GOT16],$r0,%0\n\t"
	         "add.d _dl_start:GOTOFF,%0" : "=r" (gotaddr_diff));
	return gotaddr_diff;
}

static inline void
elf_machine_relative(Elf32_Addr load_off, const Elf32_Addr rel_addr,
                     Elf32_Word relative_count)
{
	Elf32_Rela *rpnt = (void *)rel_addr;

	--rpnt;
	do {
		Elf32_Addr *const reloc_addr =
			(void *)(load_off + (++rpnt)->r_offset);

		*reloc_addr =  load_off + rpnt->r_addend;
	} while (--relative_count);
}
