/* vi: set sw=4 ts=4: */
/*
 * A small little ldd implementation for uClibc
 *
 * Copyright (C) 2001 by Lineo, inc.
 * Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 *
 * Several functions in this file (specifically, elf_find_section_type(),
 * elf_find_phdr_type(), and elf_find_dynamic(), were stolen from elflib.c from
 * elfvector (http://www.BitWagon.com/elfvector.html) by John F. Reiser
 * <jreiser@BitWagon.com>, and which is copyright 2000 BitWagon Software LLC
 * (GPL2).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */


#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

struct library {
	char *name;
	int resolved;
	char *path;
	struct library *next;
};
struct library *lib_list = NULL;
char not_found[] = "not found";



Elf32_Shdr * elf_find_section_type( int key, Elf32_Ehdr *ehdr)
{
	int j;
	Elf32_Shdr *shdr = (Elf32_Shdr *)(ehdr->e_shoff + (char *)ehdr);
	for (j = ehdr->e_shnum; --j>=0; ++shdr) {
		if (shdr->sh_type == key) {
			return shdr;
		}
	}
	return NULL;
}

Elf32_Phdr * elf_find_phdr_type( int type, Elf32_Ehdr *ehdr)
{
	int j;
	Elf32_Phdr *phdr = (Elf32_Phdr *)(ehdr->e_phoff + (char *)ehdr);
	for (j = ehdr->e_phnum; --j>=0; ++phdr) {
		if (type==phdr->p_type) {
			return phdr;
		}
	}
	return NULL;
}

/* Returns value if return_val==1, ptr otherwise */ 
void * elf_find_dynamic(int const key, Elf32_Dyn *dynp, 
	Elf32_Ehdr *ehdr, int return_val)
{
	Elf32_Phdr *pt_text = elf_find_phdr_type(PT_LOAD, ehdr);
	unsigned tx_reloc = pt_text->p_vaddr - pt_text->p_offset;
	for (; DT_NULL!=dynp->d_tag; ++dynp) {
		if (dynp->d_tag == key) {
			if (return_val == 1)
				return (void *)dynp->d_un.d_val;
			else
				return (void *)(dynp->d_un.d_val - tx_reloc + (char *)ehdr );
		}
	}
	return NULL;
}

int check_elf_header(Elf32_Ehdr const *const ehdr)
{
	if (! ehdr || strncmp((void *)ehdr, ELFMAG, SELFMAG) != 0 ||  
			ehdr->e_ident[EI_CLASS] != ELFCLASS32 ||
			ehdr->e_ident[EI_VERSION] != EV_CURRENT) 
	{
		return 1;
	}
	return 0;
}

/* This function's behavior must exactly match that 
 * in uClibc/ldso/d-link/readelflib1.c */
static void search_for_named_library(char *name, char *result, const char *path_list)
{
	int i, count = 0;
	char *path, *path_n;
	struct stat filestat;

	/* We need a writable copy of this string */
	path = strdup(path_list);
	if (!path) {
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}
	/* Eliminate all double //s */
	path_n=path;
	while((path_n=strstr(path_n, "//"))) {
		i = strlen(path_n);
		memmove(path_n, path_n+1, i-1);
	}

	/* Replace colons with zeros in path_list and count them */
	for(i=strlen(path); i > 0; i--) {
		if (path[i]==':') {
			path[i]=0;
			count++;
		}
	}

	path_n = path;
	for (i = 0; i < count; i++) {
		*result = '\0';
		strcat(result, path_n); 
		strcat(result, "/"); 
		strcat(result, name);
		if (stat (result, &filestat) == 0 && filestat.st_mode & S_IRUSR) {
			free(path);
			return;
		}
		path_n += (strlen(path_n) + 1);
	}
	free(path);
	*result = '\0';
}

