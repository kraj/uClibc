/* Load an ELF sharable library into memory.

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



/* This file contains the helper routines to load an ELF sharable
   library into memory and add the symbol table info to the chain. */

#ifdef USE_CACHE

static caddr_t _dl_cache_addr = NULL;
static size_t _dl_cache_size = 0;

int _dl_map_cache(void)
{
	int fd;
	struct stat st;
	header_t *header;
	libentry_t *libent;
	int i, strtabsize;

	if (_dl_cache_addr == (caddr_t) - 1)
		return -1;
	else if (_dl_cache_addr != NULL)
		return 0;

	if (_dl_stat(LDSO_CACHE, &st)
		|| (fd = _dl_open(LDSO_CACHE, O_RDONLY)) < 0) {
		_dl_dprintf(2, "%s: can't open cache '%s'\n", _dl_progname, LDSO_CACHE);
		_dl_cache_addr = (caddr_t) - 1;	/* so we won't try again */
		return -1;
	}

	_dl_cache_size = st.st_size;
	_dl_cache_addr = (caddr_t) _dl_mmap(0, _dl_cache_size, PROT_READ, MAP_SHARED, fd, 0);
	_dl_close(fd);
	if (_dl_cache_addr == (caddr_t) - 1) {
		_dl_dprintf(2, "%s: can't map cache '%s'\n", 
			_dl_progname, LDSO_CACHE);
		return -1;
	}

	header = (header_t *) _dl_cache_addr;

	if (_dl_cache_size < sizeof(header_t) ||
		_dl_memcmp(header->magic, LDSO_CACHE_MAGIC, LDSO_CACHE_MAGIC_LEN)
		|| _dl_memcmp(header->version, LDSO_CACHE_VER, LDSO_CACHE_VER_LEN)
		|| _dl_cache_size <
		(sizeof(header_t) + header->nlibs * sizeof(libentry_t))
		|| _dl_cache_addr[_dl_cache_size - 1] != '\0') 
	{
		_dl_dprintf(2, "%s: cache '%s' is corrupt\n", _dl_progname, 
			LDSO_CACHE);
		goto fail;
	}

	strtabsize = _dl_cache_size - sizeof(header_t) -
		header->nlibs * sizeof(libentry_t);
	libent = (libentry_t *) & header[1];

	for (i = 0; i < header->nlibs; i++) {
		if (libent[i].sooffset >= strtabsize || 
			libent[i].liboffset >= strtabsize) 
		{
			_dl_dprintf(2, "%s: cache '%s' is corrupt\n", _dl_progname, LDSO_CACHE);
			goto fail;
		}
	}

	return 0;

  fail:
	_dl_munmap(_dl_cache_addr, _dl_cache_size);
	_dl_cache_addr = (caddr_t) - 1;
	return -1;
}

int _dl_unmap_cache(void)
{
	if (_dl_cache_addr == NULL || _dl_cache_addr == (caddr_t) - 1)
		return -1;

#if 1
	_dl_munmap(_dl_cache_addr, _dl_cache_size);
	_dl_cache_addr = NULL;
#endif

	return 0;
}

#endif

/* This function's behavior must exactly match that 
 * in uClibc/ldso/util/ldd.c */
static struct elf_resolve * 
search_for_named_library(char *name, int secure, const char *path_list)
{
	int i, count = 0;
	char *path, *path_n;
	char mylibname[2050];
	struct elf_resolve *tpnt1;


	/* We need a writable copy of this string */
	path = _dl_strdup(path_list);
	if (!path) {
		_dl_dprintf(2, "Out of memory!\n");
		_dl_exit(0);
	}
	

	/* Unlike ldd.c, don't bother to eliminate double //s */


	/* Replace colons with zeros in path_list and count them */
	for(i=_dl_strlen(path); i > 0; i--) {
		if (path[i]==':') {
			path[i]=0;
			count++;
		}
	}

	path_n = path;
	for (i = 0; i < count; i++) {
		_dl_strcpy(mylibname, path_n); 
		_dl_strcat(mylibname, "/"); 
		_dl_strcat(mylibname, name);
		if ((tpnt1 = _dl_load_elf_shared_library(secure, mylibname, 0)) != NULL)
		    return tpnt1;
		path_n += (_dl_strlen(path_n) + 1);
	}
	return NULL;
}


