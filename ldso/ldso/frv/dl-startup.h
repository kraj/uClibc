     /* Copyright (C) 2003 Red Hat, Inc.
	Contributed by Alexandre Oliva <aoliva@redhat.com>

This file is part of uClibc.

uClibc is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

uClibc is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with uClibc; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
USA.  */
	
/* Any assembly language/system dependent hacks needed to setup
 * boot1.c so it will work as expected and cope with whatever platform
 * specific wierdness is needed for this architecture.

 * We override the default _dl_boot function, and replace it with a
 * bit of asm.  Then call the real _dl_boot function, which is now
 * named _dl_boot2.  */

/* At program start-up, gr16 contains a pointer to a
   elf32_fdpic_loadmap that describes how the executable was loaded
   into memory.  gr17 contains a pointer to the interpreter (our!)
   loadmap, if there is an interpreter, or 0 if we're being run as an
   executable.  gr18 holds a pointer to the interpreter's dynamic
   section, if there is an interpreter, or to the executable's dynamic
   section, otherwise.  If the executable is not dynamic, gr18 is 0.

   We rely on the fact that the linker adds a pointer to the
   _GLOBAL_OFFSET_TABLE_ as the last ROFIXUP entry, and that
   __self_reloc returns the relocated pointer to us, so that we can
   use this value to initialize the PIC register.  */

asm("" \
"	.text\n"			\
"	.global	_dl_boot\n"		\
"	.type	_dl_boot,@function\n"	\
"_dl_boot:\n"				\
"	call	.Lcall\n"		\
".Lcall:\n"				\
"	movsg	lr, gr4\n"		\
"	sethi.p	#gprelhi(.Lcall), gr5\n"\
"	setlo	#gprello(.Lcall), gr5\n"\
"	mov.p	gr17, gr8\n"		\
"	cmp	gr17, gr0, icc0\n"	\
"	sub.p	gr4, gr5, gr4\n"	\
"	ckeq	icc0, cc4\n"		\
"	cmov.p	gr16, gr8, cc4, 1\n"	\
"	sethi	#gprelhi(__ROFIXUP_LIST__), gr9\n"	\
"	sethi.p	#gprelhi(__ROFIXUP_END__), gr10\n"	\
"	setlo	#gprello(__ROFIXUP_LIST__), gr9\n"	\
"	setlo.p	#gprello(__ROFIXUP_END__), gr10\n"	\
"	add	gr9, gr4, gr9\n"	\
"	add.p	gr10, gr4, gr10\n"	\
"	call	__self_reloc\n"		\
"	mov.p	gr8, gr15\n"		\
"	mov	gr16, gr9\n"		\
"	mov.p	gr17, gr10\n"		\
"	mov	gr18, gr11\n"		\
"	addi.p	sp, #4, gr13\n"		\
"	addi	sp, #-8, sp\n"		\
"	mov.p	sp, gr12\n"		\
"	call	_dl_boot2\n"		\
"	ldd.p	@(sp, gr0), gr14\n"	\
"	addi	sp, #8, sp\n"		\
"	movgs	gr0, lr\n"		\
"	jmpl	@(gr14, gr0)\n"		\
"	.size	_dl_boot,.-_dl_boot\n"	\
);

#define _dl_boot _dl_boot2
#define DL_BOOT(X)   \
static void  __attribute__ ((used)) \
_dl_boot (void *dl_boot_got_pointer, \
	  struct elf32_fdpic_loadmap *dl_boot_progmap, \
	  struct elf32_fdpic_loadmap *dl_boot_ldsomap, \
	  Elf32_Dyn *dl_boot_ldso_dyn_pointer, \
	  struct funcdesc_value *dl_main_funcdesc, \
	  X)

struct elf32_fdpic_loadmap;
