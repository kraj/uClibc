/*
 * Various assembly language/system dependent hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA
#include <elf.h>

#ifndef __SH_FDPIC__
/*
 * Initialization sequence for a GOT.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  GOT_BASE[2] = (unsigned long) _dl_linux_resolve; \
  GOT_BASE[1] = (unsigned long) (MODULE); \
}
#else
/*
 * Initialization sequence for a GOT.  Copy the resolver function
 * descriptor and the pointer to the elf_resolve/link_map data
 * structure.  Initialize the got_value in the module while at that.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  (MODULE)->loadaddr.got_value = (GOT_BASE); \
  GOT_BASE[0] = ((unsigned long *)&_dl_linux_resolve)[0]; \
  GOT_BASE[1] = ((unsigned long *)&_dl_linux_resolve)[1]; \
  GOT_BASE[2] = (unsigned long) MODULE; \
}

/* Make sure the dynamic linker GOT is initialized.
   FIXME: Is this the right solution? No other target seems to need it????  */
#define RERELOCATE_LDSO 1
#endif

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_SH
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "sh"

struct elf_resolve;
#ifdef __SH_FDPIC__
struct funcdesc_value;
extern struct funcdesc_value volatile * _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);
#else
extern unsigned long _dl_linux_resolver(struct elf_resolve * tpnt, int reloc_entry);
#endif

#ifdef __SH_FDPIC__
struct funcdesc_value
{
  void *entry_point;
  void *got_value;
} __attribute__((__aligned__(8)));

#define ARCH_NEEDS_BOOTSTRAP_RELOCS
#define ARCH_CANNOT_PROTECT_MEMORY

#define HAVE_DL_INLINES_H

#define STRINGIFY1(s) #s
#define STRINGIFY(s) STRINGIFY1 (s)
#undef SEND_EARLY_STDERR
#define SEND_EARLY_STDERR(S) \
    do {								\
	const char *__s;						\
	int len;							\
	__asm__("mova	1f, r0\n"					\
	    "	bra	2f\n"						\
	    "	  nop\n"						\
	    "	.align 2\n"						\
	    "1:	.string " STRINGIFY (S) "\n"				\
	    "	.align 2\n"						\
	    "2:"							\
	    : "=z" (__s));						\
	for (len = 0; __s[len] != '\0'; len++)				\
	  ;								\
	_dl_write (2, __s, len);					\
    } while (0)

#define DL_LOADADDR_TYPE struct elf32_fdpic_loadaddr

#define DL_RELOC_ADDR(LOADADDR, ADDR) \
  (__reloc_pointer ((void*)(ADDR), (LOADADDR).map))

#define DL_ADDR_TO_FUNC_PTR(ADDR, LOADADDR) \
  ((void(*)(void)) _dl_funcdesc_for ((void*)(ADDR), (LOADADDR).got_value))

#define _dl_stabilize_funcdesc(val) \
  ({ __asm__ ("" : "+m" (*(val))); (val); })

#define DL_CALL_FUNC_AT_ADDR(ADDR, LOADADDR, SIGNATURE, ...) \
  ({ struct funcdesc_value fd = { (void*)(ADDR), (LOADADDR).got_value }; \
     void (*pf)(void) = (void*) _dl_stabilize_funcdesc (&fd); \
     (* SIGNATURE pf)(__VA_ARGS__); })

/* Get an absolute address by relative means.  */
#define DL_GET_ADDRESS(SYM) \
  ({							\
    void *val;						\
    __asm__ (						\
       "mova	1f, r0\n"				\
    "	mov.l	1f, r1\n"				\
    "	bra	2f\n"					\
    "	  add	r1, r0\n"				\
    "	.align 2\n"					\
    "1:	.long " STRINGIFY(SYM) "@PCREL\n"		\
    "2:\n"					\
    : "=z" (val) : : "r1");				\
    val;						\
  })

#define DL_CALL_FUNC_PCREL(FUNC, LOADADDR, ...)		\
    DL_CALL_FUNC_AT_ADDR (DL_GET_ADDRESS (FUNC),	\
			  LOADADDR,			\
			  (__typeof(FUNC) *),		\
			  __VA_ARGS__)

#define DL_INIT_LOADADDR_BOOT(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), dl_boot_got_pointer, \
			   dl_boot_ldsomap ?: dl_boot_progmap))

