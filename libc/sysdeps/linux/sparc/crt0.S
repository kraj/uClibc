/*
 * xrt0.s for ERC32. 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* code taken from leonccs 1.0 leon/src/libio/crt0.S 
	I don't know if this is available anymore, now that LECCS
	is out.  And I'm not sure if this source is in the LECCS distro  :(
*/

	.text
! Original : 
!	.global __start, _main
! uC-libc version : 
	.global _start
	.global __uClibc_main

! Start the real-time clock with a tick of 14 clocks
!

_start:

	save	%sp, -64, %sp

        /* clear the bss */
 
        sethi %hi(edata),%g2
        or    %g2,%lo(edata),%g2  ! g2 = start of bss
        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
zerobss:
        std    %g0,[%g2]
        add    %g2,8,%g2
        cmp    %g2,%g3
        bleu,a zerobss
        nop

        /* move data segment to proper location */
 
relocd:
        set (_endtext),%g2 		! g2 = start of data in aout file
        set (_environ),%g4		! g4 = start of where data should go
        set (_edata),%g3 		! g3 = end of where data should go
	subcc	%g3, %g4, %g5		! g5 = length of data

	subcc	%g4, %g2, %g0		! need to relocate data ?
	ble	initok
	ld	[%g4], %g6
!	subcc	%g6, 1, %g0
!	be	initok
mvdata:
	subcc	%g5, 8, %g5
	ldd	[%g2 + %g5], %g6
	bg	mvdata
        std    	%g6, [%g4 + %g5]

initok:

!	call    _main
	call	__uClibc_main
        nop
! Should not return from uClibc main()
!	ret
!	nop

        .seg    "data"
        .global .bdata
.bdata:
        .align  8
        .global _environ                ! first symbol in sdata
_environ:
        .word   1



