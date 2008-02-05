/*
 * Meta can never use Elf32_Rel relocations.
 *
 * Copyright (C) 2013, Imagination Technologies Ltd.
 *
 * Licensed under LGPL v2.1 or later, see the file COPYING.LIB in this tarball.
 */

#define ELF_USES_RELOCA

#include <elf.h>

/* Initialization sequence for the GOT.  */
#define INIT_GOT(GOT_BASE,MODULE)				\
{								\
	GOT_BASE[1] = (unsigned long) MODULE; 			\
	GOT_BASE[2] = (unsigned long) _dl_linux_resolve; 	\
}

/* Maximum unsigned GOT [GS]ETD offset size, ie. 2^(11+2). */
#define GOT_REG_OFFSET 0x2000

/* Defined some magic numbers that this ld.so should accept. */
#define MAGIC1 EM_METAG
#undef  MAGIC2
#define ELF_TARGET "META"

/* Need bootstrap relocations */
#define ARCH_NEEDS_BOOTSTRAP_RELOCS

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve *tpnt, int reloc_entry);

/* Variable page size. */
#define ADDR_ALIGN (_dl_pagesize - 1)
#define PAGE_ALIGN (~ADDR_ALIGN)
#define OFFS_ALIGN (PAGE_ALIGN & ~(1ul << (sizeof(_dl_pagesize) * 8 - 1)))

/* The union of reloc-type-classes where the reloc TYPE is a member.

   TYPE is in the class ELF_RTYPE_CLASS_NOCOPY if it should not be allowed
   to resolve to one of the main executable's symbols, as for a COPY
   reloc.  */
#define elf_machine_type_class(type)				\
  (((((type) == R_METAG_JMP_SLOT)) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_METAG_COPY) * ELF_RTYPE_CLASS_COPY))

static inline Elf32_Addr
elf_machine_dynamic(Elf32_Ehdr *header)
{
	Elf32_Addr *got;

	__asm__ ("MOV %0,A1LbP" : "=r" (got));

	if (header->e_ident[EI_ABIVERSION] >= 1) {
		/* GOT register offset was introduced with ABI v1 */
		got = (Elf32_Addr*)((void*)got - GOT_REG_OFFSET);
	}
	return *got;
}

#define DL_BOOT_COMPUTE_GOT(GOT) \
    ((GOT) = elf_machine_dynamic(header))

static inline Elf32_Addr
elf_machine_load_address(void)
{
	Elf32_Addr addr;
	__asm__ ("MOV  D1Ar1,A1LbP\n"
		 "ADDT D1Ar1,D1Ar1,#HI(__dl_start@GOTOFF)\n"
		 "ADD  D1Ar1,D1Ar1,#LO(__dl_start@GOTOFF)\n"
		 "ADDT D0Ar2,D0Ar2,#HI(__dl_start_addr@GOTOFF)\n"
		 "ADD  D0Ar2,D0Ar2,#LO(__dl_start_addr@GOTOFF)\n"
		 "GETD D0Ar2,[D0Ar2]\n"
		 "SUB  %0,D1Ar1,D0Ar2\n"
		 ".section .data\n"
		 "__dl_start_addr: .long __dl_start\n"
		 ".previous\n"
		 : "=d" (addr) : : "D1Ar1", "D0Ar2");
	return addr;
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

#define DL_MALLOC_ALIGN 8