void locate_library_file(Elf32_Ehdr* ehdr, Elf32_Dyn* dynamic, char *strtab, int is_suid, struct library *lib)
{
	char *buf;
	char *path;
	struct stat filestat;
	
	/* If this is a fully resolved name, our job is easy */
	if (stat (lib->name, &filestat) == 0) {
		lib->path = lib->name;
		return;
	}

	/* We need some elbow room here.  Make some room...*/
	buf = malloc(1024);
	if (!buf) {
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	/* This function must match the behavior of _dl_load_shared_library
	 * in readelflib1.c or things won't work out as expected... */

	/* The ABI specifies that RPATH is searched first, so do that now.  */
	path = (char *)elf_find_dynamic(DT_RPATH, dynamic, ehdr, 0);
	if (path) {
		search_for_named_library(lib->name, buf, path);
		if (*buf != '\0') {
			lib->path = buf;
			return;
		} else {
			free(buf);
		}
	}

	/* Next check LD_{ELF_}LIBRARY_PATH if specified and allowed.
	 * Since this app doesn't actually run an executable I will skip
	 * the suid check, and just use LD_{ELF_}LIBRARY_PATH if set */
	if (is_suid==1)
		path = NULL;
	else
		path = getenv("LD_LIBRARY_PATH");
	if (path) {
		search_for_named_library(lib->name, buf, path);
		if (*buf != '\0') {
			lib->path = buf;
			return;
		} else {
			free(buf);
		}
	}

#ifdef USE_CACHE
	/* FIXME -- add code to check the Cache here */ 
#endif

	/* Lastly, search the standard list of paths for the library.
	   This list must exactly match the list in uClibc/ldso/d-link/readelflib1.c */
	path =	UCLIBC_TARGET_PREFIX "/usr/lib:"
			UCLIBC_TARGET_PREFIX "/lib:"
			UCLIBC_DEVEL_PREFIX "/lib:"
			UCLIBC_BUILD_DIR "/lib:"
			"/usr/lib:"
			"/lib";
	search_for_named_library(lib->name, buf, path);
	if (*buf != '\0') {
		lib->path = buf;
	} else { 
		free(buf);
		printf("bad stuff\n");
		lib->path = not_found;
	}
}

static int add_library(Elf32_Ehdr* ehdr, Elf32_Dyn* dynamic, char *strtab, int is_setuid, const char *s)
{
	struct library *cur, *prev, *newlib=lib_list;

	if (!s || !strlen(s))
		return 1;

	/* We add libc.so.0 elsewhere */
	if (strcmp(s, UCLIBC_LDSO)==0)
		return 1;

	for (cur = lib_list; cur; cur=cur->next) {
		if(strcmp(cur->name, s)==0) {
			/* Lib is already in the list */
			return 0;
		}
	}

	/* Ok, this lib needs to be added to the list */
	newlib = malloc(sizeof(struct library));
	if (!newlib)
		return 1;
	newlib->name = malloc(strlen(s));
	strcpy(newlib->name, s);
	newlib->resolved = 0;
	newlib->path = NULL;
	newlib->next = NULL;

	/* Now try and locate where this library might be living... */
	locate_library_file(ehdr, dynamic, strtab, is_setuid, newlib);

	//printf("adding '%s' to '%s'\n", newlib->name, newlib->path);
	if (!lib_list) {
		lib_list = newlib;
	} else {
		for (cur = prev = lib_list;  cur->next; prev=cur, cur=cur->next); /* nothing */
		cur = newlib;
		prev->next = cur;
	}
	return 0;
}


static void find_needed_libraries(Elf32_Ehdr* ehdr, Elf32_Dyn* dynamic, char *strtab, int is_setuid)
{
	Elf32_Dyn  *dyns;

	for (dyns=dynamic; dyns->d_tag!=DT_NULL; ++dyns) {
		if (dyns->d_tag == DT_NEEDED) {
			add_library(ehdr, dynamic, strtab, is_setuid, (char*)strtab + dyns->d_un.d_val);
		}
	}
}
    
static void find_elf_interpreter(Elf32_Ehdr* ehdr, Elf32_Dyn* dynamic, char *strtab, int is_setuid)
{
	Elf32_Phdr *phdr;
	phdr = elf_find_phdr_type(PT_INTERP, ehdr);
	if (phdr) {
		struct library *cur, *prev, *newlib=lib_list;
		char *s = (char*)ehdr + phdr->p_offset;

		for (cur = lib_list; cur; cur=cur->next) {
			if(strcmp(cur->name, s)==0) {
				/* Lib is already in the list */
				return;
			}
		}
		newlib = malloc(sizeof(struct library));
		if (!newlib)
			return;
		newlib->name = malloc(strlen(s));
		strcpy(newlib->name, s);
		newlib->path = newlib->name;
		newlib->resolved = 1;
		newlib->next = NULL;

		//printf("adding '%s' to '%s'\n", newlib->name, newlib->path);
		if (!lib_list) {
			lib_list = newlib;
		} else {
			for (cur = prev = lib_list;  cur->next; prev=cur, cur=cur->next); /* nothing */
			cur = newlib;
			prev->next = cur;
		}
	}
}

/* map the .so, and locate interesting pieces */
int find_dependancies(char* filename)
{
	int is_suid = 0;
	FILE *thefile;
	struct stat statbuf;
	char *dynstr=NULL;
	Elf32_Ehdr *ehdr = NULL;
	Elf32_Shdr *dynsec = NULL;
	Elf32_Dyn *dynamic = NULL;

	if (filename == not_found)
		return 0;

	if (!filename) {
		fprintf(stderr, "No filename specified.\n");
		exit(EXIT_FAILURE);
	}
	if (!(thefile = fopen(filename, "r"))) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	if (fstat(fileno(thefile), &statbuf) < 0) {
		perror(filename);
		exit(EXIT_FAILURE);
	}

	if (statbuf.st_size < sizeof(Elf32_Ehdr))
		goto foo;

	/* mmap the file to make reading stuff from it effortless */
	ehdr = (Elf32_Ehdr *)mmap(0, statbuf.st_size, 
			PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(thefile), 0);

foo:
	/* Check if this looks like a legit ELF file */
	if (check_elf_header(ehdr)) {
		fprintf(stderr, "%s: not an ELF file.\n", filename);
		exit(EXIT_FAILURE);
	}
	/* Check if this is the right kind of ELF file */
	if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
		fprintf(stderr, "%s: not a dynamic executable\n", filename);
		exit(EXIT_FAILURE);
	}
	if (ehdr->e_type == ET_EXEC) {
		if (statbuf.st_mode & S_ISUID)
			is_suid = 1;
		if ((statbuf.st_mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP))
			is_suid = 1;
		/* FIXME */
		if (is_suid)
			fprintf(stderr, "%s: is setuid\n", filename);
	}

	dynsec = elf_find_section_type(SHT_DYNAMIC, ehdr);
	if (dynsec) {
		dynamic = (Elf32_Dyn*)(dynsec->sh_offset + (int)ehdr);
		dynstr = (char *)elf_find_dynamic(DT_STRTAB, dynamic, ehdr, 0);
		find_needed_libraries(ehdr, dynamic, dynstr, is_suid);
	}
	find_elf_interpreter(ehdr, dynamic, dynstr, is_suid);
	
	return 0;
}



int main( int argc, char** argv)
{
	int got_em_all=1;
	char *filename = argv[1];
	struct library *cur;


	if (!filename) {
		fprintf(stderr, "No filename specified.\n");
		exit(EXIT_FAILURE);
	}

	find_dependancies(filename);
	
	while(got_em_all) {
		got_em_all=0;
		/* Keep walking the list till everybody is resolved */
		for (cur = lib_list; cur; cur=cur->next) {
			if (cur->resolved == 0 && cur->path) {
				got_em_all=1;
				//printf("checking sub-depends for '%s\n", cur->path);
				find_dependancies(cur->path);
				cur->resolved = 1;
			}
		}
	}

	
	/* Print the list */
	got_em_all=0;
	for (cur = lib_list; cur; cur=cur->next) {
		got_em_all=1;
		printf("\t%s => %s\n", cur->name, cur->path);
	}
	if (got_em_all==0)
		printf("\tnot a dynamic executable\n");

	return 0;
}

