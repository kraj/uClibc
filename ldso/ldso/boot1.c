/* Run an ELF binary on a linux system.

   Copyright (C) 1993-1996, Eric Youngdale.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */



/* Program to load an ELF binary on a linux system, and run it.
 * References to symbols in sharable libraries can be resolved by
 * an ELF sharable library. */

/* Disclaimer:  I have never seen any AT&T source code for SVr4, nor have
   I ever taken any courses on internals.  This program was developed using
   information available through the book "UNIX SYSTEM V RELEASE 4,
   Programmers guide: Ansi C and Programming Support Tools", which did
   a more than adequate job of explaining everything required to get this
   working. */

/*
 * The main trick with this program is that initially, we ourselves are not
 * dynamicly linked.  This means that we cannot access any global variables
 * since the GOT is initialized by the linker assuming a virtual address of 0,
 * and we cannot call any functions since the PLT is not initialized at all
 * (it will tend to want to call the dynamic linker
 *
 * There are further restrictions - we cannot use large switch statements,
 * since the compiler generates tables of addresses and jumps through them.
 * We can use inline functions, because these do not transfer control to
 * a new address, but they must be static so that they are not exported
 * from the modules.  We cannot use normal syscall stubs, because these
 * all reference the errno global variable which is not yet initialized.
 * We can use all of the local stack variables that we want, since these
 * are all referenced to %ebp or %esp.
 *
 * Life is further complicated by the fact that initially we do not want
 * to do a complete dynamic linking.  We want to allow the user to supply
 * new functions replacing some of the library versions, and until we have
 * the list of modules that we should search set up, we do not want to do
 * any of this.  Thus I have chosen to only perform the relocations for
 * variables that start with "_dl_" since ANSI specifies that the user is
 * not supposed to redefine any of these variables.
 *
 * Fortunately, the linker itself leaves a few clues lying around, and
 * when the kernel starts the image, there are a few further clues.
 * First of all, there is information buried on the stack that the kernel
 * leaves, which includes information about the load address that the
 * program interpreter was loaded at, the number of sections, the address
 * the application was loaded at and so forth.  Here this information
 * is stored in the array dl_info, and the indicies are taken from the
 * file /usr/include/sys/auxv.h on any SVr4 system.
 *
 * The linker itself leaves a pointer to the .dynamic section in the first
 * slot of the GOT, and as it turns out, %ebx points to ghe GOT when
 * you are using PIC code, so we just dereference this to get the address
 * of the dynamic sections.
 *
 * Typically you must load all text pages as writable so that dynamic linking
 * can succeed.  The kernel under SVr4 loads these as R/O, so we must call
 * mprotect to change the protections.  Once we are done, we should set these
 * back again, so the desired behavior is achieved.  Under linux there is
 * currently no mprotect function in the distribution kernel (although
 * someone has alpha patches), so for now everything is loaded writable.
 *
 * We do not have access to malloc and friends at the initial stages of dynamic
 * linking, and it would be handy to have some scratchpad memory available
 * for use as we set things up.  It is a bit of a kluge, but we mmap /dev/zero
 * to get one page of scratchpad.  A simpleminded _dl_malloc is provided so
 * that we have some memory that can be used for this purpose.  Typically
 * we would not want to use the same memory pool as malloc anyway - the user
 * might want to redefine malloc for example.
 *
 * Our first task is to perform a minimal linking so that we can call other
 * portions of the dynamic linker.  Once we have done this, we then build
 * the list of modules that the application requires, using LD_LIBRARY_PATH
 * if this is not a suid program (/usr/lib otherwise).  Once this is done,
 * we can do the dynamic linking as required (and we must omit the things
 * we did to get the dynamic linker up and running in the first place.
 * After we have done this, we just have a few housekeeping chores and we
 * can transfer control to the user's application.
 */

#include <stdarg.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/elf.h>
#include <linux/mman.h>
#include "link.h"

#include "sysdep.h"
#include "hash.h"
#include "linuxelf.h"
#include "syscall.h"
#include "string.h"

#include "../config.h"

#define ALLOW_ZERO_PLTGOT

static char * _dl_malloc_addr, *_dl_mmap_zero;
char * _dl_library_path = 0; /* Where we look for libraries */
char *_dl_preload = 0; /* Things to be loaded before the libs. */
char *_dl_progname = "/lib/ld-linux-uclibc.so.1";
static char * _dl_not_lazy = 0;
static char * _dl_warn = 0; /* Used by ldd */
static char * _dl_trace_loaded_objects = 0;
static int (*_dl_elf_main)(int, char **, char**);

static int (*_dl_elf_init)(void);

void * (*_dl_malloc_function)(int size) = NULL;

struct r_debug * _dl_debug_addr = NULL;

