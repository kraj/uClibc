/* glibc's sysdeps/x86_64/elf/initfini.c used for reference [EPILOG] */

.file	"initfini.c"


.section .init
.globl _init
.type	_init, @function
	addq	$8, %rsp
	ret
.size	_init, .-_init


.section .fini
.globl _fini
.type	_fini, @function
	addq	$8, %rsp
	ret
.size	_fini, .-_fini
