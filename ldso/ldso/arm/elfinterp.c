/* vi: set sw=4 ts=4: */
/* ARM ELF shared library loader suppport
 *
 * Copyright (C) 2001-2004 Erik Andersen
 *
 * All rights reserved.
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

/* Program to load an ELF binary on a linux system, and run it.
   References to symbols in sharable libraries can be resolved by either
   an ELF sharable library or a linux style of shared library. */

/* Disclaimer:  I have never seen any AT&T source code for SVr4, nor have
   I ever taken any courses on internals.  This program was developed using
   information available through the book "UNIX SYSTEM V RELEASE 4,
   Programmers guide: Ansi C and Programming Support Tools", which did
   a more than adequate job of explaining everything required to get this
   working. */

#include <ldsodefs.h>

#if 0
static unsigned long
fix_bad_pc24 (unsigned long *const reloc_addr, unsigned long value)
{
  static void *fix_page;
  static unsigned int fix_offset;
  unsigned int *fix_address;
  if (! fix_page)
    {
      fix_page = _dl_mmap (NULL,  PAGE_SIZE   , PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      fix_offset = 0;
    }

  fix_address = (unsigned int *)(fix_page + fix_offset);
  fix_address[0] = 0xe51ff004;  /* ldr pc, [pc, #-4] */
  fix_address[1] = value;

  fix_offset += 8;
  if (fix_offset >= PAGE_SIZE)
    fix_page = NULL;

  return (unsigned long)fix_address;
}
#endif

static int
_dl_do_reloc(struct elf_resolve *tpnt, struct dyn_elf *scope,
	     ELF_RELOC *rpnt, const ElfW(Sym) *const symtab, const char *strtab)
{
	ElfW(Addr) *reloc_addr = (ElfW(Addr) *)(tpnt->loadaddr + rpnt->r_offset);
	const unsigned int reloc_type = ELF_R_TYPE(rpnt->r_info);
	const int symtab_index = ELF_R_SYM(rpnt->r_info);
	ElfW(Addr) symbol_addr = 0;
	struct elf_resolve *tls_tpnt = NULL;
	struct symbol_ref sym_ref;
	sym_ref.sym = &symtab[symtab_index];
	sym_ref.tpnt = NULL;
	const char *symname = strtab + sym_ref.sym->st_name;
#if defined (__SUPPORT_LD_DEBUG__)
	ElfW(Addr) old_val;
#endif

	if (symtab_index) {
		symbol_addr = (ElfW(Addr))_dl_find_hash(symname, scope, tpnt,
							elf_machine_type_class(reloc_type), &sym_ref);

		/*
		 * We want to allow undefined references to weak symbols - this
		 * might have been intentional.  We should not be linking local
		 * symbols here, so all bases should be covered.
		 */
		if (unlikely(!symbol_addr
#if defined (__UCLIBC_HAS_TLS__)
			&& (ELF_ST_TYPE(sym_ref.sym->st_info) != STT_TLS)
#endif
			&& (ELF_ST_BIND(sym_ref.sym->st_info) != STB_WEAK))) {
#if defined (__SUPPORT_LD_DEBUG__)
			_dl_dprintf(2, "%s: can't resolve symbol '%s' in lib '%s'\n",
				    _dl_progname, symname, tpnt->libname);
#else
			_dl_dprintf(2, "%s: can't resolve symbol '%s'\n",
				    _dl_progname, symname);
#endif

			/* Let the caller handle the error: it may be non fatal if called from dlopen */
			return 1;
		}
		tls_tpnt = sym_ref.tpnt;
	} else {
		/*
		 * Relocs against STN_UNDEF are usually treated as using a
		 * symbol value of zero, and using the module containing the
		 * reloc itself.
		 */
		symbol_addr = sym_ref.sym->st_value;
		tls_tpnt = tpnt;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	old_val = *reloc_addr;
#endif

	switch (reloc_type) {
		case R_ARM_NONE:
			break;
		case R_ARM_GLOB_DAT:
		case R_ARM_JUMP_SLOT:
			*reloc_addr = symbol_addr;
			break;
#if 1 /* handled by elf_machine_relative */
		case R_ARM_RELATIVE:
			*reloc_addr += tpnt->loadaddr;
			break;
#endif
		case R_ARM_ABS32:
#if defined (__UCLIBC_HAS_TLS__)
		case R_ARM_TLS_DTPOFF32:
#endif
			*reloc_addr += symbol_addr;
			break;
		case R_ARM_COPY:
			if (symbol_addr) {
#if defined (__SUPPORT_LD_DEBUG__)
				if (_dl_debug_move)
					_dl_dprintf(_dl_debug_file,
						    "\n%s move %d bytes from %x to %p",
						    symname, sym_ref.sym->st_size,
						    symbol_addr, reloc_addr);
#endif
				_dl_memcpy((char *)reloc_addr,
					   (char *)symbol_addr,
					   sym_ref.sym->st_size);
			}
			break;
#if defined (__UCLIBC_HAS_TLS__)
		case R_ARM_TLS_DTPMOD32:
			*reloc_addr = tls_tpnt->l_tls_modid;
			break;
		case R_ARM_TLS_TPOFF32:
			CHECK_STATIC_TLS((struct link_map *)tls_tpnt);
			*reloc_addr += symbol_addr + tls_tpnt->l_tls_offset;
			break;
#endif
		case R_ARM_PC24:
#if 0
				{
					unsigned long addend;
					long newvalue, topbits;

					addend = *reloc_addr & 0x00ffffff;
					if (addend & 0x00800000) addend |= 0xff000000;

					newvalue = symbol_addr - (unsigned long)reloc_addr + (addend << 2);
					topbits = newvalue & 0xfe000000;
					if (topbits != 0xfe000000 && topbits != 0x00000000)
					{
						newvalue = fix_bad_pc24(reloc_addr, symbol_addr)
							- (unsigned long)reloc_addr + (addend << 2);
						topbits = newvalue & 0xfe000000;
						if (unlikely(topbits != 0xfe000000 && topbits != 0x00000000))
						{
							_dl_dprintf(2,"symbol '%s': R_ARM_PC24 relocation out of range.",
								sym_ref.sym->st_name);
							_dl_exit(1);
						}
					}
					newvalue >>= 2;
					symbol_addr = (*reloc_addr & 0xff000000) | (newvalue & 0x00ffffff);
					*reloc_addr = symbol_addr;
					break;
				}
#else
			_dl_dprintf(2,"R_ARM_PC24: Compile shared libraries with -fPIC!\n");
			_dl_exit(1);
#endif
		default:
			return -1;
	}

#if defined (__SUPPORT_LD_DEBUG__)
	if (_dl_debug_reloc && _dl_debug_detail)
		_dl_dprintf(_dl_debug_file, "\n\tpatched: %x ==> %x @ %p\n",
			    old_val, *reloc_addr, reloc_addr);
#endif

	return 0;
}
