	.section .init
.globl _init
	.type	_init, @function
_init:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	call	__get_pc_thunk_bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx




	.section .fini
.globl _fini
	.type	_fini, @function
_fini:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	call	__get_pc_thunk_bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx



	.section	.gnu.linkonce.t.__get_pc_thunk_bx,"ax",@progbits
.globl __get_pc_thunk_bx
	.hidden	__get_pc_thunk_bx
	.type	__get_pc_thunk_bx, @function
__get_pc_thunk_bx:
	movl	(%esp), %ebx
	ret
