#ifndef LINUXELF_H
#define LINUXELF_H

#include <dl-string.h> /* before elf.h to get ELF_USES_RELOCA right */
#include <elf.h>
#include <link.h>

/* Forward declarations for stuff defined in ld_hash.h */
struct dyn_elf;
struct elf_resolve;

#include <dl-defs.h>
#ifdef __LDSO_CACHE_SUPPORT__
extern int _dl_map_cache(void);
extern int _dl_unmap_cache(void);
#else
static inline void _dl_map_cache(void) { }
static inline void _dl_unmap_cache(void) { }
#endif


/* Function prototypes for non-static stuff in readelflib1.c */
extern void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size);
extern int _dl_parse_relocation_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size);
extern struct elf_resolve * _dl_load_shared_library(int secure,
	struct dyn_elf **rpnt, struct elf_resolve *tpnt, char *full_libname,
	int trace_loaded_objects);
extern struct elf_resolve * _dl_load_elf_shared_library(int secure,
	struct dyn_elf **rpnt, char *libname);
extern struct elf_resolve *_dl_check_if_named_library_is_loaded(const char *full_libname,
	int trace_loaded_objects);
extern int _dl_linux_resolve(void);
extern int _dl_fixup(struct dyn_elf *rpnt, int flag);
extern void _dl_protect_relro (struct elf_resolve *l);

/*
 * Datatype of a relocation on this platform
 */
#ifdef ELF_USES_RELOCA
# define ELF_RELOC	ElfW(Rela)
# define DT_RELOC_TABLE_ADDR	DT_RELA
# define DT_RELOC_TABLE_SIZE	DT_RELASZ
# define DT_RELOCCOUNT		DT_RELACOUNT
# define UNSUPPORTED_RELOC_TYPE	DT_REL
# define UNSUPPORTED_RELOC_STR	"REL"
#else
# define ELF_RELOC	ElfW(Rel)
# define DT_RELOC_TABLE_ADDR	DT_REL
# define DT_RELOC_TABLE_SIZE	DT_RELSZ
# define DT_RELOCCOUNT		DT_RELCOUNT
# define UNSUPPORTED_RELOC_TYPE	DT_RELA
# define UNSUPPORTED_RELOC_STR	"RELA"
#endif

/* OS and/or GNU dynamic extensions */
#define OS_NUM 1
#define DT_RELCONT_IDX DT_NUM

#ifndef ARCH_DYNAMIC_INFO
  /* define in arch specific code, if needed */
# define ARCH_NUM 0
#endif

#define DYNAMIC_SIZE (DT_NUM+OS_NUM+ARCH_NUM)

extern void _dl_parse_dynamic_info(Elf32_Dyn *dpnt, unsigned long dynamic_info[], void *debug_addr);

static inline __attribute__((always_inline))
void __dl_parse_dynamic_info(Elf32_Dyn *dpnt, unsigned long dynamic_info[], void *debug_addr)
{
	for (; dpnt->d_tag; dpnt++) {
		if (dpnt->d_tag < DT_NUM) {
			dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
#ifndef __mips__
			if (dpnt->d_tag == DT_DEBUG)
				dpnt->d_un.d_val = (unsigned long)debug_addr;
#endif
			if (dpnt->d_tag == DT_BIND_NOW)
				dynamic_info[DT_BIND_NOW] = 1;
			if (dpnt->d_tag == DT_FLAGS &&
			    (dpnt->d_un.d_val & DF_BIND_NOW))
				dynamic_info[DT_BIND_NOW] = 1;
			if (dpnt->d_tag == DT_TEXTREL)
				dynamic_info[DT_TEXTREL] = 1;
		} else if (dpnt->d_tag < DT_LOPROC) {
			if (dpnt->d_tag == DT_RELOCCOUNT)
				dynamic_info[DT_RELCONT_IDX] = dpnt->d_un.d_val;
			if (dpnt->d_tag == DT_FLAGS_1 &&
			    (dpnt->d_un.d_val & DF_1_NOW))
				dynamic_info[DT_BIND_NOW] = 1;
		}
#ifdef ARCH_DYNAMIC_INFO
		else {
			ARCH_DYNAMIC_INFO(dpnt, dynamic_info, debug_addr);
		}
#endif
	}
}

/* Reloc type classes as returned by elf_machine_type_class().
   ELF_RTYPE_CLASS_PLT means this reloc should not be satisfied by
   some PLT symbol, ELF_RTYPE_CLASS_COPY means this reloc should not be
   satisfied by any symbol in the executable.  Some architectures do
   not support copy relocations.  In this case we define the macro to
   zero so that the code for handling them gets automatically optimized
   out.  */
#ifdef DL_NO_COPY_RELOCS
# define ELF_RTYPE_CLASS_COPY	(0x0)
#else
# define ELF_RTYPE_CLASS_COPY	(0x2)
#endif
#define ELF_RTYPE_CLASS_PLT	(0x1)


/* Convert between the Linux flags for page protections and the
   ones specified in the ELF standard. */
#define LXFLAGS(X) ( (((X) & PF_R) ? PROT_READ : 0) | \
		    (((X) & PF_W) ? PROT_WRITE : 0) | \
		    (((X) & PF_X) ? PROT_EXEC : 0))


#endif	/* LINUXELF_H */
