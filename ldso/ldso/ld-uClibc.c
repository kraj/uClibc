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
 * linking, and it would be handy to have some scratchpad memory available for
 * use as we set things up.  We mmap one page of scratch space, and have a
 * simple _dl_malloc that uses this memory.  This is a good thing, since we do
 * not want to use the same memory pool as malloc anyway - esp if the user
 * redefines malloc to do something funky.
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
#include "sysdep.h" /* before elf.h to get ELF_USES_RELOCA right */
#include <elf.h>
#include "linuxelf.h"
#include "link.h"
#include "hash.h"
#include "syscall.h"
#include "string.h"
#include "../config.h"

#define ALLOW_ZERO_PLTGOT

/*  Some arches may need to override this in boot1_arch.h */
#define	    ELFMAGIC	ELFMAG

/* This is a poor man's malloc, used prior to resolving our internal poor man's malloc */
#define DL_MALLOC(SIZE) ((void *) (malloc_buffer += SIZE, malloc_buffer - SIZE)) ;  REALIGN();
/*
 * Make sure that the malloc buffer is aligned on 4 byte boundary.  For 64 bit
 * platforms we may need to increase this to 8, but this is good enough for
 * now.  This is typically called after DL_MALLOC.
 */
#define REALIGN() malloc_buffer = (char *) (((unsigned long) malloc_buffer + 3) & ~(3))

static char *_dl_malloc_addr, *_dl_mmap_zero;
char *_dl_library_path = 0;		/* Where we look for libraries */
char *_dl_preload = 0;			/* Things to be loaded before the libs. */
#include "ld.so.h"			/* Pull in the name of ld.so */
const char *_dl_progname=_dl_static_progname;
static char *_dl_not_lazy = 0;
#ifdef DL_TRACE
static char *_dl_trace_loaded_objects = 0;
#endif
static int (*_dl_elf_main) (int, char **, char **);
static int (*_dl_elf_init) (void);
void *(*_dl_malloc_function) (int size) = NULL;
struct r_debug *_dl_debug_addr = NULL;
unsigned long *_dl_brkp;
unsigned long *_dl_envp;
char *_dl_getenv(char *symbol, char **envp);
void _dl_unsetenv(char *symbol, char **envp);
int _dl_fixup(struct elf_resolve *tpnt);
void _dl_debug_state(void);
char *_dl_get_last_path_component(char *path);

#include "boot1_arch.h"


/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL
	auxv_t[0...N]   Auxiliary Vector Table elements (mixed types)
*/

