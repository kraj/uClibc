#ifndef _LDSODEFS_H
#define _LDSODEFS_H

#include <features.h>
#include <stdbool.h>
#include <elf.h>
#include <link.h>
#include <bits/kernel-features.h>

#include <dl-defs.h>
#ifdef __UCLIBC_HAS_TLS__
# include <tls.h>
#endif

/* OS and/or GNU dynamic extensions */
#ifdef __LDSO_GNU_HASH_SUPPORT__
# define OS_NUM 2 /* for DT_RELOCCOUNT and DT_GNU_HASH entries */
#else
# define OS_NUM 1 /* for DT_RELOCCOUNT entry */
#endif

#ifndef ARCH_DYNAMIC_INFO
  /* define in arch specific code, if needed */
# define ARCH_NUM 0
#endif

#define DYNAMIC_SIZE (DT_NUM+OS_NUM+ARCH_NUM)

struct elf_resolve;
struct init_fini {
	struct elf_resolve **init_fini;
	unsigned long nlist; /* Number of entries in init_fini */
};

/* For INIT/FINI dependency sorting. */
struct init_fini_list {
	struct init_fini_list *next;
	struct elf_resolve *tpnt;
};

struct dyn_elf {
  struct elf_resolve * dyn;
  struct dyn_elf * next_handle;  /* Used by dlopen et al. */
  struct init_fini init_fini;
  struct dyn_elf * next;
  struct dyn_elf * prev;
};

struct symbol_ref {
  const ElfW(Sym) *sym;
  struct elf_resolve *tpnt;
};

struct elf_resolve {
  /* These entries must be in this order to be compatible with the interface used
     by gdb to obtain the list of symbols. */
  DL_LOADADDR_TYPE loadaddr;	/* Base address shared object is loaded at.  */
  char *libname;		/* Absolute file name object was found in.  */
  ElfW(Dyn) *dynamic_addr;	/* Dynamic section of the shared object.  */
  struct elf_resolve * next;
  struct elf_resolve * prev;
  /* Nothing after this address is used by gdb. */

#ifdef __UCLIBC_HAS_TLS__
  /* Thread-local storage related info.  */

  /* Start of the initialization image.  */
  void *l_tls_initimage;
  /* Size of the initialization image.  */
  size_t l_tls_initimage_size;
  /* Size of the TLS block.  */
  size_t l_tls_blocksize;
  /* Alignment requirement of the TLS block.  */
  size_t l_tls_align;
  /* Offset of first byte module alignment.  */
  size_t l_tls_firstbyte_offset;
# ifndef NO_TLS_OFFSET
#  define NO_TLS_OFFSET	0
# endif
  /* For objects present at startup time: offset in the static TLS block.  */
  ptrdiff_t l_tls_offset;
  /* Index of the module in the dtv array.  */
  size_t l_tls_modid;
  /* Nonzero if _dl_init_static_tls should be called for this module */
  unsigned int l_need_tls_init:1;
#endif

  ElfW(Addr) mapaddr;
  enum {elf_lib, elf_executable,program_interpreter, loaded_file} libtype;
  struct dyn_elf * symbol_scope;
  unsigned short usage_count;
  unsigned short int init_flag;
  unsigned long rtld_flags; /* RTLD_GLOBAL, RTLD_NOW etc. */
  Elf_Symndx nbucket;

#ifdef __LDSO_GNU_HASH_SUPPORT__
  /* Data needed to support GNU hash style */
  Elf32_Word l_gnu_bitmask_idxbits;
  Elf32_Word l_gnu_shift;
  const ElfW(Addr) *l_gnu_bitmask;

  union
  {
    const Elf32_Word *l_gnu_chain_zero;
    const Elf_Symndx *elf_buckets;
  };
#else
  Elf_Symndx *elf_buckets;
#endif

