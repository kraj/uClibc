#ifndef LINUXELF_H
#define LINUXELF_H

#include <dl-sysdep.h> /* before elf.h to get ELF_USES_RELOCA right */
#include <elf.h>
#include <link.h>

#ifdef DEBUG
#  define LDSO_CONF  "../util/ld.so.conf"
#  define LDSO_CACHE "../util/ld.so.cache"
#  define LDSO_PRELOAD "../util/ld.so.preload"
#else
#  define LDSO_CONF  UCLIBC_RUNTIME_PREFIX "etc/ld.so.conf"
#  define LDSO_CACHE UCLIBC_RUNTIME_PREFIX "etc/ld.so.cache"
#  define LDSO_PRELOAD UCLIBC_RUNTIME_PREFIX "etc/ld.so.preload"
#endif


#define LIB_ANY	     -1
#define LIB_DLL       0
#define LIB_ELF       1
#define LIB_ELF64     0x80
#define LIB_ELF_LIBC5 2
#define LIB_ELF_LIBC6 3
#define LIB_ELF_LIBC0 4

/* Forward declarations for stuff defined in ld_hash.h */
struct dyn_elf;
struct elf_resolve;


/* Definitions and prototypes for cache stuff */
#ifdef USE_CACHE
extern int _dl_map_cache(void);
extern int _dl_unmap_cache(void);

#define LDSO_CACHE_MAGIC "ld.so-"
#define LDSO_CACHE_MAGIC_LEN (sizeof LDSO_CACHE_MAGIC -1)
#define LDSO_CACHE_VER "1.7.0"
#define LDSO_CACHE_VER_LEN (sizeof LDSO_CACHE_VER -1)

typedef struct {
	char magic   [LDSO_CACHE_MAGIC_LEN];
	char version [LDSO_CACHE_VER_LEN];
	int nlibs;
} header_t;

typedef struct {
	int flags;
	int sooffset;
	int liboffset;
} libentry_t;

#else
static inline void _dl_map_cache(void) { }
static inline void _dl_unmap_cache(void) { }
#endif


/* Function prototypes for non-static stuff in readelflib1.c */
extern int _dl_parse_copy_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size, int type);
extern void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size, int type);
extern int _dl_parse_relocation_information(struct dyn_elf *rpnt,
	unsigned long rel_addr, unsigned long rel_size, int type);
extern struct elf_resolve * _dl_load_shared_library(int secure,
	struct dyn_elf **rpnt, struct elf_resolve *tpnt, char *full_libname,
	int trace_loaded_objects);
extern struct elf_resolve * _dl_load_elf_shared_library(int secure,
	struct dyn_elf **rpnt, char *libname);
extern struct elf_resolve *_dl_check_if_named_library_is_loaded(const char *full_libname,
	int trace_loaded_objects);
extern int _dl_linux_resolve(void);
extern int _dl_fixup(struct dyn_elf *rpnt, int flag);


/*
 * Datatype of a relocation on this platform
 */
#ifdef ELF_USES_RELOCA
# define ELF_RELOC	ElfW(Rela)
# define DT_RELOC_TABLE_ADDR	DT_RELA
# define DT_RELOC_TABLE_SIZE	DT_RELASZ
# define UNSUPPORTED_RELOC_TYPE	DT_REL
# define UNSUPPORTED_RELOC_STR	"REL"
#else
# define ELF_RELOC	ElfW(Rel)
# define DT_RELOC_TABLE_ADDR	DT_REL
# define DT_RELOC_TABLE_SIZE	DT_RELSZ
# define UNSUPPORTED_RELOC_TYPE	DT_RELA
# define UNSUPPORTED_RELOC_STR	"RELA"
#endif

/* Reloc type classes as returned by elf_machine_type_class().
   ELF_RTYPE_CLASS_PLT means this reloc should not be satisfied by
   some PLT symbol, ELF_RTYPE_CLASS_COPY means this reloc should not be
   satisfied by any symbol in the executable.  Some architectures do
   not support copy relocations.  In this case we define the macro to
   zero so that the code for handling them gets automatically optimized
   out.  */
#define ELF_RTYPE_CLASS_PLT 1
#ifndef DL_NO_COPY_RELOCS
# define ELF_RTYPE_CLASS_COPY 2
#else
# define ELF_RTYPE_CLASS_COPY 0
#endif


/* Convert between the Linux flags for page protections and the
   ones specified in the ELF standard. */
#define LXFLAGS(X) ( (((X) & PF_R) ? PROT_READ : 0) | \
		    (((X) & PF_W) ? PROT_WRITE : 0) | \
		    (((X) & PF_X) ? PROT_EXEC : 0))


#endif	/* LINUXELF_H */
