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
#NO_APP
	ret
.Lfe2:
	.size	 _init,.Lfe2-_init
#APP
	
	.section .fini
#NO_APP
	.align 1
.globl _fini
	.type	 _fini,@function
#NO_APP
	ret
.Lfe3:
	.size	 _fini,.Lfe3-_fini
#APP
	
	.ident	"GCC: (GNU) 2.95.2 19991024 (release) (Linux/VAX CVS)"
