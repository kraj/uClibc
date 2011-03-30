/* vi: set sw=4 ts=4: */
/* Licensed under the LGPL v2.1, see the file LICENSE in this tarball. */

#include <ldso.h>

/*
 * dynamic_info is of type ElfW(Word)
 * DL_RESOLVER_TYPE is ElfW(Addr) for non FDPIC
 */

#ifdef ELF_MACHINE_JMP_SLOT
DL_RESOLVER_TYPE _dl_linux_resolver(struct elf_resolve *tpnt, const ElfW(Word) reloc_entry)
{
	char *new_addr;
	char **got_addr;

	const ElfW(Sym) *const symtab = (const void *)tpnt->dynamic_info[DT_SYMTAB];
	const char *strtab = (const void *)tpnt->dynamic_info[DT_STRTAB];

	const ELF_RELOC *const this_reloc = (const void *)(tpnt->dynamic_info[DT_JMPREL] + reloc_entry);
	const int symtab_index = ELF_R_SYM(this_reloc->r_info);
	const char *symname = strtab + symtab[symtab_index].st_name;
	void *const instr_addr = (void *)DL_RELOC_ADDR(tpnt->loadaddr, this_reloc->r_offset);

	debug_sym(symtab, strtab, symtab_index);
	debug_reloc(symtab, strtab, this_reloc);

	/* Sanity check */
	_dl_assert(ELF_R_TYPE(this_reloc->r_info) != ELF_MACHINE_JMP_SLOT);

	got_addr = (char **)instr_addr;

# ifdef __SUPPORT_LD_DEBUG__
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tResolving symbol '%s' %x --> ",
			    symname, (ElfW(Addr))got_addr);
# endif

	/* Get the address of the GOT entry */
	new_addr = _dl_find_hash(symname, tpnt->symbol_scope, tpnt, ELF_RTYPE_CLASS_PLT, NULL);
	if (unlikely(!new_addr)) {
# ifdef __SUPPORT_LD_DEBUG__
		_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
			    _dl_progname, symname, tpnt->libname);
# else
		_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
			    _dl_progname, symname);
# endif
		_dl_exit(1);
	}

# ifdef __SUPPORT_LD_DEBUG__
#  ifdef __sh__
#   define SOME_ADDR 0x20000000
#  else
#   define SOME_ADDR 0x40000000
#  endif
#  ifndef __SUPPORT_LD_DEBUG_EARLY__
	if ((ElfW(Addr))got_addr < SOME_ADDR)
#  endif
	{
		if (_dl_debug_bindings) {
			_dl_dprintf(_dl_debug_file, "\nresolve function: %s", symname);
			if (_dl_debug_detail)
				_dl_dprintf(_dl_debug_file, "\n\tpatched: %p ==> %p @ %p",
					    *got_addr, new_addr, got_addr);
		}
	}
	if (!_dl_debug_nofixups)
# endif
# ifdef DL_GOT_ADDR
		DL_GOT_ADDR(got_addr, new_addr)
# else
		*got_addr = new_addr;
# endif

	return (DL_RESOLVER_TYPE)new_addr;
}
#endif

#ifndef __mips__
static int _dl_parse(struct elf_resolve *tpnt, struct dyn_elf *scope,
		     ElfW(Addr) rel_addr, ElfW(Word) rel_size,
		     int (*reloc_fnc)(struct elf_resolve *tpnt, struct dyn_elf *scope,
				      ELF_RELOC *rpnt, const ElfW(Sym) *const symtab,
				      const char *strtab))
{
	const ElfW(Sym) *const symtab = (const void *)tpnt->dynamic_info[DT_SYMTAB];
	const char *strtab = (const void *)tpnt->dynamic_info[DT_STRTAB];

	/* Parse the relocation information */
	ELF_RELOC *rpnt = (ELF_RELOC *)rel_addr;
	rel_size /= sizeof(ELF_RELOC);

	for (unsigned int i = 0; i < rel_size; i++, rpnt++) {
		int res;

		const int symtab_index = ELF_R_SYM(rpnt->r_info);

		debug_sym(symtab, strtab, symtab_index);
		debug_reloc(symtab, strtab, rpnt);

		res = reloc_fnc(tpnt, scope, rpnt, symtab, strtab);

		if (res == 0)
			continue;

		_dl_dprintf(2, "\n%s: ", _dl_progname);

		if (symtab_index)
			_dl_dprintf(2, "symbol '%s': ", strtab + symtab[symtab_index].st_name);

		if (unlikely(res < 0)) {
			const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
# ifdef __SUPPORT_LD_DEBUG__
			_dl_dprintf(2, "can't handle reloc type '%s' in lib '%s'\n",
				    _dl_reltypes(reloc_type), tpnt->libname);
# else
			_dl_dprintf(2, "can't handle reloc type %x\n",
				    reloc_type);
# endif
			_dl_exit(-res);
		}
		if (unlikely(res > 0)) {
# ifdef __SUPPORT_LD_DEBUG__
			_dl_dprintf(2, "can't resolve symbol in lib '%s'\n", tpnt->libname);
# else
			_dl_dprintf(2, "can't resolve symbol\n");
# endif
			return res;
		}
	}
	return 0;
}

int _dl_parse_relocation_information(struct dyn_elf *rpnt,
				     ElfW(Addr) rel_addr,
				     ElfW(Word) rel_size)
{
	return _dl_parse(rpnt->dyn, rpnt->dyn->symbol_scope, rel_addr, rel_size, _dl_do_reloc);
}
#endif

#if defined ELF_MACHINE_NONE && defined ELF_MACHINE_JMP_SLOT
static __always_inline int _dl_do_lazy_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope attribute_unused,
					     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab attribute_unused,
					     const char *strtab attribute_unused)
{
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)DL_RELOC_ADDR(tpnt->loadaddr, rpnt->r_offset);
# ifdef __SUPPORT_LD_DEBUG__
	ElfW(Addr) old_val = *reloc_addr;
# endif

	switch (reloc_type) {
		case ELF_MACHINE_NONE:
			break;
		case ELF_MACHINE_JMP_SLOT:
			*reloc_addr += tpnt->loadaddr;
			break;
		default:
			return -1;
	}

# ifdef __SUPPORT_LD_DEBUG__
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p",
			    old_val, *reloc_addr, reloc_addr);
# endif

	return 0;
}
#endif

#ifndef ARCH_HAS_LAZY
void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
					   ElfW(Addr) rel_addr,
					   ElfW(Word) rel_size)
{
# if defined ELF_MACHINE_NONE && defined ELF_MACHINE_JMP_SLOT
	(void)_dl_parse(rpnt->dyn, NULL, rel_addr, rel_size, _dl_do_lazy_reloc);
# else
	_dl_parse_relocation_information(rpnt, rel_addr, rel_size);
# endif
}
#endif
