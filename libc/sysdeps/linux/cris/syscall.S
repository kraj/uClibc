#include <sys/syscall.h>
	
        .globl __syscall_error
__syscall_error:
                        
        neg.d $r10,$r10

        push $r10
        push $srp
        jsr __errno_location
        pop $srp
        pop $r11
        move.d $r11,[$r10]

        moveq -1, $r10
        ret
        nop

