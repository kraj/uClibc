/* glibc's sysdeps/x86_64/elf/initfini.c used for reference [PROLOG] */

.section .init
.globl _init
.type	_init, @function
_init:
	subq	$8, %rsp


.section .fini
.globl _fini
.type	_fini, @function
_fini:
	subq	$8, %rsp
