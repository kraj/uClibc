/* vi: set sw=4 ts=4: */
/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 */

/* Define this if the system uses RELOCA.  */
#undef ELF_USES_RELOCA

/* Initialization sequence for the GOT.  */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  GOT_BASE[2] = (unsigned long) _dl_linux_resolve; \
  GOT_BASE[1] = (unsigned long) MODULE; \
}

static inline unsigned long arm_modulus(unsigned long m, unsigned long p)
{
	unsigned long i,t,inc;
	i=p; t=0;
	while(!(i&(1<<31))) {
		i<<=1;
		t++;
	}
	t--;
	for(inc=t;inc>2;inc--) {
		i=p<<inc;
		if(i&(1<<31))
			break;
		while(m>=i) {
			m-=i;
			i<<=1;
			if(i&(1<<31))
				break;
			if(i<p)
				break;
		}
	}
	while(m>=p) {
		m-=p;
	}
	return m;
}
#define do_rem(result, n, base) ((result) = arm_modulus(n, base))

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_ARM
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "ARM"

struct elf_resolve;
unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);

/* 4096 bytes alignment */
#define PAGE_ALIGN 0xfffff000
#define ADDR_ALIGN 0xfff
#define OFFS_ALIGN 0x7ffff000

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_ARM_JUMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_ARM_COPY) * ELF_RTYPE_CLASS_COPY))
