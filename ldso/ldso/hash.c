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



/* Various symbol table handling functions, including symbol lookup */

/*
 * This is the start of the linked list that describes all of the files present
 * in the system with pointers to all of the symbol, string, and hash tables, 
 * as well as all of the other good stuff in the binary.
 */

struct elf_resolve *_dl_loaded_modules = NULL;

/*
 * This is the list of modules that are loaded when the image is first
 * started.  As we add more via dlopen, they get added into other
 * chains.
 */
struct dyn_elf *_dl_symbol_tables = NULL;

/*
 * This is the list of modules that are loaded via dlopen.  We may need
 * to search these for RTLD_GLOBAL files.
 */
struct dyn_elf *_dl_handles = NULL;


/*
 * This is the hash function that is used by the ELF linker to generate
 * the hash table that each executable and library is required to
 * have.  We need it to decode the hash table.
 */

unsigned long _dl_elf_hash(const char *name)
{
	unsigned long hash = 0;
	unsigned long tmp;

	while (*name) {
		hash = (hash << 4) + *name++;
		if ((tmp = hash & 0xf0000000))
			hash ^= tmp >> 24;
		hash &= ~tmp;
	};
	return hash;
}

/*
 * Check to see if a library has already been added to the hash chain.
 */
struct elf_resolve *_dl_check_hashed_files(char *libname)
{
	struct elf_resolve *tpnt;
	int len = _dl_strlen(libname);

	for (tpnt = _dl_loaded_modules; tpnt; tpnt = tpnt->next) {
		if (_dl_strncmp(tpnt->libname, libname, len) == 0 &&
			(tpnt->libname[len] == '\0' || tpnt->libname[len] == '.'))
			return tpnt;
	}

	return NULL;
}

/*
 * We call this function when we have just read an ELF library or executable.
 * We add the relevant info to the symbol chain, so that we can resolve all
 * externals properly.
 */

struct elf_resolve *_dl_add_elf_hash_table(char *libname, 
	char *loadaddr, unsigned long *dynamic_info, unsigned long dynamic_addr, 
	unsigned long dynamic_size)
{
	unsigned long *hash_addr;
	struct elf_resolve *tpnt;
	int i;

	if (!_dl_loaded_modules) {
		tpnt = _dl_loaded_modules = 
		    (struct elf_resolve *) _dl_malloc(sizeof(struct elf_resolve));
		_dl_memset(tpnt, 0, sizeof(*tpnt));
	} else {
		tpnt = _dl_loaded_modules;
		while (tpnt->next)
			tpnt = tpnt->next;
		tpnt->next = (struct elf_resolve *) _dl_malloc(sizeof(struct elf_resolve));
		_dl_memset(tpnt->next, 0, sizeof(*(tpnt->next)));
		tpnt->next->prev = tpnt;
		tpnt = tpnt->next;
	};

	tpnt->next = NULL;
	tpnt->init_flag = 0;
	tpnt->libname = _dl_strdup(libname);
	tpnt->dynamic_addr = dynamic_addr;
	tpnt->dynamic_size = dynamic_size;
	tpnt->libtype = loaded_file;

	if (dynamic_info[DT_HASH] != 0) {
		hash_addr = (unsigned long *) (dynamic_info[DT_HASH] + loadaddr);
		tpnt->nbucket = *hash_addr++;
		tpnt->nchain = *hash_addr++;
		tpnt->elf_buckets = hash_addr;
		hash_addr += tpnt->nbucket;
		tpnt->chains = hash_addr;
	}
	tpnt->loadaddr = loadaddr;
	for (i = 0; i < 24; i++)
		tpnt->dynamic_info[i] = dynamic_info[i];
	return tpnt;
}


/*
 * This function resolves externals, and this is either called when we process
 * relocations or when we call an entry in the PLT table for the first time.
 */

char *_dl_find_hash(char *name, struct dyn_elf *rpnt1, 
	struct elf_resolve *f_tpnt, int copyrel)
{
	struct elf_resolve *tpnt;
	int si;
	char *pnt;
	int pass;
	char *strtab;
	Elf32_Sym *symtab;
	unsigned long elf_hash_number, hn;
	char *weak_result;
	struct elf_resolve *first_def;
	struct dyn_elf *rpnt, first;
	char *data_result = 0;		/* nakao */