  struct init_fini_list *init_fini;
  struct init_fini_list *rtld_local; /* keep tack of RTLD_LOCAL libs in same group */
  /*
   * These are only used with ELF style shared libraries
   */
  Elf_Symndx nchain;

#ifdef __LDSO_GNU_HASH_SUPPORT__
  union
  {
    const Elf32_Word *l_gnu_buckets;
    const Elf_Symndx *chains;
  };
#else
  Elf_Symndx *chains;
#endif
  ElfW(Word) dynamic_info[DYNAMIC_SIZE];

  unsigned long n_phent;
  ElfW(Phdr) * ppnt;

  ElfW(Addr) relro_addr;
  size_t relro_size;

  dev_t st_dev;      /* device */
  ino_t st_ino;      /* inode */

#ifdef __powerpc__
  /* this is used to store the address of relocation data words, so
   * we don't have to calculate it every time, which requires a divide */
  unsigned long data_words;
#endif

#ifdef __FDPIC__
  /* Every loaded module holds a hashtable of function descriptors of
     functions defined in it, such that it's easy to release the
     memory when the module is dlclose()d.  */
  struct funcdesc_ht *funcdesc_ht;
#endif
};

#ifdef __mips__
/* The MIPS ABI specifies that the dynamic section has to be read-only.  */

#define DL_RO_DYN_SECTION 1

/* TODO: Import in 64-bit relocations from glibc. */
#endif

#ifndef SHARED
# define EXTERN extern
#else
# ifdef IS_IN_rtld
#  define EXTERN
# else
#  define EXTERN extern
# endif
#endif

/* Non-shared code has no support for multiple namespaces.  */
#ifdef SHARED
# define DL_NNS 16
#else
# define DL_NNS 1
#endif

#define GL(x) _##x
#define GLRO(x) _##x

/* Variable pointing to the end of the stack (or close to it).  This value
   must be constant over the runtime of the application.  Some programs
   might use the variable which results in copy relocations on some
   platforms.  But this does not matter, ld.so can always use the local
   copy.  */
extern void *__libc_stack_end attribute_relro;

#ifdef SHARED
extern unsigned long _dl_error_number;
extern char *_dl_debug;
extern struct r_debug *_dl_debug_addr;
extern void *(*_dl_malloc_function)(size_t);
extern void (*_dl_free_function) (void *p);
struct elf_resolve;
# ifdef __UCLIBC_CTOR_DTOR__
extern void _dl_app_init_array(void);
extern void _dl_app_fini_array(void);
# endif
#endif
struct elf_resolve;
extern void _dl_run_init_array(struct elf_resolve *);
extern void _dl_run_fini_array(struct elf_resolve *);

extern struct dyn_elf     * _dl_symbol_tables;
extern struct elf_resolve * _dl_loaded_modules;

extern char *_dl_find_hash(const char *name, struct dyn_elf *rpnt,
		struct elf_resolve *mytpnt, int type_class,
		struct symbol_ref *symbol);

#ifdef __LDSO_CACHE_SUPPORT__
extern int _dl_map_cache(void);
extern int _dl_unmap_cache(void);
#else
static __inline__ void _dl_map_cache(void) { }
static __inline__ void _dl_unmap_cache(void) { }
#endif

/* Function prototypes for non-static stuff in dl-elf.c and elfinterp.c */
extern struct elf_resolve * _dl_load_shared_library(int secure,
	struct dyn_elf **rpnt, struct elf_resolve *tpnt, char *full_libname,
	int trace_loaded_objects);
extern int _dl_fixup(struct dyn_elf *rpnt, int flag);
extern void _dl_protect_relro (struct elf_resolve *l) internal_function;

#ifdef __UCLIBC_HAS_TLS__
/* Determine next available module ID.  */
extern size_t _dl_next_tls_modid (void) internal_function attribute_hidden;

/* Calculate offset of the TLS blocks in the static TLS block.  */
extern void _dl_determine_tlsoffset (void) internal_function attribute_hidden;

