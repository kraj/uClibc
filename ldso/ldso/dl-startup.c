/* vi: set sw=4 ts=4: */
/*
 * Program to load an ELF binary on a linux system, and run it
 * after resolving ELF shared library symbols
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald,
 *				David Engel, Hongjiu Lu and Mitch D'Souza
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * The main trick with this program is that initially, we ourselves are not
 * dynamicly linked.  This means that we cannot access any global variables or
 * call any functions.  No globals initially, since the Global Offset Table
 * (GOT) is initialized by the linker assuming a virtual address of 0, and no
 * function calls initially since the Procedure Linkage Table (PLT) is not yet
 * initialized.
 *
 * There are additional initial restrictions - we cannot use large switch
 * statements, since the compiler generates tables of addresses and jumps
 * through them.  We cannot use normal syscall stubs, because these all
 * reference the errno global variable which is not yet initialized.  We _can_
 * use all of the local stack variables that we want.  We _can_ use inline
 * functions, because these do not transfer control to a new address, but they
 * must be static so that they are not exported from the modules.
 *
 * Life is further complicated by the fact that initially we do not want to do
 * a complete dynamic linking.  We want to allow the user to supply new
 * functions to override symbols (i.e. weak symbols and/or LD_PRELOAD).  So
 * initially, we only perform relocations for variables that start with "_dl_"
 * since ANSI specifies that the user is not supposed to redefine any of these
 * variables.
 *
 * Fortunately, the linker itself leaves a few clues lying around, and when the
 * kernel starts the image, there are a few further clues.  First of all, there
 * is Auxiliary Vector Table information sitting on which is provided to us by
 * the kernel, and which includes information about the load address that the
 * program interpreter was loaded at, the number of sections, the address the
 * application was loaded at and so forth.  Here this information is stored in
 * the array auxvt.  For details see linux/fs/binfmt_elf.c where it calls
 * NEW_AUX_ENT() a bunch of time....
 *
 * Next, we need to find the GOT.  On most arches there is a register pointing
 * to the GOT, but just in case (and for new ports) I've added some (slow) C
 * code to locate the GOT for you.
 *
 * This code was originally written for SVr4, and there the kernel would load
 * all text pages R/O, so they needed to call mprotect a zillion times to mark
 * all text pages as writable so dynamic linking would succeed.  Then when they
 * were done, they would change the protections for all the pages back again.
 * Well, under Linux everything is loaded writable (since Linux does copy on
 * write anyways) so all the mprotect stuff has been disabled.
 *
 * Initially, we do not have access to _dl_malloc since we can't yet make
 * function calls, so we mmap one page to use as scratch space.  Later on, when
 * we can call _dl_malloc we reuse this this memory.  This is also beneficial,
 * since we do not want to use the same memory pool as malloc anyway - esp if
 * the user redefines malloc to do something funky.
 *
 * Our first task is to perform a minimal linking so that we can call other
 * portions of the dynamic linker.  Once we have done this, we then build the
 * list of modules that the application requires, using LD_LIBRARY_PATH if this
 * is not a suid program (/usr/lib otherwise).  Once this is done, we can do
 * the dynamic linking as required, and we must omit the things we did to get
 * the dynamic linker up and running in the first place.  After we have done
 * this, we just have a few housekeeping chores and we can transfer control to
 * the user's application.
 */

#include "ldso.h"

/* Pull in all the arch specific stuff */
#include "dl-startup.h"

/* Static declarations */
int (*_dl_elf_main) (int, char **, char **);




/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
		NULL
        env[0...N]      environment variables (pointers)
        NULL
		auxvt[0...N]   Auxiliary Vector Table elements (mixed types)
