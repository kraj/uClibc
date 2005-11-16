	.file	1 "pt-initfini.c"
	.section .mdebug.abi32
	.previous
	.abicalls
#APP
	
#include "defs.h"
	
/*@HEADER_ENDS*/
	
/*@TESTS_BEGIN*/
#NO_APP
	.text
	.align	2
	.globl	dummy
	.type	dummy, @function
dummy:
	.set	noreorder
	.set	nomacro
	
	beq	$4,$0,$L6
	move	$25,$4

	jr	$25
	nop

$L6:
	j	$31
	nop

	.set	macro
	.set	reorder
#APP
	
/*@TESTS_END*/
	
/*@_init_PROLOG_BEGINS*/
#NO_APP
	.align	2
	.type	call_initialize_minimal, @function
call_initialize_minimal:
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	lw	$25,%call16(__pthread_initialize_minimal_internal)($28)
	nop
	jr	$25
	nop

	.set	macro
	.set	reorder
#APP
	.section .init
#NO_APP
	.align	2
	.globl	_init
	.type	_init, @function
_init:
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,24($sp)
	.cprestore	16
	lw	$25,%got(call_initialize_minimal)($28)
	#nop
	addiu	$25,$25,%lo(call_initialize_minimal)
	jalr	$25
	lw	$28,16($sp)
#APP
	ALIGN
	END_INIT
	
/*@_init_PROLOG_ENDS*/
	
/*@_init_EPILOG_BEGINS*/
	.section .init
#NO_APP
	lw	$31,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

#APP
	END_INIT
	
/*@_init_EPILOG_ENDS*/
	
/*@_fini_PROLOG_BEGINS*/
	.section .fini
#NO_APP
	.align	2
	.globl	_fini
	.type	_fini, @function
_fini:
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,24($sp)
	.cprestore	16
#APP
	ALIGN
	END_FINI
	
/*@_fini_PROLOG_ENDS*/
#NO_APP
	lw	$25,%call16(i_am_not_a_leaf)($28)
	#nop
	jalr	$25
	lw	$28,16($sp)
#APP
	
/*@_fini_EPILOG_BEGINS*/
	.section .fini
#NO_APP
	lw	$31,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

#APP
	END_FINI
	
/*@_fini_EPILOG_ENDS*/
	
/*@TRAILER_BEGINS*/
	.ident	"GCC: (GNU) 4.1.0 20050604 (experimental)"
