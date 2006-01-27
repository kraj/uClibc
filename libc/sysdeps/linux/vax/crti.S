	.file	"initfini.c"
	.version	"01.01"
gcc2_compiled.:
__gnu_compiled_c:
#APP
	
	.section .init
#NO_APP
	.align 1
.globl _init
	.type	 _init,@function
_init:
	.word 0x0
#APP
	
	.align 1
	
	
	.section .fini
#NO_APP
	.align 1
.globl _fini
	.type	 _fini,@function
_fini:
	.word 0x0
#APP
	.align 1
	
	
	.ident	"GCC: (GNU) 2.95.2 19991024 (release) (Linux/VAX CVS)"