unsigned int * _dl_brkp; 

unsigned int * _dl_envp;

#define DL_MALLOC(SIZE) ((void *) (malloc_buffer += SIZE, malloc_buffer - SIZE))
/*
 * Make sure that the malloc buffer is aligned on 4 byte boundary.  For 64 bit
 * platforms we may need to increase this to 8, but this is good enough for
 * now.  This is typically called after DL_MALLOC.
 */
#define REALIGN() malloc_buffer = (char *) (((unsigned int) malloc_buffer + 3) & ~(3))



#define ELF_HASH(RESULT,NAME) { \
  unsigned long hash = 0; \
    unsigned long tmp;  \
  char * name = NAME; \
  while (*name){  \
    hash = (hash << 4) + *name++; \
    if((tmp = hash & 0xf0000000)) hash ^= tmp >> 24; \
    hash &= ~tmp; \
  } \
  RESULT = hash; \
}
extern int _dl_linux_resolve(void);
extern char * _dl_strdup(const char *);
extern char * _dl_getenv(char * symbol, char ** envp);
extern void _dl_unsetenv(char * symbol, char ** envp);
extern int _dl_fixup(struct elf_resolve * tpnt);

/*
 * Datatype of a relocation on this platform
 */
#ifdef ELF_USES_RELOCA
typedef struct elf32_rela ELF_RELOC;
#else
typedef struct elf32_rel ELF_RELOC;
#endif

/*
 * This stub function is used by some debuggers.  The idea is that they
 * can set an internal breakpoint on it, so that we are notified when the
 * address mapping is changed in some way.
 */
void _dl_debug_state()
{
  return;
}

void _dl_boot(int args);