	weak_result = 0;
	elf_hash_number = _dl_elf_hash(name);

	/* A quick little hack to make sure that any symbol in the executable
	   will be preferred to one in a shared library.  This is necessary so
	   that any shared library data symbols referenced in the executable
	   will be seen at the same address by the executable, shared libraries
	   and dynamically loaded code. -Rob Ryan (robr@cmu.edu) */
	if (!copyrel && rpnt1) {
		first = (*_dl_symbol_tables);
		first.next = rpnt1;
		rpnt1 = (&first);
	}

	/*
	 * The passes are so that we can first search the regular symbols
	 * for whatever module was specified, and then search anything
	 * loaded with RTLD_GLOBAL.  When pass is 1, it means we are just
	 * starting the first dlopened module, and anything above that
	 * is just the next one in the chain.
	 */
	for (pass = 0; (1 == 1); pass++) {

		/*
		 * If we are just starting to search for RTLD_GLOBAL, setup
		 * the pointer for the start of the search.
		 */
		if (pass == 1) {
			rpnt1 = _dl_handles;
		}

		/*
		 * Anything after this, we need to skip to the next module.
		 */
		else if (pass >= 2) {
			rpnt1 = rpnt1->next_handle;
		}

		/*
		 * Make sure we still have a module, and make sure that this
		 * module was loaded with RTLD_GLOBAL.
		 */
		if (pass != 0) {
			if (rpnt1 == NULL)
				break;
			if ((rpnt1->flags & RTLD_GLOBAL) == 0)
				continue;
		}

		for (rpnt = (rpnt1 ? rpnt1 : _dl_symbol_tables); rpnt; rpnt = rpnt->next) {
			tpnt = rpnt->dyn;

			/*
			 * The idea here is that if we are using dlsym, we want to
			 * first search the entire chain loaded from dlopen, and
			 * return a result from that if we found anything.  If this
			 * fails, then we continue the search into the stuff loaded
			 * when the image was activated.  For normal lookups, we start
			 * with rpnt == NULL, so we should never hit this.  
			 */
			if (tpnt->libtype == elf_executable && weak_result != 0) {
				break;
			}

			/*
			 * Avoid calling .urem here.
			 */
			do_rem(hn, elf_hash_number, tpnt->nbucket);
			symtab = (Elf32_Sym *) (tpnt->dynamic_info[DT_SYMTAB] + tpnt->loadaddr);
			strtab = (char *) (tpnt->dynamic_info[DT_STRTAB] + tpnt->loadaddr);
			/*
			 * This crap is required because the first instance of a
			 * symbol on the chain will be used for all symbol references.
			 * Thus this instance must be resolved to an address that
			 * contains the actual function, 
			 */

			first_def = NULL;

			for (si = tpnt->elf_buckets[hn]; si; si = tpnt->chains[si]) {
				pnt = strtab + symtab[si].st_name;

				if (_dl_strcmp(pnt, name) == 0 &&
					(ELF32_ST_TYPE(symtab[si].st_info) == STT_FUNC ||
					 ELF32_ST_TYPE(symtab[si].st_info) == STT_NOTYPE ||
					 ELF32_ST_TYPE(symtab[si].st_info) == STT_OBJECT) &&
					symtab[si].st_value != 0) {

					/* Here we make sure that we find a module where the symbol is
					 * actually defined.
					 */

					if (f_tpnt) {
						if (!first_def)
							first_def = tpnt;
						if (first_def == f_tpnt
							&& symtab[si].st_shndx == 0)
							continue;
					}

					switch (ELF32_ST_BIND(symtab[si].st_info)) {
					case STB_GLOBAL:
						if (tpnt->libtype != elf_executable && 
							ELF32_ST_TYPE(symtab[si].st_info) 
							== STT_NOTYPE) 
						{	/* nakao */
							data_result = tpnt->loadaddr + 
							    symtab[si].st_value;	/* nakao */
							break;	/* nakao */
						} else	/* nakao */
							return tpnt->loadaddr + symtab[si].st_value;
					case STB_WEAK:
						if (!weak_result)
							weak_result =
								tpnt->loadaddr + symtab[si].st_value;
						break;
					default:	/* Do local symbols need to be examined? */
						break;
					}
				}
			}
		}
	}
	if (data_result)
		return data_result;		/* nakao */
	return weak_result;
}
