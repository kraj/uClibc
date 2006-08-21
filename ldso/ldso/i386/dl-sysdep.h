/* vi: set sw=4 ts=4: */
/*
 * Various assmbly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 */

/* Define this if the system uses RELOCA.  */
#undef ELF_USES_RELOCA
#include <elf.h>
/* Initialization sequence for the GOT.  */
#define INIT_GOT(GOT_BASE,MODULE)							\
do {														\
	GOT_BASE[2] = (unsigned long) _dl_linux_resolve;		\
	GOT_BASE[1] = (unsigned long) MODULE;					\
} while(0)

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_386
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "386"

struct elf_resolve;
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);

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
  ((((type) == R_386_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)			      \
   | (((type) == R_386_COPY) * ELF_RTYPE_CLASS_COPY))

/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static inline Elf32_Addr elf_machine_dynamic (void) attribute_unused;
static inline Elf32_Addr
elf_machine_dynamic (void)
{
	register Elf32_Addr *got __asm__ ("%ebx");
	return *got;
}


/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr elf_machine_load_address (void) attribute_unused;
static inline Elf32_Addr
elf_machine_load_address (void)
{
	/* It doesn't matter what variable this is, the reference never makes
	   it to assembly.  We need a dummy reference to some global variable
	   via the GOT to make sure the compiler initialized %ebx in time.  */
	extern int _dl_errno;
	Elf32_Addr addr;
	__asm__ ("leal _dl_start@GOTOFF(%%ebx), %0\n"
	     "subl _dl_start@GOT(%%ebx), %0"
	     : "=r" (addr) : "m" (_dl_errno) : "cc");
	return addr;
}

static inline void
elf_machine_relative (Elf32_Addr load_off, const Elf32_Addr rel_addr,
		      Elf32_Word relative_count)
{
	Elf32_Rel * rpnt = (void *) rel_addr;
	--rpnt;
	do {
		Elf32_Addr *const reloc_addr = (void *) (load_off + (++rpnt)->r_offset);

		*reloc_addr += load_off;
	} while (--relative_count);
}

/*
 * These were taken from the 'dl-sysdep.h' files in the 'nptl' directory
 * in glibc.
 */
#if USE_TLS
# ifdef CONFIG_686
/* Traditionally system calls have been made using int $0x80.  A
   second method was introduced which, if possible, will use the
   sysenter/syscall instructions.  To signal the presence and where to
   find the code the kernel passes an AT_SYSINFO value in the
   auxiliary vector to the application.  */
#  define NEED_DL_SYSINFO	1
#  define USE_DL_SYSINFO	1

#  if defined NEED_DL_SYSINFO && !defined __ASSEMBLER__
extern void _dl_sysinfo_int80 (void) attribute_hidden;
#   define DL_SYSINFO_DEFAULT (uintptr_t) _dl_sysinfo_int80
#   define DL_SYSINFO_IMPLEMENTATION \
  asm (".text\n\t"							      \
       ".type _dl_sysinfo_int80,@function\n\t"				      \
       ".hidden _dl_sysinfo_int80\n"					      \
       CFI_STARTPROC "\n"						      \
       "_dl_sysinfo_int80:\n\t"						      \
       "int $0x80;\n\t"							      \
       "ret;\n\t"							      \
       CFI_ENDPROC "\n"							      \
       ".size _dl_sysinfo_int80,.-_dl_sysinfo_int80\n\t"		      \
       ".previous");
#  endif
# else
/* Traditionally system calls have been made using int $0x80.  A
   second method was introduced which, if possible, will use the
   sysenter/syscall instructions.  To signal the presence and where to
   find the code the kernel passes an AT_SYSINFO value in the
   auxiliary vector to the application.
   sysenter/syscall is not useful on i386 through i586, but the dynamic
   linker and dl code in libc.a has to be able to load i686 compiled
   libraries.  */
#  define NEED_DL_SYSINFO	1
#  undef USE_DL_SYSINFO

#  if defined NEED_DL_SYSINFO && !defined __ASSEMBLER__
extern void _dl_sysinfo_int80 (void) attribute_hidden;
#  define DL_SYSINFO_DEFAULT (uintptr_t) _dl_sysinfo_int80
#  define DL_SYSINFO_IMPLEMENTATION \
  asm (".text\n\t"							      \
       ".type _dl_sysinfo_int80,@function\n\t"				      \
       ".hidden _dl_sysinfo_int80\n\t"					      \
       CFI_STARTPROC "\n"						      \
       "_dl_sysinfo_int80:\n\t"						      \
       "int $0x80;\n\t"							      \
       "ret;\n\t"							      \
       CFI_ENDPROC "\n"							      \
       ".size _dl_sysinfo_int80,.-_dl_sysinfo_int80\n\t"		      \
       ".previous;");
#  endif
# endif /* CONFIG_686 */
#endif /* USE_TLS */