/*
 * Used to return error codes back to dlopen et. al.
 */

unsigned long _dl_error_number;
unsigned long _dl_internal_error_number;

struct elf_resolve *_dl_load_shared_library(int secure, 
	struct elf_resolve *tpnt, char *full_libname)
{
	char *pnt;
	struct elf_resolve *tpnt1;
	char *libname;

	_dl_internal_error_number = 0;

	/* quick hack to ensure mylibname buffer doesn't overflow.  don't 
	   allow full_libname or any directory to be longer than 1024. */
	if (_dl_strlen(full_libname) > 1024)
		goto goof;

	pnt = libname = full_libname;
	while (*pnt) {
		if (*pnt == '/')
			libname = pnt + 1;
		pnt++;
	}

	/* If the filename has any '/', try it straight and leave it at that.
	   For IBCS2 compatibility under linux, we substitute the string 
	   /usr/i486-sysv4/lib for /usr/lib in library names. */

	if (libname != full_libname) {
		tpnt1 = _dl_load_elf_shared_library(secure, full_libname, 0);
		if (tpnt1)
			return tpnt1;
		goto goof;
	}

	/*
	 * The ABI specifies that RPATH is searched before LD_*_PATH or
	 * the default path of /usr/lib.  Check in rpath directories.
	 */
	for (tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next) {
		if (tpnt->libtype == elf_executable) {
			pnt = (char *) tpnt->dynamic_info[DT_RPATH];
			if (pnt) {
				pnt += (unsigned long) tpnt->loadaddr +
					tpnt->dynamic_info[DT_STRTAB];
				if ((tpnt1 = search_for_named_library(libname, secure, pnt)) != NULL) 
				{
				    return tpnt1;
				}
			}
		}
	}

	/* Check in LD_{ELF_}LIBRARY_PATH, if specified and allowed */
	if (_dl_library_path) {
	    if ((tpnt1 = search_for_named_library(libname, secure, _dl_library_path)) != NULL) 
	    {
		return tpnt1;
	    }
	}

	/*
	 * Where should the cache be searched?  There is no such concept in the
	 * ABI, so we have some flexibility here.  For now, search it before
	 * the hard coded paths that follow (i.e before /lib and /usr/lib).
	 */
#ifdef USE_CACHE
	if (_dl_cache_addr != NULL && _dl_cache_addr != (caddr_t) - 1) {
		int i;
		header_t *header = (header_t *) _dl_cache_addr;
		libentry_t *libent = (libentry_t *) & header[1];
		char *strs = (char *) &libent[header->nlibs];

		for (i = 0; i < header->nlibs; i++) {
			if ((libent[i].flags == LIB_ELF ||
				 libent[i].flags == LIB_ELF_LIBC5) &&
				_dl_strcmp(libname, strs + libent[i].sooffset) == 0 &&
				(tpnt1 = _dl_load_elf_shared_library(secure, 
				     strs + libent[i].liboffset, 0)))
				return tpnt1;
		}
	}
#endif

	/* Lastly, search the standard list of paths for the library.
	   This list must exactly match the list in uClibc/ldso/util/ldd.c */
	if ((tpnt1 = search_for_named_library(libname, secure, 
			UCLIBC_TARGET_PREFIX "/usr/lib:"
			UCLIBC_TARGET_PREFIX "/lib:"
			UCLIBC_DEVEL_PREFIX "/lib:"
			UCLIBC_BUILD_DIR "/lib:"
			"/usr/lib:"
			"/lib")
		    ) != NULL) 
	{
	    return tpnt1;
	}

