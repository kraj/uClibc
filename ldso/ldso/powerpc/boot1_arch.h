/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.  */

/* Overrive the default _dl_boot function, and replace it with a bit of asm.
 * Then call the real _dl_boot function, which is now named _dl_boot2. */

asm("\
.text
.globl _dl_boot
_dl_boot:
	addi	3,1,4

	bl      _dl_boot2

	li	0,0
	lwz	0,42(0)
.previous\n\
");

#define _dl_boot _dl_boot2
#define DL_BOOT(X)   static void *  __attribute__ ((unused)) _dl_boot (X)

