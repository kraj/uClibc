	.file	"errno.c"
gcc2_compiled.:
__gnu_compiled_c:
.globl errno
.data
	.even
errno:
	.long 0
.text
	.even
	.def	__errno_location;	.val	__errno_location;	.scl	2;	.type	0144;	.endef
.globl __errno_location
__errno_location:
	.def	.bf;	.val	.;	.scl	101;	.line	7;	.endef
	link.w %a6,#0
	.ln	2
	lea errno(%a5),%a0
	move.l %a0,%d1
	move.l %d1,%d0
	bra .L1
	.ln	3
.L1:
	.def	.ef;	.val	.;	.scl	101;	.line	3;	.endef
	unlk %a6
	rts
	.def	__errno_location;	.val	.;	.scl	-1;	.endef
	.def	errno;	.val	errno;	.scl	2;	.type	04;	.endef
