# 1 "crt0.S"
 


















	.global	_start
	.global	__main
	.global _end
 

	.bss
	.global environ
environ:
	.long 0

	.text
_start:			 
	nop
	nop

	movea.l %d5, %a5

	lea	__bss_start(%a5), %a0
	lea	end(%a5), %a1
 
	 
	 










	move.l 8(%sp), %d5
	move.l %d5, environ(%a5)
	
	bsr main

	move.l %d0,%sp@-
	bsr	exit		 

# 69 "crt0.S"


	.global	_cleanup
_cleanup:
	rts	 