void _dl_boot(int args){
  unsigned int argc;
  char ** argv, ** envp;
  int status;

  unsigned int load_addr;
  unsigned int * got;
  unsigned int * aux_dat;
  int goof = 0;
  struct elfhdr * header;
  struct elf_resolve * tpnt;
  struct dyn_elf * rpnt;
  struct elf_resolve * app_tpnt;
  unsigned int brk_addr;
  unsigned int dl_data[AT_EGID+1];
  unsigned char * malloc_buffer, *mmap_zero;
  int (*_dl_atexit)(void *);
  int * lpnt;
  struct dynamic * dpnt;
  unsigned int *hash_addr;
  struct r_debug * debug_addr;
  unsigned int *chains;
  int indx;
  int _dl_secure;

  /* First obtain the information on the stack that tells us more about
     what binary is loaded, where it is loaded, etc, etc */

  GET_ARGV(aux_dat, args);
  argc = *(aux_dat - 1);
  argv = (char **) aux_dat;
  aux_dat += argc;  /* Skip over the argv pointers */
  aux_dat++;  /* Skip over NULL at end of argv */
  envp = (char **) aux_dat;
  while(*aux_dat) aux_dat++;  /* Skip over the envp pointers */
  aux_dat++;  /* Skip over NULL at end of envp */
  dl_data[AT_UID] = -1; /* check later to see if it is changed */
  while(*aux_dat)
    {
      unsigned int * ad1;
      ad1 = aux_dat + 1;
      if( *aux_dat <= AT_EGID ) dl_data[*aux_dat] = *ad1;
      aux_dat += 2;
    }

  /* Next, locate the GOT */

  load_addr = dl_data[AT_BASE];

  GET_GOT(got);
  dpnt = (struct dynamic *) (*got + load_addr);
 
  /* OK, time for another hack.  Now call mmap to get a page of writable
     memory that can be used for a temporary malloc.  We do not know brk
     yet, so we cannot use real malloc. */

  {
    /* This hack is to work around a suspected asm bug in gcc-2.7.0 */
    //int zfileno;
//#define ZFILENO ((-1 & (~zfileno)) | zfileno)
#define ZFILENO -1

#ifndef MAP_ANONYMOUS
#ifdef __sparc__
#define MAP_ANONYMOUS 0x20
#else
#error MAP_ANONYMOUS not defined and suplementary value not known
#endif
#endif

    /* See if we need to relocate this address */
    mmap_zero = malloc_buffer = (unsigned char *) _dl_mmap((void*) 0, 4096,
			     PROT_READ | PROT_WRITE, 
			     MAP_PRIVATE | MAP_ANONYMOUS, ZFILENO, 0);
    if(_dl_mmap_check_error(mmap_zero)) {
	SEND_STDERR("dl_boot: mmap of /dev/zero failed!\n");
	_dl_exit(13);
    }
  }

  tpnt = DL_MALLOC(sizeof(struct elf_resolve));
  REALIGN();
  _dl_memset (tpnt, 0, sizeof (*tpnt));
  app_tpnt = DL_MALLOC(sizeof(struct elf_resolve));
  REALIGN();
  _dl_memset (app_tpnt, 0, sizeof (*app_tpnt));

  /*
   * This is used by gdb to locate the chain of shared libraries that are currently loaded.
   */
  debug_addr = DL_MALLOC(sizeof(struct r_debug));
  REALIGN();
  _dl_memset (debug_addr, 0, sizeof (*debug_addr));

  /* OK, that was easy.  Next scan the DYNAMIC section of the image.
     We are only doing ourself right now - we will have to do the rest later */

  while(dpnt->d_tag)
    {
      tpnt->dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
      if(dpnt->d_tag == DT_TEXTREL ||
	 SVR4_BUGCOMPAT) tpnt->dynamic_info[DT_TEXTREL] = 1;
      dpnt++;
    }

  {
    struct elf_phdr * ppnt;
    int i;
    
    ppnt = (struct elf_phdr *) dl_data[AT_PHDR];
    for(i=0; i<dl_data[AT_PHNUM]; i++, ppnt++)
      if(ppnt->p_type == PT_DYNAMIC) {
	dpnt = (struct dynamic *) ppnt->p_vaddr;
	while(dpnt->d_tag)
	  {
	    if(dpnt->d_tag > DT_JMPREL) {dpnt++; continue; }
	    app_tpnt->dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
	    if(dpnt->d_tag == DT_DEBUG) dpnt->d_un.d_val = (int) debug_addr;
	    if(dpnt->d_tag == DT_TEXTREL ||
	       SVR4_BUGCOMPAT) app_tpnt->dynamic_info[DT_TEXTREL] = 1;
	    dpnt++;
	  }
      }
  }

  /* Get some more of the information that we will need to dynamicly link
     this module to itself */

  hash_addr = (unsigned int *) (tpnt->dynamic_info[DT_HASH]+load_addr);
  tpnt->nbucket = *hash_addr++;
  tpnt->nchain = *hash_addr++;
  tpnt->elf_buckets = hash_addr;
  hash_addr += tpnt->nbucket;
  chains = hash_addr;

  /* Ugly, ugly.  We need to call mprotect to change the protection of
     the text pages so that we can do the dynamic linking.  We can set the
     protection back again once we are done */

  {
    struct elf_phdr * ppnt;
    int i;

    /* First cover the shared library/dynamic linker. */
    if(tpnt->dynamic_info[DT_TEXTREL]) {
      header = (struct elfhdr *) dl_data[AT_BASE];	    
      ppnt = (struct elf_phdr *) (dl_data[AT_BASE] + header->e_phoff);
      for(i=0; i<header->e_phnum ; i++, ppnt++) {
	if(ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W))
		_dl_mprotect((void *) (load_addr + (ppnt->p_vaddr & 0xfffff000)),
			      (ppnt->p_vaddr & 0xfff) + (unsigned int) ppnt->p_filesz,
			      PROT_READ | PROT_WRITE | PROT_EXEC);
      }
    }
    
    /* Now cover the application program. */
    if(app_tpnt->dynamic_info[DT_TEXTREL]) {
      ppnt = (struct elf_phdr *) dl_data[AT_PHDR];
      for(i=0; i<dl_data[AT_PHNUM]; i++, ppnt++) {
	if(ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W))
	  _dl_mprotect((void *) (ppnt->p_vaddr & 0xfffff000),
		       (ppnt->p_vaddr & 0xfff) + (unsigned int) ppnt->p_filesz,
		       PROT_READ | PROT_WRITE | PROT_EXEC);
      }
    }
  }

  /* OK, now do the relocations.  We do not do a lazy binding here, so
   that once we are done, we have considerably more flexibility. */

  goof = 0;
  for(indx=0; indx < 2; indx++)
    {
      int i;
      ELF_RELOC * rpnt;
      unsigned int * reloc_addr;
      unsigned int symbol_addr;
      int symtab_index;
      unsigned int rel_addr, rel_size;

  
#ifdef ELF_USES_RELOCA
      rel_addr = (indx ? tpnt->dynamic_info[DT_JMPREL] : tpnt->dynamic_info[DT_RELA]);
      rel_size = (indx ? tpnt->dynamic_info[DT_PLTRELSZ] : tpnt->dynamic_info[DT_RELASZ]);
#else
      rel_addr = (indx ? tpnt->dynamic_info[DT_JMPREL] : tpnt->dynamic_info[DT_REL]);
      rel_size = (indx ? tpnt->dynamic_info[DT_PLTRELSZ] : tpnt->dynamic_info[DT_RELSZ]);
#endif


      if(!rel_addr) continue;

      /* Now parse the relocation information */
      rpnt = (ELF_RELOC *) (rel_addr + load_addr);
      for(i=0; i< rel_size; i+=sizeof(ELF_RELOC), rpnt++){
	reloc_addr = (int *) (load_addr + (int)rpnt->r_offset);
	symtab_index = ELF32_R_SYM(rpnt->r_info);
	symbol_addr = 0;
	if(symtab_index) {
	  char * strtab;
	  struct elf32_sym * symtab;

	  symtab = (struct elf32_sym *) (tpnt->dynamic_info[DT_SYMTAB]+load_addr);
	  strtab = (char *) (tpnt->dynamic_info[DT_STRTAB]+load_addr);

	  /* We only do a partial dynamic linking right now.  The user
	     is not supposed to redefine any symbols that start with
	     a '_', so we can do this with confidence. */

	  if (!_dl_symbol(strtab + symtab[symtab_index].st_name)) continue;

	  symbol_addr = load_addr + symtab[symtab_index].st_value;

	  if(!symbol_addr) {
	    /*
	     * This will segfault - you cannot call a function until
	     * we have finished the relocations.
	     */
	    SEND_STDERR("ELF dynamic loader - unable to self-bootstrap - symbol ");
	    SEND_STDERR(strtab + symtab[symtab_index].st_name);
	    SEND_STDERR(" undefined.\n");
	    goof++;
	  }
	}
	/*
	 * Use this machine-specific macro to perform the actual relocation.
	 */
	PERFORM_BOOTSTRAP_RELOC(rpnt, reloc_addr, symbol_addr, load_addr);
      }
    }

  if (goof)    _dl_exit(14);

  /* OK, at this point we have a crude malloc capability.  Start to build
     the tables of the modules that are required for this beast to run.
     We start with the basic executable, and then go from there.  Eventually
     we will run across ourself, and we will need to properly deal with that
     as well. */

  _dl_malloc_addr = malloc_buffer;

  _dl_mmap_zero = mmap_zero;