  goof:
	/* Well, we shot our wad on that one.  All we can do now is punt */
	if (_dl_internal_error_number)
		_dl_error_number = _dl_internal_error_number;
	else
		_dl_error_number = DL_ERROR_NOFILE;
	return NULL;
}

/*
 * Read one ELF library into memory, mmap it into the correct locations and
 * add the symbol info to the symbol chain.  Perform any relocations that
 * are required.
 */

//extern _elf_rtbndr(void);

struct elf_resolve *_dl_load_elf_shared_library(int secure, 
	char *libname, int flag)
{
	elfhdr *epnt;
	unsigned long dynamic_addr = 0;
	unsigned long dynamic_size = 0;
	Elf32_Dyn *dpnt;
	struct elf_resolve *tpnt;
	elf_phdr *ppnt;
	int piclib;
	char *status;
	int flags;
	char header[4096];
	unsigned long dynamic_info[24];
	int *lpnt;
	unsigned long libaddr;
	unsigned long minvma = 0xffffffff, maxvma = 0;

	int i;
	int infile;

	/* If this file is already loaded, skip this step */
	tpnt = _dl_check_hashed_files(libname);
	if (tpnt)
		return tpnt;

	/* If we are in secure mode (i.e. a setu/gid binary using LD_PRELOAD),
	   we don't load the library if it isn't setuid. */

	if (secure) {
		struct stat st;

		if (_dl_stat(libname, &st) || !(st.st_mode & S_ISUID))
			return NULL;
	}

	libaddr = 0;
	infile = _dl_open(libname, O_RDONLY);
	if (infile < 0) {
#if 0
		/*
		 * NO!  When we open shared libraries we may search several paths.
		 * it is inappropriate to generate an error here.
		 */
		_dl_dprintf(2, "%s: can't open '%s'\n", _dl_progname, libname);
#endif
		_dl_internal_error_number = DL_ERROR_NOFILE;
		return NULL;
	}

	_dl_read(infile, header, sizeof(header));
	epnt = (elfhdr *) header;
	if (epnt->e_ident[0] != 0x7f ||
		epnt->e_ident[1] != 'E' || 
		epnt->e_ident[2] != 'L' || 
		epnt->e_ident[3] != 'F') 
	{
		_dl_dprintf(2, "%s: '%s' is not an ELF file\n", _dl_progname,
					 libname);
		_dl_internal_error_number = DL_ERROR_NOTELF;
		_dl_close(infile);
		return NULL;
	};

	if ((epnt->e_type != ET_DYN) || (epnt->e_machine != MAGIC1 
#ifdef MAGIC2
		    && epnt->e_machine != MAGIC2
#endif
		)) 
	{
		_dl_internal_error_number = 
		    (epnt->e_type != ET_DYN ? DL_ERROR_NOTDYN : DL_ERROR_NOTMAGIC);
		_dl_dprintf(2, "%s: '%s' is not an ELF executable for " ELF_TARGET 
			"\n", _dl_progname, libname);
		_dl_close(infile);
		return NULL;
	};

	ppnt = (elf_phdr *) & header[epnt->e_phoff];

	piclib = 1;
	for (i = 0; i < epnt->e_phnum; i++) {

		if (ppnt->p_type == PT_DYNAMIC) {
			if (dynamic_addr)
				_dl_dprintf(2, "%s: '%s' has more than one dynamic section\n", 
					_dl_progname, libname);
			dynamic_addr = ppnt->p_vaddr;
			dynamic_size = ppnt->p_filesz;
		};

		if (ppnt->p_type == PT_LOAD) {
			/* See if this is a PIC library. */
			if (i == 0 && ppnt->p_vaddr > 0x1000000) {
				piclib = 0;
				minvma = ppnt->p_vaddr;
			}
			if (piclib && ppnt->p_vaddr < minvma) {
				minvma = ppnt->p_vaddr;
			}
			if (((unsigned long) ppnt->p_vaddr + ppnt->p_memsz) > maxvma) {
				maxvma = ppnt->p_vaddr + ppnt->p_memsz;
			}
		}
		ppnt++;
	};

	maxvma = (maxvma + 0xfffU) & ~0xfffU;
	minvma = minvma & ~0xffffU;

	flags = MAP_PRIVATE /*| MAP_DENYWRITE */ ;
	if (!piclib)
		flags |= MAP_FIXED;

	status = (char *) _dl_mmap((char *) (piclib ? 0 : minvma), 
		maxvma - minvma, PROT_NONE, flags | MAP_ANONYMOUS, -1, 0);
	if (_dl_mmap_check_error(status)) {
		_dl_dprintf(2, "%s: can't map %s\n", _dl_progname, libname);
		_dl_internal_error_number = DL_ERROR_MMAP_FAILED;
		_dl_close(infile);
		return NULL;
	};
	libaddr = (unsigned long) status;
	flags |= MAP_FIXED;

	/* Get the memory to store the library */
	ppnt = (elf_phdr *) & header[epnt->e_phoff];

	for (i = 0; i < epnt->e_phnum; i++) {
		if (ppnt->p_type == PT_LOAD) {

			/* See if this is a PIC library. */
			if (i == 0 && ppnt->p_vaddr > 0x1000000) {
				piclib = 0;
				/* flags |= MAP_FIXED; */
			}



			if (ppnt->p_flags & PF_W) {
				unsigned long map_size;
				char *cpnt;

				status = (char *) _dl_mmap((char *) ((piclib ? libaddr : 0) + 
					(ppnt->p_vaddr & 0xfffff000)), (ppnt->p_vaddr & 0xfff) 
					+ ppnt->p_filesz, LXFLAGS(ppnt->p_flags), flags, infile, 
					ppnt->p_offset & 0x7ffff000);

				if (_dl_mmap_check_error(status)) {
					_dl_dprintf(2, "%s: can't map '%s'\n", 
						_dl_progname, libname);
					_dl_internal_error_number = DL_ERROR_MMAP_FAILED;
					_dl_munmap((char *) libaddr, maxvma - minvma);
					_dl_close(infile);
					return NULL;
				};

				/* Pad the last page with zeroes. */
				cpnt = (char *) (status + (ppnt->p_vaddr & 0xfff) +
							  ppnt->p_filesz);
				while (((unsigned long) cpnt) & 0xfff)
					*cpnt++ = 0;

				/* I am not quite sure if this is completely
				 * correct to do or not, but the basic way that
				 * we handle bss segments is that we mmap
				 * /dev/zero if there are any pages left over
				 * that are not mapped as part of the file */

				map_size = (ppnt->p_vaddr + ppnt->p_filesz + 0xfff) & 0xfffff000;
				if (map_size < ppnt->p_vaddr + ppnt->p_memsz)
					status = (char *) _dl_mmap((char *) map_size + 
						(piclib ? libaddr : 0), 
						ppnt->p_vaddr + ppnt->p_memsz - map_size, 
						LXFLAGS(ppnt->p_flags), flags | MAP_ANONYMOUS, -1, 0);
			} else
				status = (char *) _dl_mmap((char *) (ppnt->p_vaddr & 0xfffff000) 
					+ (piclib ? libaddr : 0), (ppnt->p_vaddr & 0xfff) + 
					ppnt->p_filesz, LXFLAGS(ppnt->p_flags), flags, 
					infile, ppnt->p_offset & 0x7ffff000);
			if (_dl_mmap_check_error(status)) {
				_dl_dprintf(2, "%s: can't map '%s'\n", _dl_progname, libname);
				_dl_internal_error_number = DL_ERROR_MMAP_FAILED;
				_dl_munmap((char *) libaddr, maxvma - minvma);
				_dl_close(infile);
				return NULL;
			};

			/* if(libaddr == 0 && piclib) {
			   libaddr = (unsigned long) status;
			   flags |= MAP_FIXED;
			   }; */
		};
		ppnt++;
	};
	_dl_close(infile);

	/* For a non-PIC library, the addresses are all absolute */
	if (piclib) {
		dynamic_addr += (unsigned long) libaddr;
	}

	/* 
	 * OK, the ELF library is now loaded into VM in the correct locations
	 * The next step is to go through and do the dynamic linking (if needed).
	 */

	/* Start by scanning the dynamic section to get all of the pointers */

	if (!dynamic_addr) {
		_dl_internal_error_number = DL_ERROR_NODYNAMIC;
		_dl_dprintf(2, "%s: '%s' is missing a dynamic section\n", 
			_dl_progname, libname);
		return NULL;
	}

	dpnt = (Elf32_Dyn *) dynamic_addr;

	dynamic_size = dynamic_size / sizeof(Elf32_Dyn);
	_dl_memset(dynamic_info, 0, sizeof(dynamic_info));
	for (i = 0; i < dynamic_size; i++) {
		if (dpnt->d_tag > DT_JMPREL) {
			dpnt++;
			continue;
		}
		dynamic_info[dpnt->d_tag] = dpnt->d_un.d_val;
		if (dpnt->d_tag == DT_TEXTREL || SVR4_BUGCOMPAT)
			dynamic_info[DT_TEXTREL] = 1;
		dpnt++;
	};

	/* If the TEXTREL is set, this means that we need to make the pages
	   writable before we perform relocations.  Do this now. They get set back
	   again later. */

	if (dynamic_info[DT_TEXTREL]) {
		ppnt = (elf_phdr *) & header[epnt->e_phoff];
		for (i = 0; i < epnt->e_phnum; i++, ppnt++) {
			if (ppnt->p_type == PT_LOAD && !(ppnt->p_flags & PF_W))
				_dl_mprotect((void *) ((piclib ? libaddr : 0) + 
					    (ppnt->p_vaddr & 0xfffff000)), 
					(ppnt->p_vaddr & 0xfff) + (unsigned long) ppnt->p_filesz, 
					PROT_READ | PROT_WRITE | PROT_EXEC);
		}
	}


	tpnt = _dl_add_elf_hash_table(libname, (char *) libaddr, dynamic_info, 
		dynamic_addr, dynamic_size);

	tpnt->ppnt = (elf_phdr *) (tpnt->loadaddr + epnt->e_phoff);
	tpnt->n_phent = epnt->e_phnum;

	/*
	 * OK, the next thing we need to do is to insert the dynamic linker into
	 * the proper entry in the GOT so that the PLT symbols can be properly
	 * resolved. 
	 */

	lpnt = (int *) dynamic_info[DT_PLTGOT];

	if (lpnt) {
		lpnt = (int *) (dynamic_info[DT_PLTGOT] + ((int) libaddr));
		INIT_GOT(lpnt, tpnt);
	};

	return tpnt;
}

/* Ugly, ugly.  Some versions of the SVr4 linker fail to generate COPY
   relocations for global variables that are present both in the image and
   the shared library.  Go through and do it manually.  If the images
   are guaranteed to be generated by a trustworthy linker, then this
   step can be skipped. */

int _dl_copy_fixups(struct dyn_elf *rpnt)
{
	int goof = 0;
	struct elf_resolve *tpnt;

	if (rpnt->next)
		goof += _dl_copy_fixups(rpnt->next);
	else
		return 0;

	tpnt = rpnt->dyn;

	if (tpnt->init_flag & COPY_RELOCS_DONE)
		return goof;
	tpnt->init_flag |= COPY_RELOCS_DONE;

#ifdef ELF_USES_RELOCA
	goof += _dl_parse_copy_information(rpnt, 
		tpnt->dynamic_info[DT_RELA], tpnt->dynamic_info[DT_RELASZ], 0);

#else
	goof += _dl_parse_copy_information(rpnt, tpnt->dynamic_info[DT_REL], 
		tpnt->dynamic_info[DT_RELSZ], 0);

#endif
	return goof;
}