#define DL_INIT_LOADADDR_PROG(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), 0, dl_boot_progmap))

/* The GOT address is not at a fixed offset in FDPIC.
   The address is computed by __sefl_reloc, and passed into _dl_start
   as a function argument, dl_boot_got_pointer.  */
#define DL_BOOT_COMPUTE_GOT(got) ((got) = dl_boot_got_pointer)

/* The _DYNAMIC address is not at a fixed offset or referenced in
   the GOT.  The kernel passes it in R10, and it is passed to
   _dl_start as an argument, dl_boot_ldso_dyn_pointer.  */
#define DL_BOOT_COMPUTE_DYN(DPNT, GOT, LOAD_ADDR) \
  ((DPNT) = dl_boot_ldso_dyn_pointer)

#define DL_PARSE_DYNAMIC_INFO(DPNT, DYNAMIC_INFO, DEBUG_ADDR, LOAD_OFF) \
    DL_CALL_FUNC_PCREL (_dl_parse_dynamic_info,				\
			LOAD_OFF,					\
			DPNT, DYNAMIC_INFO, DEBUG_ADDR, LOAD_OFF)

#define DL_INIT_LOADADDR_EXTRA_DECLS \
  int dl_init_loadaddr_load_count;
#define DL_INIT_LOADADDR(LOADADDR, BASEADDR, PHDR, PHDRCNT) \
  (dl_init_loadaddr_load_count = \
     __dl_init_loadaddr (&(LOADADDR), (PHDR), (PHDRCNT)))
#define DL_INIT_LOADADDR_HDR(LOADADDR, ADDR, PHDR) \
  (__dl_init_loadaddr_hdr ((LOADADDR), (ADDR), (PHDR), \
			   dl_init_loadaddr_load_count))
#define DL_LOADADDR_UNMAP(LOADADDR, LEN) \
  (__dl_loadaddr_unmap ((LOADADDR), (NULL)))
#define DL_LIB_UNMAP(LIB, LEN) \
  (__dl_loadaddr_unmap ((LIB)->loadaddr, (LIB)->funcdesc_ht))
#define DL_LOADADDR_BASE(LOADADDR) \
  ((LOADADDR).map->segs[0].addr)

/* This is called from dladdr(), such that we map a function
   descriptor's address to the function's entry point before trying to
   find in which library it's defined.  */
#define DL_LOOKUP_ADDRESS(ADDRESS) (_dl_lookup_address (ADDRESS))

#define DL_ADDR_IN_LOADADDR(ADDR, TPNT, TFROM) \
  (! (TFROM) && __dl_addr_in_loadaddr ((void*)(ADDR), (TPNT)->loadaddr))

/* We only support loading FDPIC independently-relocatable shared
   libraries.  It probably wouldn't be too hard to support loading
   shared libraries that require relocation by the same amount, but we
   don't know that they exist or would be useful, and the dynamic
   loader code could leak the whole-library map unless we keeping a
   bit more state for DL_LOADADDR_UNMAP and DL_LIB_UNMAP, so let's
   keep things simple for now.  */
#define DL_CHECK_LIB_TYPE(epnt, piclib, _dl_progname, libname) \
do \
{ \
   if (((epnt)->e_flags & EF_SH_FDPIC) && ((epnt)->e_flags & EF_SH_PIC)) \
    (piclib) = 2; \
  else \
    { \
      _dl_internal_error_number = LD_ERROR_NOTDYN; \
      _dl_dprintf(2, "%s: '%s' is not an FDPIC shared library" \
		  "\n", (_dl_progname), (libname)); \
      _dl_close(infile); \
      return NULL; \
    } \
} \
while (0)

/* We want want to apply all relocations in the interpreter during
   bootstrap.  Because of this, we have to skip the interpreter
   relocations in _dl_parse_relocation_information(), see
   elfinterp.c.  */
#define DL_SKIP_BOOTSTRAP_RELOC(SYMTAB, INDEX, STRTAB) 0

#ifdef __NR_pread
#define _DL_PREAD(FD, BUF, SIZE, OFFSET) \
  (_dl_pread((FD), (BUF), (SIZE), (OFFSET)))
#else
# error "pread is required for FDPIC"
#endif

/* We want to return to dlsym() a function descriptor if the symbol
   turns out to be a function.  */
