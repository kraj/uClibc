/* Copyright (C) 2001, 2003 Free Software Foundation, Inc.
   Copyright (C) 1999, 2000 Axis Communications AB.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*#************************************************************************#*/
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: memset()                                                 */
/*#                                                                         */
/*# PARAMETERS:  void* dst;   Destination address.                          */
/*#              int     c;   Value of byte to write.                       */
/*#              int   len;   Number of bytes to write.                     */
/*#                                                                         */
/*# RETURNS:     dst.                                                       */
/*#                                                                         */
/*# DESCRIPTION: Sets the memory dst of length len bytes to c, as standard. */
/*#              Framework taken from memcpy.  This routine is              */
/*#              very sensitive to compiler changes in register allocation. */
/*#              Should really be rewritten to avoid this problem.          */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 990713    HP              Tired of watching this function (or           */
/*#                           really, the nonoptimized generic              */
/*#                           implementation) take up 90% of simulator      */
/*#                           output.  Measurements needed.                 */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/

/* No, there's no macro saying 12*4, since it is "hard" to get it into
   the asm in a good way.  Thus better to expose the problem everywhere.
   */

/* Assuming 1 cycle per dword written or read (ok, not really true), and
   one per instruction, then 43+3*(n/48-1) <= 24+24*(n/48-1)
   so n >= 45.7; n >= 0.9; we win on the first full 48-byte block to set. */

#define ZERO_BLOCK_SIZE (1*12*4)

void *memset(void *, int, unsigned long);

libc_hidden_proto(memset)
void *memset(void *pdst,
             int c,
             unsigned long plen)
{
  /* Ok.  Now we want the parameters put in special registers.
     Make sure the compiler is able to make something useful of this. */

  register char *return_dst __asm__ ("r10") = pdst;
  register long n __asm__ ("r12") = plen;
  register int lc __asm__ ("r11") = c;

  /* Most apps use memset sanely.  Only those memsetting about 3..4
     bytes or less get penalized compared to the generic implementation
     - and that's not really sane use. */

  /* Ugh.  This is fragile at best.  Check with newer GCC releases, if
     they compile cascaded "x |= x << 8" sanely! */
  __asm__("movu.b %0,$r13 \n\
	   lslq 8,$r13    \n\
	   move.b %0,$r13 \n\
	   move.d $r13,%0 \n\
	   lslq 16,$r13   \n\
	   or.d $r13,%0"
          : "=r" (lc) : "0" (lc) : "r13");

  {
    register char *dst __asm__ ("r13") = pdst;
 
  if (((unsigned long) pdst & 3) != 0
     /* Oops! n=0 must be a legal call, regardless of alignment. */
      && n >= 3)
  {
    if ((unsigned long)dst & 1)
    {
      *dst = (char) lc;
      n--;
      dst++;
    }

    if ((unsigned long)dst & 2)
    {
      *(short *)dst = lc;
      n -= 2;
      dst += 2;
    }
  }

  /* Now the fun part.  For the threshold value of this, check the equation
     above. */
  /* Decide which copying method to use. */
  if (n >= ZERO_BLOCK_SIZE)
  {
    /* For large copies we use 'movem' */

  /* It is not optimal to tell the compiler about clobbering any
     registers; that will move the saving/restoring of those registers
     to the function prologue/epilogue, and make non-movem sizes
     suboptimal.

      This method is not foolproof; it assumes that the "asm reg"
     declarations at the beginning of the function really are used
     here (beware: they may be moved to temporary registers).
      This way, we do not have to save/move the registers around into
     temporaries; we can safely use them straight away.  */
    __asm__ volatile ("								\n\
	.syntax no_register_prefix						\n\
										\n\
        ;; Check that the register asm declaration got right.			\n\
        ;; The GCC manual explicitly says there's no warranty for that (too).	\n\
	.ifnc %0-%1-%4,$r13-$r12-$r11						\n\
	.err									\n\
	.endif									\n\
										\n\
	;; Save the registers we'll clobber in the movem process		\n\
	;; on the stack.  Don't mention them to gcc, it will only be		\n\
	;; upset.								\n\
	subq 	11*4,sp								\n\
        movem   r10,[sp]							\n\
										\n\
        move.d  r11,r0								\n\
        move.d  r11,r1								\n\
        move.d  r11,r2								\n\
        move.d  r11,r3								\n\
        move.d  r11,r4								\n\
        move.d  r11,r5								\n\
        move.d  r11,r6								\n\
        move.d  r11,r7								\n\
        move.d  r11,r8								\n\
        move.d  r11,r9								\n\
        move.d  r11,r10								\n\
										\n\
        ;; Now we've got this:							\n\
	;; r13 - dst								\n\
	;; r12 - n								\n\
										\n\
        ;; Update n for the first loop						\n\
        subq    12*4,r12							\n\
0:										\n\
        subq   12*4,r12								\n\
        bge     0b								\n\
	movem	r11,[r13+]							\n\
										\n\
        addq   12*4,r12  ;; compensate for last loop underflowing n		\n\
										\n\
	;; Restore registers from stack						\n\
        movem [sp+],r10" 

     /* Outputs */ : "=r" (dst), "=r" (n)
     /* Inputs */ : "0" (dst), "1" (n), "r" (lc));
    
  }

    /* Either we directly starts copying, using dword copying
       in a loop, or we copy as much as possible with 'movem' 
       and then the last block (<44 bytes) is copied here.
       This will work since 'movem' will have updated src,dst,n. */

    while ( n >= 16 )
    {
      *((long*)dst)++ = lc;
      *((long*)dst)++ = lc;
      *((long*)dst)++ = lc;
      *((long*)dst)++ = lc;
      n -= 16;
    }

    /* A switch() is definitely the fastest although it takes a LOT of code.
     * Particularly if you inline code this.
     */
    switch (n)
    {
      case 0:
        break;
      case 1:
        *(char*)dst = (char) lc;
        break;
      case 2:
        *(short*)dst = (short) lc;
        break;
      case 3:
        *((short*)dst)++ = (short) lc;
        *(char*)dst = (char) lc;
        break;
      case 4:
        *((long*)dst)++ = lc;
        break;
      case 5:
        *((long*)dst)++ = lc;
        *(char*)dst = (char) lc;
        break;
      case 6:
        *((long*)dst)++ = lc;
        *(short*)dst = (short) lc;
        break;
      case 7:
        *((long*)dst)++ = lc;
        *((short*)dst)++ = (short) lc;
        *(char*)dst = (char) lc;
        break;
      case 8:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        break;
      case 9:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *(char*)dst = (char) lc;
        break;
      case 10:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *(short*)dst = (short) lc;
        break;
      case 11:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((short*)dst)++ = (short) lc;
        *(char*)dst = (char) lc;
        break;
      case 12:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        break;
      case 13:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *(char*)dst = (char) lc;
        break;
      case 14:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *(short*)dst = (short) lc;
        break;
      case 15:
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((long*)dst)++ = lc;
        *((short*)dst)++ = (short) lc;
        *(char*)dst = (char) lc;
        break;
    }
  }

  return return_dst; /* destination pointer. */
} /* memset() */
libc_hidden_def(memset)
