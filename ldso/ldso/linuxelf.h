/* This should eventually appear in the distribution version of linux/elf.h */
#ifndef R_SPARC_NONE
#define R_SPARC_NONE		0
#define R_SPARC_8		1
#define R_SPARC_16		2
#define R_SPARC_32		3
#define R_SPARC_DISP8		4
#define R_SPARC_DISP16		5
#define R_SPARC_DISP32		6
#define R_SPARC_WDISP30		7
#define R_SPARC_WDISP22		8
#define R_SPARC_HI22		9
#define R_SPARC_22		10
#define R_SPARC_13		11
#define R_SPARC_LO10		12
#define R_SPARC_GOT10		13
#define R_SPARC_GOT13		14
#define R_SPARC_GOT22		15
#define R_SPARC_PC10		16
#define R_SPARC_PC22		17
#define R_SPARC_WPLT30		18
#define R_SPARC_COPY		19
#define R_SPARC_GLOB_DAT	20
#define R_SPARC_JMP_SLOT	21
#define R_SPARC_RELATIVE	22
#define R_SPARC_UA32		23
#endif

#ifndef R_68K_NONE
#define R_68K_NONE	0
#define R_68K_32	1
#define R_68K_16	2
#define R_68K_8		3
#define R_68K_PC32	4
#define R_68K_PC16	5
#define R_68K_PC8	6
#define R_68K_GOT32	7
#define R_68K_GOT16	8
#define R_68K_GOT8	9
#define R_68K_GOT32O	10
#define R_68K_GOT16O	11
#define R_68K_GOT8O	12
#define R_68K_PLT32	13
#define R_68K_PLT16	14
#define R_68K_PLT8	15
#define R_68K_PLT32O	16
#define R_68K_PLT16O	17
#define R_68K_PLT8O	18
#define R_68K_COPY	19
#define R_68K_GLOB_DAT	20
#define R_68K_JMP_SLOT	21
#define R_68K_RELATIVE	22
#define R_68K_NUM	23
#endif

/*
 * These constants define the elements of the auxiliary vector used to
 * pass additional information from the kernel to an ELF application.
 */

#ifndef AT_NULL
typedef struct
{
  int	a_type;
  union{
    long a_val;
    void *p_ptr;
    void (*a_fcn)();
  } a_un;
} auxv_t;

/*
 * Values of a_type... These often appear in the file /usr/include/sys/auxv.h
 * on SVr4 systems.
 */
#define AT_NULL		0
#define AT_IGNORE	1
#define AT_EXECFD	2
#define AT_PHDR		3
#define AT_PHENT	4
#define AT_PHNUM	5
#define AT_PAGESZ	6
#define AT_BASE		7
#define AT_FLAGS       	8
#define AT_ENTRY	9
#endif
#ifndef AT_NOTELF
#define AT_NOTELF 10	/* program is not ELF */
#define AT_UID    11	/* real uid */
#define AT_EUID   12	/* effective uid */
#define AT_GID    13	/* real gid */
#define AT_EGID   14	/* effective gid */
#endif

extern int _dl_linux_resolve(void);
extern struct elf_resolve * _dl_load_shared_library(int secure, 
				struct elf_resolve *, char * libname);
extern void * _dl_malloc(int size);
extern int _dl_map_cache(void);
extern int _dl_unmap_cache(void);

extern struct elf_resolve * _dl_load_elf_shared_library(int secure, 
				char * libname, int);
int _dl_copy_fixups(struct dyn_elf * tpnt);

extern int linux_run(int argc, char * argv[]);

extern void _dl_parse_lazy_relocation_information(struct elf_resolve * tpnt, int rel_addr,
       int rel_size, int type);

extern int _dl_parse_relocation_information(struct elf_resolve * tpnt, int rel_addr,
       int rel_size, int type);
extern int _dl_parse_copy_information(struct dyn_elf * rpnt, int rel_addr,
       int rel_size, int type);


/* This means that we may be forced to manually search for copy fixups
   which were omitted by the linker.  We cannot depend upon the DT_TEXTREL
   to tell us whether there are fixups in a text section or not. */

#ifndef SVR4_BUGCOMPAT
#define SVR4_BUGCOMPAT 1
#endif

#ifndef PF_R
#define PF_R 4
#define PF_W 2
#define PF_X 1
#endif

/* Convert between the Linux flags for page protections and the
   ones specified in the ELF standard. */

#define LXFLAGS(X) ( (((X) & PF_R) ? PROT_READ : 0) | \
		    (((X) & PF_W) ? PROT_WRITE : 0) | \
		    (((X) & PF_X) ? PROT_EXEC : 0))

