#include "link.h"

#ifndef RTLD_NEXT
#define RTLD_NEXT	((void*)-1)
#endif

struct dyn_elf{
  unsigned long flags;
  struct elf_resolve * dyn;
  struct dyn_elf * next_handle;  /* Used by dlopen et al. */
  struct dyn_elf * next;
};
 
struct elf_resolve{
  /* These entries must be in this order to be compatible with the interface used
     by gdb to obtain the list of symbols. */
  char * loadaddr;
  char * libname;
  unsigned long dynamic_addr;
  struct elf_resolve * next;
  struct elf_resolve * prev;
  /* Nothing after this address is used by gdb. */
  enum {elf_lib, elf_executable,program_interpreter, loaded_file} libtype;
  struct dyn_elf * symbol_scope;
  unsigned short usage_count;
  unsigned short int init_flag;
  unsigned int nbucket;
  unsigned long * elf_buckets;
  /*
   * These are only used with ELF style shared libraries
   */
  unsigned long nchain;
  unsigned long * chains;
  unsigned long dynamic_info[24];

  unsigned long dynamic_size;
  unsigned long n_phent;
  Elf32_Phdr * ppnt;
};

#if 0
/*
 * The DT_DEBUG entry in the .dynamic section is given the address of this structure.
 * gdb can pick this up to obtain the correct list of loaded modules.
 */

struct r_debug{
  int r_version;
  struct elf_resolve * link_map;
  unsigned long brk_fun;
  enum {RT_CONSISTENT, RT_ADD, RT_DELETE};
  unsigned long ldbase;
};
#endif

#define COPY_RELOCS_DONE 1
#define RELOCS_DONE 2
#define JMP_RELOCS_DONE 4
#define INIT_FUNCS_CALLED 8

extern struct dyn_elf     * _dl_symbol_tables;
extern struct elf_resolve * _dl_loaded_modules;
extern struct dyn_elf 	  * _dl_handles;

extern struct elf_resolve * _dl_check_hashed_files(char * libname);
extern struct elf_resolve * _dl_add_elf_hash_table(char * libname, 
	char * loadaddr, unsigned long * dynamic_info, 
	unsigned long dynamic_addr, unsigned long dynamic_size);
extern char * _dl_find_hash(char * name, struct dyn_elf * rpnt1, 
	unsigned long instr_addr, struct elf_resolve * f_tpnt, 
	int copyrel);
extern int _dl_linux_dynamic_link(void);

#ifdef __mc68000__
/* On m68k constant strings are referenced through the GOT. */
/* XXX Requires load_addr to be defined. */
#define SEND_STDERR(X)				\
  { const char *__s = (X);			\
    if (__s < (const char *) load_addr) __s += load_addr;	\
    _dl_write (2, __s, _dl_strlen_inline (__s));	\
  }
#else
#define SEND_STDERR(X) _dl_write(2, X, _dl_strlen_inline(X));
#endif

#define SEND_ADDRESS_STDERR(X, add_a_newline) { \
    char tmp[13]; \
    _dl_write(2, _dl_simple_ltoahex_inline( tmp, (unsigned long)(X)), 12); \
    if (add_a_newline) { \
	tmp[0]='\n'; \
	_dl_write(2, tmp, 1); \
    } \
};    

#define SEND_NUMBER_STDERR(X, add_a_newline) { \
    char tmp[13]; \
    _dl_write(2, (void *)_dl_simple_ltoahex_inline( tmp, (unsigned long)(X)), 12); \
    if (add_a_newline) { \
	tmp[0]='\n'; \
	_dl_write(2, tmp, 1); \
    } \
};    
extern int _dl_fdprintf(int, const char *, ...);
extern char * _dl_library_path;
extern char * _dl_not_lazy;
extern char * _dl_strdup(const char *);
extern unsigned long _dl_elf_hash(const char * name);

static inline int _dl_symbol(char * name)
{
  if(name[0] != '_' || name[1] != 'd' || name[2] != 'l' || name[3] != '_')
    return 0;
  return 1;
}


#define DL_ERROR_NOFILE 1
#define DL_ERROR_NOZERO 2
#define DL_ERROR_NOTELF 3
#define DL_ERROR_NOTMAGIC 4
#define DL_ERROR_NOTDYN 5
#define DL_ERROR_MMAP_FAILED 6
#define DL_ERROR_NODYNAMIC 7
#define DL_WRONG_RELOCS 8
#define DL_BAD_HANDLE 9
#define DL_NO_SYMBOL 10

