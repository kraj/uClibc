/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.  */

asm("\
	.text
	.globl	_dl_boot
_dl_boot:
        mov	r15, r4
	mov.l   .L_dl_boot2, r0
	bsrf    r0
	add	#4, r4
.jmp_loc:
	jmp	@r0
	 mov    #0, r4        /* call _start with arg == 0 */
.L_dl_boot2:\n\
	.long   _dl_boot2-.jmp_loc\n\
	.previous\n\
");

#define _dl_boot _dl_boot2
#define LD_BOOT(X)   static void *  __attribute__ ((unused)) _dl_boot (X)