*/
DL_BOOT(unsigned long args)
{
	unsigned int argc;
	char **argv, **envp;
	unsigned long load_addr;
	unsigned long *got;
	unsigned long *aux_dat;
	int goof = 0;
	ElfW(Ehdr) *header;
	struct elf_resolve tpnt_tmp;
	struct elf_resolve *tpnt = &tpnt_tmp;
	Elf32_auxv_t auxvt[AT_EGID + 1];
	Elf32_Dyn *dpnt;
	int indx;
#if defined(__i386__)
	int status = 0;
#endif



	/* WARNING! -- we cannot make _any_ funtion calls until we have
	 * taken care of fixing up our own relocations.  Making static
	 * inline calls is ok, but _no_ function calls.  Not yet
	 * anyways. */

	/* First obtain the information on the stack that tells us more about
	   what binary is loaded, where it is loaded, etc, etc */
	GET_ARGV(aux_dat, args);
#if defined (__arm__) || defined (__mips__) || defined (__cris__)
	aux_dat += 1;
#endif
	argc = *(aux_dat - 1);
	argv = (char **) aux_dat;
	aux_dat += argc;			/* Skip over the argv pointers */
	aux_dat++;					/* Skip over NULL at end of argv */
	envp = (char **) aux_dat;
	while (*aux_dat)
		aux_dat++;				/* Skip over the envp pointers */
	aux_dat++;					/* Skip over NULL at end of envp */

	/* Place -1 here as a checkpoint.  We later check if it was changed
	 * when we read in the auxvt */
	auxvt[AT_UID].a_type = -1;

	/* The junk on the stack immediately following the environment is
	 * the Auxiliary Vector Table.  Read out the elements of the auxvt,
	 * sort and store them in auxvt for later use. */
	while (*aux_dat) {
		Elf32_auxv_t *auxv_entry = (Elf32_auxv_t *) aux_dat;

		if (auxv_entry->a_type <= AT_EGID) {
			_dl_memcpy(&(auxvt[auxv_entry->a_type]), auxv_entry, sizeof(Elf32_auxv_t));
		}
		aux_dat += 2;
	}

	/* locate the ELF header.   We need this done as soon as possible
	 * (esp since SEND_STDERR() needs this on some platforms... */
	load_addr = auxvt[AT_BASE].a_un.a_val;
	header = (ElfW(Ehdr) *) auxvt[AT_BASE].a_un.a_ptr;

	/* Check the ELF header to make sure everything looks ok.  */
	if (!header || header->e_ident[EI_CLASS] != ELFCLASS32 ||
			header->e_ident[EI_VERSION] != EV_CURRENT
			/* Do not use an inline _dl_strncmp here or some arches
			* will blow chunks, i.e. those that need to relocate all
			* string constants... */
			|| header->e_ident[EI_MAG0] != ELFMAG0
			|| header->e_ident[EI_MAG1] != ELFMAG1
			|| header->e_ident[EI_MAG2] != ELFMAG2
			|| header->e_ident[EI_MAG3] != ELFMAG3)
	{
		SEND_STDERR("Invalid ELF header\n");
		_dl_exit(0);
	}
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("ELF header=");
	SEND_ADDRESS_STDERR(load_addr, 1);
#endif


	/* Locate the global offset table.  Since this code must be PIC
	 * we can take advantage of the magic offset register, if we
	 * happen to know what that is for this architecture.  If not,
	 * we can always read stuff out of the ELF file to find it... */
#if defined(__i386__)
	__asm__("\tmovl %%ebx,%0\n\t":"=a"(got));
#elif defined(__m68k__)
	__asm__("movel %%a5,%0":"=g"(got));
#elif defined(__sparc__)
	__asm__("\tmov %%l7,%0\n\t":"=r"(got));
#elif defined(__arm__)
	__asm__("\tmov %0, r10\n\t":"=r"(got));
#elif defined(__powerpc__)
	__asm__("\tbl _GLOBAL_OFFSET_TABLE_-4@local\n\t":"=l"(got));
#elif defined(__mips__)
	__asm__("\tmove %0, $28\n\tsubu %0,%0,0x7ff0\n\t":"=r"(got));
#elif defined(__sh__) && !defined(__SH5__)
	__asm__(
			"       mov.l    1f, %0\n"
			"       mova     1f, r0\n"
			"       bra      2f\n"
			"       add r0,  %0\n"
			"       .balign  4\n"
			"1:     .long    _GLOBAL_OFFSET_TABLE_\n"
			"2:" : "=r" (got) : : "r0");
#elif defined(__cris__)
	__asm__("\tmove.d $pc,%0\n\tsub.d .:GOTOFF,%0\n\t":"=r"(got));
#else
	/* Do things the slow way in C */
	{
		unsigned long tx_reloc;
		Elf32_Dyn *dynamic = NULL;
		Elf32_Shdr *shdr;
		Elf32_Phdr *pt_load;

#ifdef __SUPPORT_LD_DEBUG_EARLY__
		SEND_STDERR("Finding the GOT using C code to read the ELF file\n");
#endif
		/* Find where the dynamic linking information section is hiding */
		shdr = (Elf32_Shdr *) (header->e_shoff + (char *) header);
		for (indx = header->e_shnum; --indx >= 0; ++shdr) {
			if (shdr->sh_type == SHT_DYNAMIC) {
				goto found_dynamic;
			}
		}
		SEND_STDERR("missing dynamic linking information section \n");
		_dl_exit(0);

found_dynamic:
		dynamic = (Elf32_Dyn *) (shdr->sh_offset + (char *) header);

		/* Find where PT_LOAD is hiding */
		pt_load = (Elf32_Phdr *) (header->e_phoff + (char *) header);
		for (indx = header->e_phnum; --indx >= 0; ++pt_load) {
			if (pt_load->p_type == PT_LOAD) {
				goto found_pt_load;
			}
		}
		SEND_STDERR("missing loadable program segment\n");
		_dl_exit(0);

found_pt_load:
		/* Now (finally) find where DT_PLTGOT is hiding */
		tx_reloc = pt_load->p_vaddr - pt_load->p_offset;
		for (; DT_NULL != dynamic->d_tag; ++dynamic) {
			if (dynamic->d_tag == DT_PLTGOT) {
				goto found_got;
			}
		}
		SEND_STDERR("missing global offset table\n");
		_dl_exit(0);

found_got:
		got = (unsigned long *) (dynamic->d_un.d_val - tx_reloc +
				(char *) header);
	}
#endif

	/* Now, finally, fix up the location of the dynamic stuff */
	dpnt = (Elf32_Dyn *) (*got + load_addr);
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("First Dynamic section entry=");
	SEND_ADDRESS_STDERR(dpnt, 1);
#endif
	_dl_memset(tpnt, 0, sizeof(struct elf_resolve));

	/* OK, that was easy.  Next scan the DYNAMIC section of the image.
	   We are only doing ourself right now - we will have to do the rest later */
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("scanning DYNAMIC section\n");
#endif
	tpnt->dynamic_addr = dpnt;
#ifdef __mips__
	/* MIPS cannot call functions here, must inline */
	__dl_parse_dynamic_info(dpnt, tpnt->dynamic_info, NULL);
#else
	_dl_parse_dynamic_info(dpnt, tpnt->dynamic_info, NULL);
#endif

#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("done scanning DYNAMIC section\n");
#endif

#ifndef __FORCE_SHAREABLE_TEXT_SEGMENTS__
	/* Ugly, ugly.  We need to call mprotect to change the protection of
	   the text pages so that we can do the dynamic linking.  We can set the
	   protection back again once we are done */

	{
		ElfW(Phdr) *ppnt;
		int i;

#ifdef __SUPPORT_LD_DEBUG_EARLY__
		SEND_STDERR("calling mprotect on the shared library/dynamic linker\n");
#endif

		/* First cover the shared library/dynamic linker. */
		if (tpnt->dynamic_info[DT_TEXTREL]) {
			header = (ElfW(Ehdr) *) auxvt[AT_BASE].a_un.a_ptr;
			ppnt = (ElfW(Phdr) *) ((int)auxvt[AT_BASE].a_un.a_ptr +
					header->e_phoff);
			for (i = 0; i < header->e_phnum; i++, ppnt++) {
				if (ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W)) {
					_dl_mprotect((void *) (load_addr + (ppnt->p_vaddr & PAGE_ALIGN)),
							(ppnt->p_vaddr & ADDR_ALIGN) + (unsigned long) ppnt->p_filesz,
							PROT_READ | PROT_WRITE | PROT_EXEC);
				}
			}
		}
	}