DL_BOOT(unsigned long args)
{
	unsigned int argc;
	char **argv, **envp;
	unsigned long load_addr;
	unsigned long *got;
	unsigned long *aux_dat;
	int goof = 0;
	elfhdr *header;
	struct elf_resolve *tpnt;
	struct dyn_elf *rpnt;
	struct elf_resolve *app_tpnt;
	unsigned long brk_addr;
	Elf32_auxv_t auxv_t[AT_EGID + 1];
	unsigned char *malloc_buffer, *mmap_zero;
	int (*_dl_atexit) (void *);
	unsigned long *lpnt;
	Elf32_Dyn *dpnt;
	unsigned long *hash_addr;
	struct r_debug *debug_addr;
	unsigned long *chains;
	int indx;
	int _dl_secure;
	int status;


	/* WARNING! -- we cannot make _any_ funtion calls until we have
	 * taken care of fixing up our own relocations.  Making static
	 * lnline calls is ok, but _no_ function calls.  Not yet
	 * anyways. */

	/* First obtain the information on the stack that tells us more about
	   what binary is loaded, where it is loaded, etc, etc */
	GET_ARGV(aux_dat,args);
#if defined(__arm__)
	aux_dat+=1;
#endif	
	argc = *(aux_dat - 1);
        argv = (char **) aux_dat;
	aux_dat += argc;			/* Skip over the argv pointers */
	aux_dat++;				/* Skip over NULL at end of argv */
	envp = (char **) aux_dat;
	while (*aux_dat)
		aux_dat++;			/* Skip over the envp pointers */
	aux_dat++;				/* Skip over NULL at end of envp */

	/* Place -1 here as a checkpoint.  We later check if it was changed
	 * when we read in the auxv_t */
	auxv_t[AT_UID].a_type = -1;
	
	/* The junk on the stack immediately following the environment is  
	 * the Auxiliary Vector Table.  Read out the elements of the auxv_t,
	 * sort and store them in auxv_t for later use. */
	while (*aux_dat) 
	{
		Elf32_auxv_t *auxv_entry = (Elf32_auxv_t*) aux_dat;

		if (auxv_entry->a_type <= AT_EGID) {
			_dl_memcpy_inline(&(auxv_t[auxv_entry->a_type]), 
				auxv_entry, sizeof(Elf32_auxv_t));
		}
		aux_dat += 2;
	}
	
	/* locate the ELF header.   We need this done as soon as possible 
	 * (esp since SEND_STDERR() needs this on some platforms... */
	load_addr = auxv_t[AT_BASE].a_un.a_val;
	header = (elfhdr *) auxv_t[AT_BASE].a_un.a_ptr;

	/* Check the ELF header to make sure everything looks ok.  */
	if (! header || header->e_ident[EI_CLASS] != ELFCLASS32 ||
		header->e_ident[EI_VERSION] != EV_CURRENT || 
		_dl_strncmp_inline((void *)header, ELFMAGIC, SELFMAG) != 0)
	{
	    SEND_STDERR("Invalid ELF header\n");
	    _dl_exit(0);
	}
#ifdef DL_DEBUG
	SEND_STDERR("ELF header =");
	SEND_ADDRESS_STDERR(load_addr, 1);
#endif	


	/* Locate the global offset table.  Since this code must be PIC  
	 * we can take advantage of the magic offset register, if we
	 * happen to know what that is for this architecture.  If not,
	 * we can always read stuff out of the ELF file to find it... */
#if defined(__i386__)
	__asm__("\tmovl %%ebx,%0\n\t" : "=a" (got));
#elif defined(__m68k__)
	__asm__ ("movel %%a5,%0" : "=g" (got))
#elif defined(__sparc__)
	__asm__("\tmov %%l7,%0\n\t" : "=r" (got))
#elif defined(__arm__)
	__asm__("\tmov %0, r10\n\t" : "=r"(got));
#elif defined(__powerpc__)
	__asm__("\tbl _GLOBAL_OFFSET_TABLE_-4@local\n\t" : "=l"(got));
#else
	/* Do things the slow way in C */
	{
	    unsigned long tx_reloc;
	    Elf32_Dyn *dynamic=NULL;
	    Elf32_Shdr *shdr;
	    Elf32_Phdr *pt_load;

#ifdef DL_DEBUG
	    SEND_STDERR("Finding the got using C code to read the ELF file\n");
#endif	
	    /* Find where the dynamic linking information section is hiding */
	    shdr = (Elf32_Shdr *)(header->e_shoff + (char *)header);
	    for (indx = header->e_shnum; --indx>=0; ++shdr) {
		if (shdr->sh_type == SHT_DYNAMIC) {
		    goto found_dynamic;
		}
	    }
	    SEND_STDERR("missing dynamic linking information section \n");
	    _dl_exit(0);

found_dynamic:
	    dynamic = (Elf32_Dyn*)(shdr->sh_offset + (char *)header);

	    /* Find where PT_LOAD is hiding */
	    pt_load = (Elf32_Phdr *)(header->e_phoff + (char *)header);
	    for (indx = header->e_phnum; --indx>=0; ++pt_load) {
		if (pt_load->p_type == PT_LOAD) {
		    goto found_pt_load;
		}
	    }
	    SEND_STDERR("missing loadable program segment\n");
	    _dl_exit(0);

found_pt_load:
	    /* Now (finally) find where DT_PLTGOT is hiding */
	    tx_reloc = pt_load->p_vaddr - pt_load->p_offset;
	    for (; DT_NULL!=dynamic->d_tag; ++dynamic) {
		if (dynamic->d_tag == DT_PLTGOT) {
		    goto found_got;
		}       
	    }       
	    SEND_STDERR("missing global offset table\n");
	    _dl_exit(0);

found_got:
	    got = (unsigned long *)(dynamic->d_un.d_val - tx_reloc + (char *)header );
	}
#endif

	/* Now, finally, fix up the location of the dynamic stuff */
	dpnt = (Elf32_Dyn *) (*got + load_addr);
#ifdef DL_DEBUG
	SEND_STDERR("First Dynamic section entry=");
	SEND_ADDRESS_STDERR(dpnt, 1);
#endif	

	
	/* Call mmap to get a page of writable memory that can be used 
	 * for _dl_malloc throughout the shared lib loader. */
	mmap_zero = malloc_buffer = _dl_mmap((void *) 0, 4096, 
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (_dl_mmap_check_error(mmap_zero)) {
	    SEND_STDERR("dl_boot: mmap of a spare page failed!\n");
	    _dl_exit(13);
	}

	tpnt = DL_MALLOC(sizeof(struct elf_resolve));
	_dl_memset_inline(tpnt, 0, sizeof(*tpnt));
	app_tpnt = DL_MALLOC(sizeof(struct elf_resolve));
	_dl_memset_inline(app_tpnt, 0, sizeof(*app_tpnt));

	/*
	 * This is used by gdb to locate the chain of shared libraries that are currently loaded.
	 */
	debug_addr = DL_MALLOC(sizeof(struct r_debug));
	_dl_memset_inline(debug_addr, 0, sizeof(*debug_addr));

	/* OK, that was easy.  Next scan the DYNAMIC section of the image.
	   We are only doing ourself right now - we will have to do the rest later */

	while (dpnt->d_tag) {
		tpnt->dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
		if (dpnt->d_tag == DT_TEXTREL || SVR4_BUGCOMPAT)
			tpnt->dynamic_info[DT_TEXTREL] = 1;
		dpnt++;
	}

	{
		elf_phdr *ppnt;
		int i;

		ppnt = (elf_phdr *) auxv_t[AT_PHDR].a_un.a_ptr;
		for (i = 0; i < auxv_t[AT_PHNUM].a_un.a_val; i++, ppnt++)
			if (ppnt->p_type == PT_DYNAMIC) {
				dpnt = (Elf32_Dyn *) ppnt->p_vaddr;
				while (dpnt->d_tag) {
					if (dpnt->d_tag > DT_JMPREL) {
						dpnt++;
						continue;
					}
					app_tpnt->dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
					if (dpnt->d_tag == DT_DEBUG)
						dpnt->d_un.d_val = (unsigned long) debug_addr;
					if (dpnt->d_tag == DT_TEXTREL || SVR4_BUGCOMPAT)
						app_tpnt->dynamic_info[DT_TEXTREL] = 1;
					dpnt++;
				}
			}
	}

	/* Get some more of the information that we will need to dynamicly link
	   this module to itself */

	hash_addr = (unsigned long *) (tpnt->dynamic_info[DT_HASH] + load_addr);
	tpnt->nbucket = *hash_addr++;
	tpnt->nchain = *hash_addr++;
	tpnt->elf_buckets = hash_addr;
	hash_addr += tpnt->nbucket;
	chains = hash_addr;

	/* Ugly, ugly.  We need to call mprotect to change the protection of
	   the text pages so that we can do the dynamic linking.  We can set the
	   protection back again once we are done */

	{
		elf_phdr *ppnt;
		int i;

		/* First cover the shared library/dynamic linker. */
		if (tpnt->dynamic_info[DT_TEXTREL]) {
			header = (elfhdr *) auxv_t[AT_BASE].a_un.a_ptr;
			ppnt = (elf_phdr *) (auxv_t[AT_BASE].a_un.a_ptr + header->e_phoff);
			for (i = 0; i < header->e_phnum; i++, ppnt++) {
				if (ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W))
					_dl_mprotect((void *) (load_addr + (ppnt->p_vaddr & 0xfffff000)), 
						(ppnt->p_vaddr & 0xfff) + (unsigned long) ppnt->p_filesz, 
						PROT_READ | PROT_WRITE | PROT_EXEC);
			}
		}

		/* Now cover the application program. */
		if (app_tpnt->dynamic_info[DT_TEXTREL]) {
			ppnt = (elf_phdr *) auxv_t[AT_PHDR].a_un.a_ptr;
			for (i = 0; i < auxv_t[AT_PHNUM].a_un.a_val; i++, ppnt++) {
				if (ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W))
					_dl_mprotect((void *) (ppnt->p_vaddr & 0xfffff000), 
						(ppnt->p_vaddr & 0xfff) + 
						(unsigned long) ppnt->p_filesz, 
						PROT_READ | PROT_WRITE | PROT_EXEC);
			}
		}
	}

	/* OK, now do the relocations.  We do not do a lazy binding here, so
	   that once we are done, we have considerably more flexibility. */

	goof = 0;
	for (indx = 0; indx < 2; indx++) {
		int i;
		ELF_RELOC *rpnt;
		unsigned long *reloc_addr;
		unsigned long symbol_addr;
		int symtab_index;
		unsigned long rel_addr, rel_size;


#ifdef ELF_USES_RELOCA
		rel_addr =
			(indx ? tpnt->dynamic_info[DT_JMPREL] : tpnt->
			 dynamic_info[DT_RELA]);
		rel_size =
			(indx ? tpnt->dynamic_info[DT_PLTRELSZ] : tpnt->
			 dynamic_info[DT_RELASZ]);
#else
		rel_addr =
			(indx ? tpnt->dynamic_info[DT_JMPREL] : tpnt->
			 dynamic_info[DT_REL]);
		rel_size =
			(indx ? tpnt->dynamic_info[DT_PLTRELSZ] : tpnt->
			 dynamic_info[DT_RELSZ]);
#endif


		if (!rel_addr)
			continue;

		/* Now parse the relocation information */
		rpnt = (ELF_RELOC *) (rel_addr + load_addr);
		for (i = 0; i < rel_size; i += sizeof(ELF_RELOC), rpnt++) {
			reloc_addr = (unsigned long *) (load_addr + (unsigned long) rpnt->r_offset);
			symtab_index = ELF32_R_SYM(rpnt->r_info);
			symbol_addr = 0;
			if (symtab_index) {
				char *strtab;
				Elf32_Sym *symtab;

				symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + load_addr);
				strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + load_addr);

				/* We only do a partial dynamic linking right now.  The user
				   is not supposed to redefine any symbols that start with
				   a '_', so we can do this with confidence. */

				if (!_dl_symbol(strtab + symtab[symtab_index].st_name))
					continue;

				symbol_addr = load_addr + symtab[symtab_index].st_value;

				if (!symbol_addr) {
					/*
					 * This will segfault - you cannot call a function until
					 * we have finished the relocations.
					 */
					SEND_STDERR("ELF dynamic loader - unable to "
						"self-bootstrap - symbol ");
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

	if (goof) {
		_dl_exit(14);
	}

	/* OK, at this point we have a crude malloc capability.  Start to build
	   the tables of the modules that are required for this beast to run.
	   We start with the basic executable, and then go from there.  Eventually
	   we will run across ourself, and we will need to properly deal with that
	   as well. */
	_dl_malloc_addr = malloc_buffer;
	_dl_mmap_zero = mmap_zero;

	/* Now we have done the mandatory linking of some things.  We are now
	   free to start using global variables, since these things have all been
	   fixed up by now.  Still no function calls outside of this library ,
	   since the dynamic resolver is not yet ready. */
	lpnt = (unsigned long *) (tpnt->dynamic_info[DT_PLTGOT] + load_addr);
	INIT_GOT(lpnt, tpnt);

	/* OK, this was a big step, now we need to scan all of the user images
	   and load them properly. */

	tpnt->next = 0;
	tpnt->libname = 0;
	tpnt->libtype = program_interpreter;

	{
		elfhdr *epnt;
		elf_phdr *ppnt;
		int i;

		epnt = (elfhdr *) auxv_t[AT_BASE].a_un.a_ptr;
		tpnt->n_phent = epnt->e_phnum;
		tpnt->ppnt = ppnt = (elf_phdr *) (load_addr + epnt->e_phoff);
		for (i = 0; i < epnt->e_phnum; i++, ppnt++) {
			if (ppnt->p_type == PT_DYNAMIC) {
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
		elf_phdr *ppnt;
		int i;

		ppnt = (elf_phdr *) auxv_t[AT_PHDR].a_un.a_ptr;
		for (i = 0; i < auxv_t[AT_PHNUM].a_un.a_val; i++, ppnt++) {
			if (ppnt->p_type == PT_LOAD) {
				if (ppnt->p_vaddr + ppnt->p_memsz > brk_addr)
					brk_addr = ppnt->p_vaddr + ppnt->p_memsz;
			}
			if (ppnt->p_type == PT_DYNAMIC) {
#ifndef ALLOW_ZERO_PLTGOT
				/* make sure it's really there. */
				if (app_tpnt->dynamic_info[DT_PLTGOT] == 0)
					continue;
#endif
				/* OK, we have what we need - slip this one into the list. */
				app_tpnt = _dl_add_elf_hash_table("", 0, 
					app_tpnt->dynamic_info, ppnt->p_vaddr, ppnt->p_filesz);
				_dl_loaded_modules->libtype = elf_executable;
				_dl_loaded_modules->ppnt = (elf_phdr *) auxv_t[AT_PHDR].a_un.a_ptr;
				_dl_loaded_modules->n_phent = auxv_t[AT_PHNUM].a_un.a_val;
				_dl_symbol_tables = rpnt = (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
				_dl_memset(rpnt, 0, sizeof(*rpnt));
				rpnt->dyn = _dl_loaded_modules;
				app_tpnt->usage_count++;
				app_tpnt->symbol_scope = _dl_symbol_tables;
				lpnt = (unsigned long *) (app_tpnt->dynamic_info[DT_PLTGOT]);
#ifdef ALLOW_ZERO_PLTGOT
				if (lpnt)
#endif
					INIT_GOT(lpnt, _dl_loaded_modules);
			}
			if (ppnt->p_type == PT_INTERP) {	/* OK, fill this in - we did not 
								   have this before */
				tpnt->libname = _dl_strdup((char *) ppnt->p_offset +
							   (auxv_t[AT_PHDR].a_un.a_val & 0xfffff000));
			}
		}
	}

	if (argv[0]) {
		_dl_progname = argv[0];
	}

	/* Now we need to figure out what kind of options are selected.
	   Note that for SUID programs we ignore the settings in LD_LIBRARY_PATH */
	{
		_dl_not_lazy = _dl_getenv("LD_BIND_NOW", envp);

		if ((auxv_t[AT_UID].a_un.a_val == -1 && _dl_suid_ok()) ||
			(auxv_t[AT_UID].a_un.a_val != -1 && 
			 auxv_t[AT_UID].a_un.a_val == auxv_t[AT_EUID].a_un.a_val
			 && auxv_t[AT_GID].a_un.a_val== auxv_t[AT_EGID].a_un.a_val)) {
			_dl_secure = 0;
			_dl_preload = _dl_getenv("LD_PRELOAD", envp);
			_dl_library_path = _dl_getenv("LD_LIBRARY_PATH", envp);
		} else {
			_dl_secure = 1;
			_dl_preload = _dl_getenv("LD_PRELOAD", envp);
			_dl_unsetenv("LD_AOUT_PRELOAD", envp);
			_dl_unsetenv("LD_LIBRARY_PATH", envp);
			_dl_unsetenv("LD_AOUT_LIBRARY_PATH", envp);
			_dl_library_path = NULL;
		}
	}

#ifdef DL_TRACE
	_dl_trace_loaded_objects = _dl_getenv("LD_TRACE_LOADED_OBJECTS", envp);
#endif
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

		if (_dl_preload) 
		{
			char c, *str, *str2;

			str = _dl_preload;
			while (*str == ':' || *str == ' ' || *str == '\t')
				str++;
			while (*str) 
			{
				str2 = str;
				while (*str2 && *str2 != ':' && *str2 != ' ' && *str2 != '\t')
					str2++;
				c = *str2;
				*str2 = '\0';
				if (!_dl_secure || _dl_strchr(str, '/') == NULL) 
				{
					tpnt1 = _dl_load_shared_library(_dl_secure, NULL, str);
					if (!tpnt1) {
#ifdef DL_TRACE
						if (_dl_trace_loaded_objects)
							_dl_fprintf(1, "\t%s => not found\n", str);
						else {
#endif
							_dl_fprintf(2, "%s: can't load "
								"library '%s'\n", _dl_progname, str);
							_dl_exit(15);
#ifdef DL_TRACE
						}
#endif
					} else {
#ifdef DL_TRACE
						if (_dl_trace_loaded_objects
							&& !tpnt1->usage_count) {
							/* this is a real hack to make ldd not print 
							 * the library itself when run on a library. */
							if (_dl_strcmp(_dl_progname, str) != 0)
								_dl_fprintf(1, "\t%s => %s (0x%x)\n", str, tpnt1->libname, 
									(unsigned) tpnt1->loadaddr);
						}
#endif
						rpnt->next = (struct dyn_elf *)
							_dl_malloc(sizeof(struct dyn_elf));
						_dl_memset(rpnt->next, 0, sizeof(*(rpnt->next)));
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
			struct stat st;
			char *preload;

			if (!_dl_stat(LDSO_PRELOAD, &st)) {
				if ((fd = _dl_open(LDSO_PRELOAD, O_RDONLY)) < 0) {
					_dl_fprintf(2, "%s: can't open file '%s'\n", 
						_dl_progname, LDSO_PRELOAD);
				} else {
					preload = (caddr_t) _dl_mmap(0, st.st_size + 1, 
						PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
					_dl_close(fd);
					if (preload == (caddr_t) - 1) {
						_dl_fprintf(2, "%s: can't map file '%s'\n", 
							_dl_progname, LDSO_PRELOAD);
					} else {
						char c, *cp, *cp2;

						/* convert all separators and comments to spaces */
						for (cp = preload; *cp; /*nada */ ) {
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
							/*nada */ ;

						while (*cp) {
							/* find end of library */
							for (cp2 = cp; *cp && *cp != ' '; cp++)
								/*nada */ ;
							c = *cp;
							*cp = '\0';

							tpnt1 = _dl_load_shared_library(0, NULL, cp2);
							if (!tpnt1) {
#ifdef DL_TRACE
								if (_dl_trace_loaded_objects)
									_dl_fprintf(1, "\t%s => not found\n", cp2);
								else {
#endif
									_dl_fprintf(2, "%s: can't load library '%s'\n", 
										_dl_progname, cp2);
									_dl_exit(15);
#ifdef DL_TRACE
						}
#endif
							} else {
#ifdef DL_TRACE
								if (_dl_trace_loaded_objects
									&& !tpnt1->usage_count) {
									_dl_fprintf(1, "\t%s => %s (0x%x)\n", cp2, 
										tpnt1->libname, (unsigned) tpnt1->loadaddr);
								}
#endif
								rpnt->next = (struct dyn_elf *)
									_dl_malloc(sizeof(struct dyn_elf));
								_dl_memset(rpnt->next, 0, 
									sizeof(*(rpnt->next)));
								rpnt = rpnt->next;
								tpnt1->usage_count++;
								tpnt1->symbol_scope = _dl_symbol_tables;
								tpnt1->libtype = elf_lib;
								rpnt->dyn = tpnt1;
							}

							/* find start of next library */
							*cp = c;
							for ( /*nada */ ; *cp && *cp == ' '; cp++)
								/*nada */ ;
						}

						_dl_munmap(preload, st.st_size + 1);
					}
				}
			}
		}

		for (tcurr = _dl_loaded_modules; tcurr; tcurr = tcurr->next) {
			for (dpnt = (Elf32_Dyn *) tcurr->dynamic_addr; dpnt->d_tag;
				 dpnt++) {
				if (dpnt->d_tag == DT_NEEDED) {
					lpnt = tcurr->loadaddr + tcurr->dynamic_info[DT_STRTAB] +
						dpnt->d_un.d_val;
					if (tpnt && _dl_strcmp(lpnt, 
						    _dl_get_last_path_component(tpnt->libname)) == 0) {
						struct elf_resolve *ttmp;

#ifdef DL_TRACE
						if (_dl_trace_loaded_objects && !tpnt->usage_count) {
						    _dl_fprintf(1, "\t%s => %s (0x%x)\n", 
							    lpnt, tpnt->libname, (unsigned) tpnt->loadaddr);
						}
#endif
						ttmp = _dl_loaded_modules;
						while (ttmp->next)
							ttmp = ttmp->next;
						ttmp->next = tpnt;
						tpnt->prev = ttmp;
						tpnt->next = NULL;
						rpnt->next = (struct dyn_elf *)
							_dl_malloc(sizeof(struct dyn_elf));
						_dl_memset(rpnt->next, 0, sizeof(*(rpnt->next)));
						rpnt = rpnt->next;
						rpnt->dyn = tpnt;
						tpnt->usage_count++;
						tpnt->symbol_scope = _dl_symbol_tables;
						tpnt = NULL;
						continue;
					}
					if (!(tpnt1 = _dl_load_shared_library(0, tcurr, lpnt))) {
#ifdef DL_TRACE
						if (_dl_trace_loaded_objects)
							_dl_fprintf(1, "\t%s => not found\n", lpnt);
						else {
#endif
							_dl_fprintf(2, "%s: can't load library '%s'\n", 
								_dl_progname, lpnt);
							_dl_exit(16);
#ifdef DL_TRACE
						}
#endif
					} else {
#ifdef DL_TRACE
						if (_dl_trace_loaded_objects && !tpnt1->usage_count)
							_dl_fprintf(1, "\t%s => %s (0x%x)\n", lpnt, tpnt1->libname, 
								(unsigned) tpnt1->loadaddr);
#endif
						rpnt->next = (struct dyn_elf *)
							_dl_malloc(sizeof(struct dyn_elf));
						_dl_memset(rpnt->next, 0, sizeof(*(rpnt->next)));
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
#ifdef DL_TRACE
	if (_dl_trace_loaded_objects) {
		char *_dl_warn = 0;
		_dl_warn = _dl_getenv("LD_WARN", envp);
		if (!_dl_warn)
			_dl_exit(0);
	}
#endif

	/*
	 * If the program interpreter is not in the module chain, add it.  This will
	 * be required for dlopen to be able to access the internal functions in the 
	 * dynamic linker.
	 */
	if (tpnt) {
		struct elf_resolve *tcurr;

		tcurr = _dl_loaded_modules;
		if (tcurr)
			while (tcurr->next)
				tcurr = tcurr->next;
		tpnt->next = NULL;
		tpnt->usage_count++;

		if (tcurr) {
			tcurr->next = tpnt;
			tpnt->prev = tcurr;
		} else {
			_dl_loaded_modules = tpnt;
			tpnt->prev = NULL;
		}
		if (rpnt) {
			rpnt->next = (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
			_dl_memset(rpnt->next, 0, sizeof(*(rpnt->next)));
			rpnt = rpnt->next;
		} else {
			rpnt = (struct dyn_elf *) _dl_malloc(sizeof(struct dyn_elf));
			_dl_memset(rpnt, 0, sizeof(*(rpnt->next)));
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
#ifdef DL_TRACE
	if (goof || _dl_trace_loaded_objects)
		_dl_exit(0);
#endif

	/* OK, at this point things are pretty much ready to run.  Now we
	   need to touch up a few items that are required, and then
	   we can let the user application have at it.  Note that
	   the dynamic linker itself is not guaranteed to be fully
	   dynamicly linked if we are using ld.so.1, so we have to look
	   up each symbol individually. */


	_dl_brkp = (unsigned long *) _dl_find_hash("___brk_addr", NULL, 1, NULL, 0);
	if (_dl_brkp)
		*_dl_brkp = brk_addr;
	_dl_envp =
		(unsigned long *) _dl_find_hash("__environ", NULL, 1, NULL, 0);

	if (_dl_envp)
		*_dl_envp = (unsigned long) envp;
	{
		int i;
		elf_phdr *ppnt;

		/* We had to set the protections of all pages to R/W for dynamic linking.
		   Set text pages back to R/O */
		for (tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next)
			for (ppnt = tpnt->ppnt, i = 0; i < tpnt->n_phent; i++, ppnt++)
				if (ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W) &&
					tpnt->dynamic_info[DT_TEXTREL])
					_dl_mprotect((void *) (tpnt->loadaddr + (ppnt->p_vaddr & 0xfffff000)), 
						(ppnt->p_vaddr & 0xfff) + (unsigned long) ppnt->p_filesz, 
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
	((void (*)(void)) debug_addr->r_brk) ();

	for (tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next) {
		/* Apparently crt1 for the application is responsible for handling this.
		 * We only need to run the init/fini for shared libraries
		 */
		if (tpnt->libtype == program_interpreter ||
			tpnt->libtype == elf_executable)
			continue;
		if (tpnt->init_flag & INIT_FUNCS_CALLED)
			continue;
		tpnt->init_flag |= INIT_FUNCS_CALLED;

		if (tpnt->dynamic_info[DT_INIT]) {
			_dl_elf_init = (int (*)(void)) (tpnt->loadaddr + 
				tpnt->dynamic_info[DT_INIT]);
			(*_dl_elf_init) ();
		}
		if (_dl_atexit && tpnt->dynamic_info[DT_FINI]) {
			(*_dl_atexit) (tpnt->loadaddr + tpnt->dynamic_info[DT_FINI]);
		}
#undef DL_DEBUG
#ifdef DL_DEBUG
		else {
			_dl_fprintf(2, tpnt->libname);
			_dl_fprintf(2, ": ");
			if (!_dl_atexit)
				_dl_fprintf(2, "The address is atexit () is 0x0.");
			if (!tpnt->dynamic_info[DT_FINI])
				_dl_fprintf(2, "Invalid .fini section.");
			_dl_fprintf(2, "\n");
		}
#endif
#undef DL_DEBUG
	}

	/* OK we are done here.  Turn out the lights, and lock up. */
	_dl_elf_main = (int (*)(int, char **, char **)) auxv_t[AT_ENTRY].a_un.a_fcn;


	/*
	 * Transfer control to the application.
	 */
	status = 0; /* Used on x86, but not on other arches */
	START();
}

/*
 * This stub function is used by some debuggers.  The idea is that they
 * can set an internal breakpoint on it, so that we are notified when the
 * address mapping is changed in some way.
 */
void _dl_debug_state()
{
	return;
}

int _dl_fixup(struct elf_resolve *tpnt)
{
	int goof = 0;
	
	if (tpnt->next)
		goof += _dl_fixup(tpnt->next);
	if (tpnt->dynamic_info[DT_REL]) {
#ifdef ELF_USES_RELOCA
		_dl_fprintf(2, "%s: can't handle REL relocation records\n", 
			_dl_progname);
		_dl_exit(17);
#else
		if (tpnt->init_flag & RELOCS_DONE)
			return goof;
		tpnt->init_flag |= RELOCS_DONE;
		goof += _dl_parse_relocation_information(tpnt, 
			tpnt->dynamic_info[DT_REL], tpnt->dynamic_info[DT_RELSZ], 0);
#endif
	}
	if (tpnt->dynamic_info[DT_RELA]) {
#ifdef ELF_USES_RELOCA
		if (tpnt->init_flag & RELOCS_DONE)
			return goof;
		tpnt->init_flag |= RELOCS_DONE;
		goof += _dl_parse_relocation_information(tpnt, 
			tpnt->dynamic_info[DT_RELA], tpnt->dynamic_info[DT_RELASZ], 0);
#else
		_dl_fprintf(2, "%s: can't handle RELA relocation records\n", 
			_dl_progname);
		_dl_exit(18);
#endif
	}
	if (tpnt->dynamic_info[DT_JMPREL]) {
		if (tpnt->init_flag & JMP_RELOCS_DONE)
			return goof;
		tpnt->init_flag |= JMP_RELOCS_DONE;
		if (!_dl_not_lazy || *_dl_not_lazy == 0)
			_dl_parse_lazy_relocation_information(tpnt, 
				tpnt->dynamic_info[DT_JMPREL], tpnt->dynamic_info[DT_PLTRELSZ], 0);
		else
			goof += _dl_parse_relocation_information(tpnt, 
				tpnt->dynamic_info[DT_JMPREL], tpnt->dynamic_info[DT_PLTRELSZ], 0);
	}
	return goof;
}

void *_dl_malloc(int size)
{
	void *retval;

#ifdef DL_DEBUG
	SEND_STDERR("malloc: request for ");
	SEND_NUMBER_STDERR(size, 0);
	SEND_STDERR(" bytes\n");
#endif	

	if (_dl_malloc_function)
		return (*_dl_malloc_function) (size);

	if (_dl_malloc_addr - _dl_mmap_zero + size > 4096) {
#ifdef DL_DEBUG
		SEND_STDERR("malloc: mmapping more memory\n");
#endif	
		_dl_mmap_zero = _dl_malloc_addr = _dl_mmap((void *) 0, size, 
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (_dl_mmap_check_error(_dl_mmap_zero)) {
			_dl_fprintf(2, "%s: mmap of a spare page failed!\n", _dl_progname);
			_dl_exit(20);
		}
	}
	retval = _dl_malloc_addr;
	_dl_malloc_addr += size;

	/*
	 * Align memory to 4 byte boundary.  Some platforms require this, others
	 * simply get better performance.
	 */
	_dl_malloc_addr = (char *) (((unsigned long) _dl_malloc_addr + 3) & ~(3));
	return retval;
}

char *_dl_getenv(char *symbol, char **envp)
{
	char *pnt;
	char *pnt1;

	while ((pnt = *envp++)) {
		pnt1 = symbol;
		while (*pnt && *pnt == *pnt1)
			pnt1++, pnt++;
		if (!*pnt || *pnt != '=' || *pnt1)
			continue;
		return pnt + 1;
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
		if (!*pnt || *pnt != '=' || *pnt1)
			*newenvp++ = *envp;
	}
	*newenvp++ = *envp;
	return;
}

char *_dl_strdup(const char *string)
{
	char *retval;
	int len;

	len = _dl_strlen(string);
	retval = _dl_malloc(len + 1);
	_dl_strcpy(retval, string);
	return retval;
}

char *_dl_get_last_path_component(char *path)
{
	char *s;

	s=path+_dl_strlen(path)-1;

	/* strip trailing slashes */
	while (s != path && *s == '/') {
		*s-- = '\0';
	}

	/* find last component */
	s = _dl_strrchr(path, '/');
	if (s == NULL || s[1] == '\0')
		return path;
	else
		return s+1;
}

size_t _dl_strlen(const char * str)
{
	register char *ptr = (char *) str;

	while (*ptr)
		ptr++;
	return (ptr - str);
}

char * _dl_strcpy(char * dst,const char *src)
{
	register char *ptr = dst;

	while (*src)
		*dst++ = *src++;
	*dst = '\0';

	return ptr;
}
 
int _dl_strcmp(const char * s1,const char * s2)
{
	unsigned register char c1, c2;

	do {
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0')
			return c1 - c2;
	}
	while (c1 == c2);

	return c1 - c2;
}

int _dl_strncmp(const char * s1,const char * s2,size_t len)
{
	unsigned register char c1 = '\0';
	unsigned register char c2 = '\0';

	while (len > 0) {
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0' || c1 != c2)
			return c1 - c2;
		len--;
	}

	return c1 - c2;
}

char * _dl_strchr(const char * str,int c)
{
	register char ch;

	do {
		if ((ch = *str) == c)
			return (char *) str;
		str++;
	}
	while (ch);

	return 0;
}

char *_dl_strrchr(const char *str, int c)
{
    register char *prev = 0;
    register char *ptr = (char *) str;

    while (*ptr != '\0') {
	if (*ptr == c)
	    prev = ptr;
	ptr++;  
    }   
    if (c == '\0')
	return(ptr);
    return(prev);
}

void * _dl_memcpy(void * dst, const void * src, size_t len)
{
	register char *a = dst;
	register const char *b = src;

	while (len--)
		*a++ = *b++;

	return dst;
}

void * _dl_memset(void * str,int c,size_t len)
{
	register char *a = str;

	while (len--)
		*a++ = c;

	return str;
}

/* Minimum printf which handles only characters, %d's and %s's */
void _dl_fprintf(int fd, const char *fmt, ...)
{
    int num;
    va_list args;
    char *start, *ptr, *string;
    char buf[2048];

    start = ptr = buf;
    
    if (!fmt)
	return;

    if (_dl_strlen(fmt) >= (sizeof(buf)-1))
	_dl_write(fd, "(overflow)\n", 10);

    _dl_strcpy(buf, fmt);
    va_start(args, fmt);

    while(start)
    {
	while(*ptr != '%' && *ptr) { 
	    ptr++;
	}

	if(*ptr == '%')
	{
	    *ptr++ = '\0';
	    _dl_write(fd, start, _dl_strlen(start));

	    switch(*ptr++)
	    {
		case 's':
		    string = va_arg(args, char *);
		    if (!string)
			_dl_write(fd, "(null)", 6);
		    else
			_dl_write(fd, string, _dl_strlen(string));
		    break;

		case 'i':
		case 'd':
		    {
			char tmp[13];
			num = va_arg(args, int);
			string = _dl_simple_ltoa_inline(tmp, num);
			_dl_write(fd, string, _dl_strlen(string));
			break;
		    }
		case 'x':
		case 'X':
		    {
			char tmp[13];
			num = va_arg(args, int);
			string = _dl_simple_ltoahex_inline(tmp, num);
			_dl_write(fd, string, _dl_strlen(string));
			break;
		    }
		default:
			_dl_write(fd, "(null)", 6);
			break;
	    }

	    start = ptr;
	}
	else
	{
	    _dl_write(fd, start, _dl_strlen(start));
	    start = NULL;
	}
    }
    return;
}