#define DL_FIND_HASH_VALUE(TPNT, TYPE_CLASS, SYM) \
  (((TYPE_CLASS) & ELF_RTYPE_CLASS_DLSYM) \
   && ELF32_ST_TYPE((SYM)->st_info) == STT_FUNC \
   ? _dl_funcdesc_for (DL_RELOC_ADDR ((TPNT)->loadaddr, (SYM)->st_value),    \
 		       (TPNT)->loadaddr.got_value)			     \
   : DL_RELOC_ADDR ((TPNT)->loadaddr, (SYM)->st_value))

#define DL_GET_READY_TO_RUN_EXTRA_PARMS \
  , struct elf32_fdpic_loadmap *dl_boot_progmap
#define DL_GET_READY_TO_RUN_EXTRA_ARGS \
  , dl_boot_progmap

#else /* !__SH_FDPIC__  */
#include <link.h>
#define DL_LOADADDR_TYPE ElfW(Addr)
#endif

static __always_inline unsigned int
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

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry or
   TLS variable, so undefined references should not be allowed to
   define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_SH_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_SH_COPY) * ELF_RTYPE_CLASS_COPY))

#ifndef __SH_FDPIC__
/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static __always_inline Elf32_Addr __attribute__ ((unused))
elf_machine_dynamic (void)
{
	register Elf32_Addr *got;
	__asm__ ("mov r12,%0" :"=r" (got));
	return *got;
}
#endif

#ifndef __SH_FDPIC__
/* Return the run-time load address of the shared object.  */
static __always_inline Elf32_Addr __attribute__ ((unused))
elf_machine_load_address (void)
{
	Elf32_Addr addr;
	__asm__ ("mov.l 1f,r0\n\
        mov.l 3f,r2\n\
        add r12,r2\n\
        mov.l @(r0,r12),r0\n\
        bra 2f\n\
         sub r0,r2\n\
        .align 2\n\
        1: .long _dl_start@GOT\n\
        3: .long _dl_start@GOTOFF\n\
        2: mov r2,%0"
	     : "=r" (addr) : : "r0", "r1", "r2");
	return addr;
}
#else
/* FDPIC needs access to the loadmap for this.  */
#include <bits/elf-fdpic.h>

#define elf_machine_load_address() \
		elf_machine_load_address1 (dl_boot_ldsomap, dl_boot_progmap)
static __always_inline Elf32_Addr __attribute__ ((unused))
elf_machine_load_address1 (struct elf32_fdpic_loadmap *ldso_map,
			   struct elf32_fdpic_loadmap *exec_map)
{
  struct elf32_fdpic_loadmap *map = (ldso_map ?: exec_map);

  /* The ELF header is assumed to be the first entry in the load map.  */
  return map->segs[0].addr;
}
#endif

#define COPY_UNALIGNED_WORD(swp, twp, align) \
  { \
    void *__s = (swp), *__t = (twp); \
    unsigned char *__s1 = __s, *__t1 = __t; \
    unsigned short *__s2 = __s, *__t2 = __t; \
    unsigned long *__s4 = __s, *__t4 = __t; \
    switch ((align)) \
    { \
    case 0: \
      *__t4 = *__s4; \
      break; \
    case 2: \
      *__t2++ = *__s2++; \
      *__t2 = *__s2; \
      break; \
    default: \
      *__t1++ = *__s1++; \
      *__t1++ = *__s1++; \
      *__t1++ = *__s1++; \
      *__t1 = *__s1; \
      break; \
    } \
  }

static __always_inline void
elf_machine_relative (DL_LOADADDR_TYPE load_off, const Elf32_Addr rel_addr,
		      Elf32_Word relative_count)
{
#ifndef __SH_FDPIC__
	Elf32_Addr value;
	Elf32_Rela * rpnt = (void *)rel_addr;

	do {
		Elf32_Addr *const reloc_addr = (void *) (load_off + rpnt->r_offset);

		if (rpnt->r_addend)
			value = load_off + rpnt->r_addend;
		else {
			COPY_UNALIGNED_WORD (reloc_addr, &value, (int) reloc_addr & 3);
			value += load_off;
		}
		COPY_UNALIGNED_WORD (&value, reloc_addr, (int) reloc_addr & 3);
		rpnt++;
	} while (--relative_count);
#endif
#undef COPY_UNALIGNED_WORD
}

