/* vi: set sw=4 ts=4: */
/*
 * Architecture specific code used by dl-startup.c
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 */


/* Overrive the default _dl_boot function, and replace it with a bit of asm.
 * Then call the real _dl_boot function, which is now named _dl_boot2. */
asm(""						\
"	.text\n"				\
"	.globl	_dl_boot\n"		\
"_dl_boot:\n"				\
"	mov	r7, sp\n"			\
"	@ldr	r0, [sp], #4\n"	\
"	mov	r0, sp\n"			\
"	bl	_dl_boot2\n"		\
"	mov	r6, r0\n"			\
"	mov	r0, r7\n"			\
"	mov	pc, r6\n"			\
);

#define DL_BOOT(X)   static __attribute_used__ void* _dl_boot2 (X)


/* Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.  */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long*)   ARGS)

/* Handle relocation of the symbols in the dynamic loader. */
static inline
void PERFORM_BOOTSTRAP_RELOC(ELF_RELOC *rpnt, unsigned long *reloc_addr,
	unsigned long symbol_addr, unsigned long load_addr, Elf32_Sym *symtab)
{
	switch (ELF32_R_TYPE(rpnt->r_info)) {
		case R_ARM_NONE:
			break;
		case R_ARM_ABS32:
			*reloc_addr += symbol_addr;
			break;
		case R_ARM_PC24:
			{
				unsigned long addend;
				long newvalue, topbits;

				addend = *reloc_addr & 0x00ffffff;
				if (addend & 0x00800000) addend |= 0xff000000;

				newvalue = symbol_addr - (unsigned long)reloc_addr + (addend << 2);
				topbits = newvalue & 0xfe000000;
				if (topbits != 0xfe000000 && topbits != 0x00000000)
				{
#if 0
					// Don't bother with this during ldso initilization...
					newvalue = fix_bad_pc24(reloc_addr, symbol_addr)
						- (unsigned long)reloc_addr + (addend << 2);
					topbits = newvalue & 0xfe000000;
					if (unlikely(topbits != 0xfe000000 && topbits != 0x00000000))
					{
						SEND_STDERR("R_ARM_PC24 relocation out of range\n");
						_dl_exit(1);
					}
#else
					SEND_STDERR("R_ARM_PC24 relocation out of range\n");
					_dl_exit(1);
#endif
				}
				newvalue >>= 2;
				symbol_addr = (*reloc_addr & 0xff000000) | (newvalue & 0x00ffffff);
				*reloc_addr = symbol_addr;
				break;
			}
		case R_ARM_GLOB_DAT:
		case R_ARM_JUMP_SLOT:
			*reloc_addr = symbol_addr;
			break;
		case R_ARM_RELATIVE:
			*reloc_addr += load_addr;
			break;
		case R_ARM_COPY:
			break;
		default:
			SEND_STDERR("Unsupported relocation type\n");
			_dl_exit(1);
	}
}


/* Transfer control to the user's application, once the dynamic loader is
 * done.  This routine has to exit the current function, then call the
 * _dl_elf_main function.  */
#define START()   return _dl_elf_main;