/*  tpnt = _dl_malloc(sizeof(struct elf_resolve)); */

/* Now we have done the mandatory linking of some things.  We are now
   free to start using global variables, since these things have all been
   fixed up by now.  Still no function calls outside of this library ,
   since the dynamic resolver is not yet ready. */

  lpnt = (int *) (tpnt->dynamic_info[DT_PLTGOT] + load_addr);
  INIT_GOT(lpnt, tpnt);

  /* OK, this was a big step, now we need to scan all of the user images
     and load them properly. */

  tpnt->next = 0;
  tpnt->libname = 0;
  tpnt->libtype = program_interpreter;

  { struct elfhdr * epnt;
    struct elf_phdr * ppnt;
    int i;

    epnt = (struct elfhdr *) dl_data[AT_BASE];
    tpnt->n_phent = epnt->e_phnum;
    tpnt->ppnt = ppnt = (struct elf_phdr *) (load_addr + epnt->e_phoff);
    for(i=0;i < epnt->e_phnum; i++, ppnt++){
      if(ppnt->p_type == PT_DYNAMIC) {
	tpnt->dynamic_addr = ppnt->p_vaddr + load_addr;
	tpnt->dynamic_size = ppnt->p_filesz;
      }
    }
  }

  tpnt->chains = chains;
  tpnt->loadaddr = (char *) load_addr;

  brk_addr = 0;
  rpnt = NULL;

  /* At this point we are now free to examine the user application,
     and figure out which libraries are supposed to be called.  Until
     we have this list, we will not be completely ready for dynamic linking */

  {
    struct elf_phdr * ppnt;
    int i;

    ppnt = (struct elf_phdr *) dl_data[AT_PHDR];
    for(i=0; i<dl_data[AT_PHNUM]; i++, ppnt++) {
      if(ppnt->p_type == PT_LOAD) {
	if(ppnt->p_vaddr + ppnt->p_memsz > brk_addr) 
	  brk_addr = ppnt->p_vaddr + ppnt->p_memsz;
      }
      if(ppnt->p_type == PT_DYNAMIC) {
#ifndef ALLOW_ZERO_PLTGOT
	/* make sure it's really there. */
	if (app_tpnt->dynamic_info[DT_PLTGOT] == 0) continue;
#endif
	/* OK, we have what we need - slip this one into the list. */
	app_tpnt = _dl_add_elf_hash_table("", 0, 
			    app_tpnt->dynamic_info, ppnt->p_vaddr, ppnt->p_filesz);
	_dl_loaded_modules->libtype = elf_executable;
	_dl_loaded_modules->ppnt = (struct elf_phdr *) dl_data[AT_PHDR];
	_dl_loaded_modules->n_phent = dl_data[AT_PHNUM];
	_dl_symbol_tables = rpnt = 
	   (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
	_dl_memset (rpnt, 0, sizeof (*rpnt));
	rpnt->dyn = _dl_loaded_modules;
	app_tpnt->usage_count++;
	app_tpnt->symbol_scope = _dl_symbol_tables;
	lpnt = (int *) (app_tpnt->dynamic_info[DT_PLTGOT]);
#ifdef ALLOW_ZERO_PLTGOT
	if (lpnt)
#endif
	  INIT_GOT(lpnt, _dl_loaded_modules);
      }
      if(ppnt->p_type == PT_INTERP) { /* OK, fill this in - we did not have
					 this before */
	tpnt->libname =  _dl_strdup((char *) ppnt->p_offset +(dl_data[AT_PHDR] & 0xfffff000));
      }
    }
  }

  if (argv[0])
    _dl_progname = argv[0];

  /* Now we need to figure out what kind of options are selected.
   Note that for SUID programs we ignore the settings in LD_LIBRARY_PATH */
  {
    _dl_not_lazy = _dl_getenv("LD_BIND_NOW",envp);

    if ( (dl_data[AT_UID] == -1 && _dl_suid_ok()) ||
	 (dl_data[AT_UID] != -1 && dl_data[AT_UID] == dl_data[AT_EUID] &&
	  dl_data[AT_GID] == dl_data[AT_EGID]))
    {
      _dl_secure = 0;
      _dl_preload = _dl_getenv("LD_PRELOAD", envp);
      _dl_library_path = _dl_getenv("LD_LIBRARY_PATH",envp);
    }
    else
    {
      _dl_secure = 1;
      _dl_preload = _dl_getenv("LD_PRELOAD", envp);
      _dl_unsetenv("LD_AOUT_PRELOAD", envp);
      _dl_unsetenv("LD_LIBRARY_PATH", envp);
      _dl_unsetenv("LD_AOUT_LIBRARY_PATH", envp);
      _dl_library_path = NULL;
    }
  }

  _dl_trace_loaded_objects = _dl_getenv("LD_TRACE_LOADED_OBJECTS", envp);

  /* OK, we now have the application in the list, and we have some
     basic stuff in place.  Now search through the list for other shared
     libraries that should be loaded, and insert them on the list in the
     correct order. */

#ifdef USE_CACHE
  _dl_map_cache();
#endif

  {
    struct elf_resolve *tcurr;
    struct elf_resolve *tpnt1;
    char *lpnt;

    if (_dl_preload) {
      char c, *str, *str2;

      str = _dl_preload;
      while (*str == ':' || *str == ' ' || *str == '\t')
	str++;
      while (*str) {
	str2 = str;
	while (*str2 && *str2 != ':' && *str2 != ' ' && *str2 != '\t')
	  str2++;
	c = *str2;
	*str2 = '\0';
	if (!_dl_secure || _dl_strchr(str, '/') == NULL) {
	  tpnt1 = _dl_load_shared_library(_dl_secure, NULL, str);
	  if (!tpnt1) {
	    if (_dl_trace_loaded_objects)
	      _dl_fdprintf(1, "\t%s => not found\n", str);
	    else {
	      _dl_fdprintf(2, "%s: can't load library '%s'\n",
			   _dl_progname, str);
	      _dl_exit(15);
	    }
	  } else {
	    if (_dl_trace_loaded_objects && !tpnt1->usage_count) {
	      /* this is a real hack to make ldd not print the
		 library itself when run on a library. */
	      if (_dl_strcmp(_dl_progname, str) != 0)
		_dl_fdprintf(1, "\t%s => %s (0x%x)\n", str, tpnt1->libname,
			     (unsigned)tpnt1->loadaddr);
	    }
	    rpnt->next = 
	      (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
	    _dl_memset (rpnt->next, 0, sizeof (*(rpnt->next)));
	    rpnt = rpnt->next;
	    tpnt1->usage_count++;
	    tpnt1->symbol_scope = _dl_symbol_tables;
	    tpnt1->libtype = elf_lib;
	    rpnt->dyn = tpnt1;
	  }
	}
	*str2 = c;
	str = str2;
	while (*str == ':' || *str == ' ' || *str == '\t')
	  str++;
      }
    }

    {
      int fd;
      struct kernel_stat st;
      char *preload;

      if (!_dl_stat(LDSO_PRELOAD, &st)) {
	if ((fd = _dl_open(LDSO_PRELOAD, O_RDONLY)) < 0) {
	  _dl_fdprintf(2, "%s: can't open file '%s'\n", _dl_progname, 
		       LDSO_PRELOAD);
	} else {
	  preload = (caddr_t)_dl_mmap(0, st.st_size+1, PROT_READ|PROT_WRITE, 
				      MAP_PRIVATE, fd, 0);
	  _dl_close (fd);
	  if (preload == (caddr_t)-1) {
	    _dl_fdprintf(2, "%s: can't map file '%s'\n", _dl_progname, 
			 LDSO_PRELOAD);
	  } else {
	    char c, *cp, *cp2;

	    /* convert all separators and comments to spaces */
	    for (cp = preload; *cp; /*nada*/) {
	      if (*cp == ':' || *cp == '\t' || *cp == '\n') {
		*cp++ = ' ';
	      } else if (*cp == '#') {
		do
		  *cp++ = ' ';
		while (*cp != '\n' && *cp != '\0');
	      } else {
		cp++;
	      }
	    }

	    /* find start of first library */
	    for (cp = preload; *cp && *cp == ' '; cp++)
	      /*nada*/;

	    while (*cp) {
	      /* find end of library */
	      for (cp2 = cp; *cp && *cp != ' '; cp++)
		/*nada*/;
	      c = *cp;
	      *cp = '\0';

	      tpnt1 = _dl_load_shared_library(0, NULL, cp2);
	      if (!tpnt1) {
		if (_dl_trace_loaded_objects)
		  _dl_fdprintf(1, "\t%s => not found\n", cp2);
		else {
		  _dl_fdprintf(2, "%s: can't load library '%s'\n",
			       _dl_progname, cp2);
		  _dl_exit(15);
		}
	      } else {
		if (_dl_trace_loaded_objects && !tpnt1->usage_count)
		  _dl_fdprintf(1, "\t%s => %s (0x%x)\n", cp2, tpnt1->libname,
			       (unsigned)tpnt1->loadaddr);
                rpnt->next = 
		  (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
		_dl_memset (rpnt->next, 0, sizeof (*(rpnt->next)));
		rpnt = rpnt->next;
		tpnt1->usage_count++;
		tpnt1->symbol_scope = _dl_symbol_tables;
		tpnt1->libtype = elf_lib;
		rpnt->dyn = tpnt1;
	      }

	      /* find start of next library */
	      *cp = c;
	      for (/*nada*/; *cp && *cp == ' '; cp++)
		/*nada*/;
	    }

	    _dl_munmap(preload, st.st_size+1);
	  }
	}
      }
    }

    for (tcurr = _dl_loaded_modules; tcurr; tcurr = tcurr->next)
    {
      for (dpnt = (struct dynamic *)tcurr->dynamic_addr; dpnt->d_tag; dpnt++)
      {
	if(dpnt->d_tag == DT_NEEDED)
	{
	  lpnt = tcurr->loadaddr + tcurr->dynamic_info[DT_STRTAB] + 
	    dpnt->d_un.d_val;
	  if (tpnt && _dl_strcmp(lpnt, tpnt->libname) == 0)
	  {
	    struct elf_resolve * ttmp;
	    ttmp = _dl_loaded_modules;
	    while (ttmp->next) 
	      ttmp = ttmp->next;
	    ttmp->next = tpnt;
	    tpnt->prev = ttmp;
	    tpnt->next = NULL;
	    rpnt->next = 
	      (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
	    _dl_memset (rpnt->next, 0, sizeof (*(rpnt->next)));
	    rpnt = rpnt->next;
	    rpnt->dyn = tpnt;
	    tpnt->usage_count++;
	    tpnt->symbol_scope = _dl_symbol_tables;
	    tpnt = NULL;
	    continue;
	  }
	  if (!(tpnt1 = _dl_load_shared_library(0, tcurr, lpnt)))
	  {
	    if (_dl_trace_loaded_objects)
	      _dl_fdprintf(1, "\t%s => not found\n", lpnt);
	    else
	    {
	      _dl_fdprintf(2, "%s: can't load library '%s'\n",
			   _dl_progname, lpnt);
	      _dl_exit(16);
	    }
	  }
	  else
	  {
	    if (_dl_trace_loaded_objects && !tpnt1->usage_count)
	      _dl_fdprintf(1, "\t%s => %s (0x%x)\n", lpnt, tpnt1->libname,
			   (unsigned)tpnt1->loadaddr);
	    rpnt->next = 
	      (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
	    _dl_memset (rpnt->next, 0, sizeof (*(rpnt->next)));
	    rpnt = rpnt->next;
	    tpnt1->usage_count++;
	    tpnt1->symbol_scope = _dl_symbol_tables;
	    tpnt1->libtype = elf_lib;
	    rpnt->dyn = tpnt1;
	  }
	}
      }
    }
  }

#ifdef USE_CACHE
  _dl_unmap_cache();
#endif

    /* ldd uses uses this.  I am not sure how you pick up the other flags */ 
  if(_dl_trace_loaded_objects)
    {
      _dl_warn = _dl_getenv("LD_WARN", envp);
      if (!_dl_warn) _dl_exit(0);
    }

  /*
   * If the program interpreter is not in the module chain, add it.  This will
   * be required for dlopen to be able to access the internal functions in the 
   * dynamic linker.
   */
  if(tpnt) {
    struct elf_resolve * tcurr;

    tcurr = _dl_loaded_modules;
    if (tcurr)
      while(tcurr->next) tcurr = tcurr->next;
    tpnt->next = NULL;
    tpnt->usage_count++;

    if (tcurr) {
      tcurr->next = tpnt;
      tpnt->prev = tcurr;
    }
    else {
      _dl_loaded_modules = tpnt;
      tpnt->prev = NULL;
    }
    if (rpnt) {
      rpnt->next = 
	(struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
      _dl_memset (rpnt->next, 0, sizeof (*(rpnt->next)));
      rpnt = rpnt->next;
    } else {
      rpnt = 	(struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
      _dl_memset (rpnt, 0, sizeof (*(rpnt->next)));
    }
    rpnt->dyn = tpnt;
    tpnt = NULL;
  }

  /*
   * OK, now all of the kids are tucked into bed in their proper addresses.
   * Now we go through and look for REL and RELA records that indicate fixups
   * to the GOT tables.  We need to do this in reverse order so that COPY
   * directives work correctly */


  goof = _dl_loaded_modules ? _dl_fixup(_dl_loaded_modules) : 0;


  /* Some flavors of SVr4 do not generate the R_*_COPY directive,
   and we have to manually search for entries that require fixups. 
   Solaris gets this one right, from what I understand.  */


  if (_dl_symbol_tables)
    goof += _dl_copy_fixups(_dl_symbol_tables);

  if(goof || _dl_trace_loaded_objects) _dl_exit(0);

  /* OK, at this point things are pretty much ready to run.  Now we
     need to touch up a few items that are required, and then
     we can let the user application have at it.  Note that
     the dynamic linker itself is not guaranteed to be fully
     dynamicly linked if we are using ld.so.1, so we have to look
     up each symbol individually. */


  _dl_brkp = (unsigned int *) _dl_find_hash("___brk_addr", NULL, 1, NULL, 0);
  if (_dl_brkp) *_dl_brkp = brk_addr;
  _dl_envp = (unsigned int *) _dl_find_hash("__environ", NULL, 1, NULL, 0);

  if (_dl_envp) *_dl_envp = (unsigned int) envp;

  {
    int i;
    struct elf_phdr * ppnt;

  /* We had to set the protections of all pages to R/W for dynamic linking.
     Set text pages back to R/O */
  for(tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next)
    for(ppnt = tpnt->ppnt, i=0; i < tpnt->n_phent; i++, ppnt++)
      if(ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W) &&
	 tpnt->dynamic_info[DT_TEXTREL])
	_dl_mprotect((void *) (tpnt->loadaddr + (ppnt->p_vaddr & 0xfffff000)),
		     (ppnt->p_vaddr & 0xfff) + (unsigned int) ppnt->p_filesz,
		     LXFLAGS(ppnt->p_flags));

  }

  _dl_atexit = (int (*)(void *)) _dl_find_hash("atexit", NULL, 1, NULL, 0);

  /*
   * OK, fix one more thing - set up the debug_addr structure to point
   * to our chain.  Later we may need to fill in more fields, but this
   * should be enough for now.
   */
  debug_addr->r_map = (struct link_map *) _dl_loaded_modules;
  debug_addr->r_version = 1;
  debug_addr->r_ldbase = load_addr;
  debug_addr->r_brk = (unsigned long) &_dl_debug_state;
  _dl_debug_addr = debug_addr;
  debug_addr->r_state = RT_CONSISTENT;
  /* This is written in this funny way to keep gcc from inlining the
     function call. */
  ((void (*)(void))debug_addr->r_brk)();

  for(tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next)
    {
      /* Apparently crt1 for the application is responsible for handling this.
       * We only need to run the init/fini for shared libraries
       */
      if (tpnt->libtype == program_interpreter ||
	tpnt->libtype == elf_executable) continue;
      if (tpnt->init_flag & INIT_FUNCS_CALLED) continue;
      tpnt->init_flag |= INIT_FUNCS_CALLED;
      
      if(tpnt->dynamic_info[DT_INIT]) {
	_dl_elf_init = (int (*)(void)) (tpnt->loadaddr + 
				    tpnt->dynamic_info[DT_INIT]);
	(*_dl_elf_init)();
      }
      if(_dl_atexit && tpnt->dynamic_info[DT_FINI])
      {
        (*_dl_atexit)(tpnt->loadaddr + tpnt->dynamic_info[DT_FINI]);
      }
#undef DL_DEBUG
#ifdef DL_DEBUG
      else
      {
	_dl_fdprintf(2, tpnt->libname);
	_dl_fdprintf(2, ": ");
	if (!_dl_atexit)
	  _dl_fdprintf(2, "The address is atexit () is 0x0.");
	if (!tpnt->dynamic_info[DT_FINI])
	  _dl_fdprintf(2, "Invalid .fini section.");
	_dl_fdprintf(2, "\n");
      }
#endif
#undef DL_DEBUG
   }

  /* OK we are done here.  Turn out the lights, and lock up. */
  _dl_elf_main = (int (*)(int, char**, char**)) dl_data[AT_ENTRY];


  /*
   * Transfer control to the application.
   */
  START();
}

int _dl_fixup(struct elf_resolve * tpnt)
{
  int goof = 0;
  if(tpnt->next) goof += _dl_fixup(tpnt->next);

  if(tpnt->dynamic_info[DT_REL]) {
#ifdef ELF_USES_RELOCA
    _dl_fdprintf(2, "%s: can't handle REL relocation records\n", _dl_progname);
    _dl_exit(17);
#else
    if (tpnt->init_flag & RELOCS_DONE) return goof;
    tpnt->init_flag |= RELOCS_DONE;
   
    goof += _dl_parse_relocation_information(tpnt, tpnt->dynamic_info[DT_REL],
					     tpnt->dynamic_info[DT_RELSZ], 0);
#endif
  }
  if(tpnt->dynamic_info[DT_RELA]) {
#ifdef ELF_USES_RELOCA
    if (tpnt->init_flag & RELOCS_DONE) return goof;
    tpnt->init_flag |= RELOCS_DONE;
   
    goof += _dl_parse_relocation_information(tpnt, tpnt->dynamic_info[DT_RELA],
					     tpnt->dynamic_info[DT_RELASZ], 0);
#else
    _dl_fdprintf(2, "%s: can't handle RELA relocation records\n", _dl_progname);
    _dl_exit(18);
#endif
  }
  if(tpnt->dynamic_info[DT_JMPREL])
    {
      if (tpnt->init_flag & JMP_RELOCS_DONE) return goof;
      tpnt->init_flag |= JMP_RELOCS_DONE;
      
      if(! _dl_not_lazy || *_dl_not_lazy == 0)
	_dl_parse_lazy_relocation_information(tpnt, tpnt->dynamic_info[DT_JMPREL],
					      tpnt->dynamic_info[DT_PLTRELSZ], 0);
      else
	goof +=  _dl_parse_relocation_information(tpnt,
						  tpnt->dynamic_info[DT_JMPREL],
						  tpnt->dynamic_info[DT_PLTRELSZ], 0);
    }
  return goof;
}

void * _dl_malloc(int size) {
  void * retval;

  if(_dl_malloc_function)
  	return (*_dl_malloc_function)(size);

  if(_dl_malloc_addr-_dl_mmap_zero+size>4096) {
	_dl_mmap_zero = _dl_malloc_addr = (unsigned char *) _dl_mmap((void*) 0, size,
  				PROT_READ | PROT_WRITE, 
  				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  	if(_dl_mmap_check_error(_dl_mmap_zero)) {
  	    _dl_fdprintf(2, "%s: can't map '/dev/zero'\n", _dl_progname);
  	    _dl_exit(20);
  	}
  }
  retval = _dl_malloc_addr;
  _dl_malloc_addr += size;

  /*
   * Align memory to 4 byte boundary.  Some platforms require this, others
   * simply get better performance.
   */
  _dl_malloc_addr = (char *) (((unsigned int) _dl_malloc_addr + 3) & ~(3));
  return retval;
}

char * _dl_getenv(char *symbol, char **envp)
{
  char *pnt;
  char *pnt1;
  while ((pnt = *envp++)) {
    pnt1 = symbol;
    while (*pnt && *pnt == *pnt1)
      pnt1++, pnt++;
    if (!*pnt || *pnt != '=' || *pnt1)
      continue;
    return pnt+1;
  }
  return 0;
}

void _dl_unsetenv(char *symbol, char **envp)
{
  char *pnt;
  char *pnt1;
  char **newenvp = envp;
  for (pnt = *envp; pnt; pnt = *++envp) {
    pnt1 = symbol;
    while (*pnt && *pnt == *pnt1)
      pnt1++, pnt++;
    if(!*pnt || *pnt != '=' || *pnt1)
      *newenvp++ = *envp;
  }
  *newenvp++ = *envp;
  return;
}

char * _dl_strdup(const char * string){
  char * retval;
  int len;

  len = _dl_strlen(string);
  retval = _dl_malloc(len + 1);
  _dl_strcpy(retval, string);
  return retval;
}