/* Set up the data structures for TLS, when they were not set up at startup.
   Returns nonzero on malloc failure.
   This is called from _dl_map_object_from_fd or by libpthread.  */
extern int _dl_tls_setup (void) internal_function;
rtld_hidden_proto (_dl_tls_setup)

/* Allocate memory for static TLS block (unless MEM is nonzero) and dtv.  */
extern void *_dl_allocate_tls (void *mem) internal_function;

/* Get size and alignment requirements of the static TLS block.  */
extern void _dl_get_tls_static_info (size_t *sizep, size_t *alignp)
     internal_function;

extern void _dl_allocate_static_tls (struct link_map *map)
     internal_function attribute_hidden;

/* Taken from glibc/elf/dl-reloc.c */
#define CHECK_STATIC_TLS(sym_map)											\
	do {																	\
		if (__builtin_expect ((sym_map)->l_tls_offset == NO_TLS_OFFSET, 0))	\
			_dl_allocate_static_tls (sym_map);								\
	} while (0)

/* These are internal entry points to the two halves of _dl_allocate_tls,
   only used within rtld.c itself at startup time.  */
extern void *_dl_allocate_tls_storage (void)
     internal_function attribute_hidden;
extern void *_dl_allocate_tls_init (void *) internal_function;

/* Deallocate memory allocated with _dl_allocate_tls.  */
extern void _dl_deallocate_tls (void *tcb, bool dealloc_tcb) internal_function;

extern void _dl_nothread_init_static_tls (struct link_map *) attribute_hidden;

/* Highest dtv index currently needed.  */
EXTERN size_t _dl_tls_max_dtv_idx;
/* Flag signalling whether there are gaps in the module ID allocation.  */
EXTERN bool _dl_tls_dtv_gaps;
/* Information about the dtv slots.  */
EXTERN struct dtv_slotinfo_list
{
  size_t len;
  struct dtv_slotinfo_list *next;
  struct dtv_slotinfo
  {
    size_t gen;
    bool is_static;
    struct link_map *map;
  } slotinfo[0];
} *_dl_tls_dtv_slotinfo_list;
/* Number of modules in the static TLS block.  */
EXTERN size_t _dl_tls_static_nelem;
/* Size of the static TLS block.  */
EXTERN size_t _dl_tls_static_size;
/* Size actually allocated in the static TLS block.  */
EXTERN size_t _dl_tls_static_used;
/* Alignment requirement of the static TLS block.  */
EXTERN size_t _dl_tls_static_align;
/* Function pointer for catching TLS errors.  */
EXTERN void **(*_dl_error_catch_tsd) (void) __attribute__ ((const));

/* Number of additional entries in the slotinfo array of each slotinfo
   list element.  A large number makes it almost certain take we never
   have to iterate beyond the first element in the slotinfo list.  */
# define TLS_SLOTINFO_SURPLUS (62)

/* Number of additional slots in the dtv allocated.  */
# define DTV_SURPLUS	(14)

/* Initial dtv of the main thread, not allocated with normal malloc.  */
EXTERN void *_dl_initial_dtv;
/* Generation counter for the dtv.  */
EXTERN size_t _dl_tls_generation;

EXTERN void (*_dl_init_static_tls) (struct link_map *);
#endif

/* We have the auxiliary vector.  */
#define HAVE_AUX_VECTOR

/* We can assume that the kernel always provides the AT_UID, AT_EUID,
   AT_GID, and AT_EGID values in the auxiliary vector from 2.4.0 or so on.  */
#if __ASSUME_AT_XID
# define HAVE_AUX_XID
#endif

/* We can assume that the kernel always provides the AT_SECURE value
   in the auxiliary vector from 2.5.74 or so on.  */
#if __ASSUME_AT_SECURE
# define HAVE_AUX_SECURE
#endif

/* Starting with one of the 2.4.0 pre-releases the Linux kernel passes
   up the page size information.  */
#if __ASSUME_AT_PAGESIZE
# define HAVE_AUX_PAGESIZE
#endif

#endif