#endif
#if defined(__mips__)
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("About to do MIPS specific GOT bootstrap\n");
#endif
	/* For MIPS we have to do stuff to the GOT before we do relocations.  */
	PERFORM_BOOTSTRAP_GOT(got, tpnt);
#endif

	/* OK, now do the relocations.  We do not do a lazy binding here, so
	   that once we are done, we have considerably more flexibility. */
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("About to do library loader relocations\n");
#endif
#ifdef  ELF_MACHINE_PLTREL_OVERLAP
# define INDX_MAX 1
#else
# define INDX_MAX 2
#endif
	goof = 0;
	for (indx = 0; indx < INDX_MAX; indx++) {
		unsigned int i;
		ELF_RELOC *rpnt;
		unsigned long *reloc_addr;
		unsigned long symbol_addr;
		int symtab_index;
		unsigned long rel_addr, rel_size;
		Elf32_Sym *sym;

		rel_addr = (indx ? tpnt->dynamic_info[DT_JMPREL] : tpnt->
				dynamic_info[DT_RELOC_TABLE_ADDR]);
		rel_size = (indx ? tpnt->dynamic_info[DT_PLTRELSZ] : tpnt->
				dynamic_info[DT_RELOC_TABLE_SIZE]);

		if (!rel_addr)
			continue;

		/* Now parse the relocation information */
		rpnt = (ELF_RELOC *) (rel_addr + load_addr);
		for (i = 0; i < rel_size; i += sizeof(ELF_RELOC), rpnt++) {
			reloc_addr = (unsigned long *) (load_addr + (unsigned long) rpnt->r_offset);
			symtab_index = ELF32_R_SYM(rpnt->r_info);
			symbol_addr = 0;
			sym = NULL;
			if (symtab_index) {
				char *strtab;
				Elf32_Sym *symtab;

				symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + load_addr);
				strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + load_addr);
				sym = &symtab[symtab_index];
				symbol_addr = load_addr + sym->st_value;

#ifdef __SUPPORT_LD_DEBUG_EARLY__
				SEND_STDERR("relocating symbol: ");
				SEND_STDERR(strtab + sym->st_name);
				SEND_STDERR("\n");
#endif
			}
			/* Use this machine-specific macro to perform the actual relocation.  */
			PERFORM_BOOTSTRAP_RELOC(rpnt, reloc_addr, symbol_addr, load_addr, sym);
		}
	}

	if (goof) {
		_dl_exit(14);
	}

#ifdef __SUPPORT_LD_DEBUG_EARLY__
	/* Wahoo!!! */
	SEND_STDERR("Done relocating library loader, so we can now\n"
			"\tuse globals and make function calls!\n");
#endif

	/* Now we have done the mandatory linking of some things.  We are now
	   free to start using global variables, since these things have all been
	   fixed up by now.  Still no function calls outside of this library ,
	   since the dynamic resolver is not yet ready. */
	_dl_get_ready_to_run(tpnt, load_addr, auxvt, envp, argv);


	/* Transfer control to the application.  */
#ifdef __SUPPORT_LD_DEBUG_EARLY__
	SEND_STDERR("transfering control to application\n");
#endif
	_dl_elf_main = (int (*)(int, char **, char **)) auxvt[AT_ENTRY].a_un.a_fcn;
	START();
}

