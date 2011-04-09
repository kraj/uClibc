/* vi: set sw=4 ts=4: */
/* Licensed under the LGPL v2.1, see the file LICENSE in this tarball. */

#include <ldsodefs.h>

/*
 * dynamic_info is of type ElfW(Word)
 * DL_RESOLVER_TYPE is ElfW(Addr) for non FDPIC
 */

#ifdef __FDPIC__
#define DL_RESOLVER_PTR DL_RESOLVER_TYPE
#else
#define DL_RESOLVER_PTR DL_RESOLVER_TYPE *
#endif

#if defined ELF_MACHINE_JMP_SLOT || defined ELF_MACHINE_FUNCDESC
DL_RESOLVER_TYPE _dl_linux_resolver(struct elf_resolve *tpnt, const ElfW(Word) reloc_entry)
{
	char *new_addr;
# ifdef __FDPIC__
	struct funcdesc_value funcval;
	struct funcdesc_value __volatile__ *got;
# else
	char **got;
# endif

	const ElfW(Sym) *const symtab = (const void *)tpnt->dynamic_info[DT_SYMTAB];
	const char *strtab = (const void *)tpnt->dynamic_info[DT_STRTAB];

	const ELF_RELOC *const this_reloc = (const void *)(tpnt->dynamic_info[DT_JMPREL] + reloc_entry);
	const int symtab_index = ELF_R_SYM(this_reloc->r_info);
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
# ifndef __FDPIC__
	void *const instr_addr = (void *)DL_RELOC_ADDR(tpnt->loadaddr, this_reloc->r_offset);
# endif

	debug_sym(symtab, strtab, symtab_index);
	debug_reloc(symtab, strtab, this_reloc);

	/* Sanity check */
# ifdef ELF_MACHINE_JMP_SLOT
	_dl_assert(ELF_R_TYPE(this_reloc->r_info) != ELF_MACHINE_JMP_SLOT);
# endif

# ifdef __FDPIC__
	got = (DL_RESOLVER_TYPE)DL_RELOC_ADDR(tpnt->loadaddr, this_reloc->r_offset);
# else
	got = (char **)instr_addr;
# endif

# ifdef __SUPPORT_LD_DEBUG__
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tResolving symbol '%s' %p --> ",
			    symname, got);
# endif

	/* Get the address of the GOT entry */
# ifdef __FDPIC__
	new_addr = _dl_find_hash(symname, tpnt->symbol_scope, NULL, 0, &sym_ref);
# else
	new_addr = _dl_find_hash(symname, tpnt->symbol_scope, tpnt, ELF_RTYPE_CLASS_PLT, NULL);
# endif
	if (unlikely(!new_addr)) {
# ifdef __FDPIC__
	    new_addr = _dl_find_hash(symname, NULL, NULL, 0, &sym_ref);
	    if (unlikely(!new_addr))
# endif
	    {
# ifdef __SUPPORT_LD_DEBUG__
		_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
			    _dl_progname, symname, tpnt->libname);
# else
		_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
			    _dl_progname, symname);
# endif
		_dl_exit(1);
	    }
	}

# ifdef __FDPIC__
	funcval.entry_point = new_addr;
	funcval.got_value = sym_ref.tpnt->loadaddr.got_value;
# endif

# ifdef __SUPPORT_LD_DEBUG__
#  ifdef __sh__
#   define SOME_ADDR 0x20000000
#  elif !defined __DSBT__ && !defined __FDPIC__
#   define SOME_ADDR 0x40000000
#  endif
#  if !defined __SUPPORT_LD_DEBUG_EARLY__ && defined SOME_ADDR
	if ((ElfW(Addr))got < SOME_ADDR)
#  endif
	{
		if (_dl_debug_bindings) {
			_dl_dprintf(_dl_debug_file, "\nresolve function: %s", symname);
			if (_dl_debug_detail)
				_dl_dprintf(_dl_debug_file,
# ifdef __FDPIC__
					    "\n\tpatched (%p,%p) ==> (%p,%p) @ %p",
					    got->entry_point, got->got_value,
					    funcval.entry_point, funcval.got_value,
# else
					    "\n\tpatched: %p ==> %p @ %p",
					    *got, new_addr,
# endif
					    got);
		}
	}
	if (!_dl_debug_nofixups)
# endif
# ifdef __FDPIC__
		*got = funcval;
	/* psm: this is wrong, it should use probably funcval */
	return got;
# else
#  ifdef DL_GOT_ADDR
		DL_GOT_ADDR(got, new_addr)
#  else
		*got = new_addr;
#  endif
	return (DL_RESOLVER_TYPE)new_addr;
# endif
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
# ifdef DSBT_FLUSH_RELOCS
	DSBT_FLUSH_RELOCS
# endif
	return 0;
}

static int _dl_parse_relocation_information(struct dyn_elf *rpnt,
	ElfW(Addr) rel_addr, ElfW(Word) rel_size)
{
	return _dl_parse(rpnt->dyn, rpnt->dyn->symbol_scope, rel_addr, rel_size, _dl_do_reloc);
}
#endif

#if defined ELF_MACHINE_NONE && (defined ELF_MACHINE_JMP_SLOT || defined ELF_MACHINE_FUNCDESC)
static __always_inline int _dl_do_lazy_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope attribute_unused,
					     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab attribute_unused,
					     const char *strtab attribute_unused)
{
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	DL_RESOLVER_PTR reloc_addr = (DL_RESOLVER_PTR)DL_RELOC_ADDR(tpnt->loadaddr, rpnt->r_offset);
# ifdef __FDPIC__
	struct funcdesc_value funcval;
# endif
# ifdef __SUPPORT_LD_DEBUG__
#  ifdef __FDPIC__
	ElfW(Addr) old_val = (ElfW(Addr))reloc_addr->entry_point;
#  else
	ElfW(Addr) old_val = *reloc_addr;
#  endif
# endif

	switch (reloc_type) {
		case ELF_MACHINE_NONE:
			break;
# ifdef __FDPIC__
		case ELF_MACHINE_FUNCDESC:
				funcval = *reloc_addr;
				funcval.entry_point = (void *)DL_RELOC_ADDR(tpnt->loadaddr, funcval.entry_point);
				funcval.got_value = tpnt->loadaddr.got_value;
				*reloc_addr = funcval;
# else
		case ELF_MACHINE_JMP_SLOT:
			*reloc_addr = DL_RELOC_ADDR(tpnt->loadaddr, *reloc_addr);
# endif
			break;
		default:
			return -1;
	}

# ifdef __SUPPORT_LD_DEBUG__
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p",
#  ifdef __FDPIC__
			    old_val, reloc_addr->entry_point, reloc_addr);
#  else
			    old_val, *reloc_addr, reloc_addr);
#  endif
# endif

	return 0;
}
#endif

#ifndef ARCH_HAS_LAZY
static void _dl_parse_lazy_relocation_information(struct dyn_elf *rpnt,
	ElfW(Addr) rel_addr, ElfW(Word) rel_size)
{
# if defined ELF_MACHINE_NONE && defined ELF_MACHINE_JMP_SLOT
	(void)_dl_parse(rpnt->dyn, NULL, rel_addr, rel_size, _dl_do_lazy_reloc);
# else
	_dl_parse_relocation_information(rpnt, rel_addr, rel_size);
# endif
}
#endif
