/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.  See arm/boot1_arch.h for an example of what
 * can be done.
 */

asm("
	.text
	.globl _dl_boot
_dl_boot:
	.set noreorder
	bltzal $0, 0f
	nop
0:	.cpload $31
	.set reorder
	# Store offset of DYNAMIC section in first entry of GOT
	la $4, _DYNAMIC
	sw $4, -0x7ff0($28)
	move $4, $29
	la $8, coff
	bltzal $8, coff
coff:	subu $8, $31, $8
	la $25, _dl_boot2
	addu $25, $8
	jalr $25
");

#define _dl_boot _dl_boot2
#define DL_BOOT(X)   static void __attribute__ ((unused)) _dl_boot (X)
