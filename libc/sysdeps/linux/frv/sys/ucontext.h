/* Copyright (C) 1999, 2000, 2001, 2003 Free Software Foundation, Inc.
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

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <signal.h>

/* We need the signal context definitions even if they are not used
   included in <signal.h>.  */
#include <bits/sigcontext.h>


typedef unsigned long greg_t;

/* Number of general registers.  */
#define NGREG	(10+2+64)

/* Container for all general registers.  */
typedef greg_t gregset_t[NGREG];

#ifdef __USE_GNU
/* Number of each register is the `gregset_t' array.  */
enum
{
  PSR = 0,
#define PSR	PSR
  ISR = 1,
#define ISR	ISR
  CCR = 2,
#define CCR	CCR
  CCCR = 3,
#define CCCR	CCCR
  LR = 4,
#define LR	LR
  LCR = 5,
#define LCR	LCR
  PC = 6,
#define PC	PC
  __STATUS = 7,
#define __STATUS	__STATUS
  SYSCALLNO = 8,
#define SYSCALLNO	SYSCALLNO
  ORIG_GR8 = 9,
#define ORIG_GR8	ORIG_GR8
  GNER0 = 10,
#define GNER0	GNER0
  GNER1 = 11,
#define GNER1	GNER1
  GR0 = 12,
#define GR0	GR0
  GR1 = 13,
#define GR1	GR1
  GR2 = 14,
#define GR2	GR2
  GR3 = 15,
#define GR3	GR3
  GR4 = 16,
#define GR4	GR4
  GR5 = 17,
#define GR5	GR5
  GR6 = 18,
#define GR6	GR6
  GR7 = 19,
#define GR7	GR7
  GR8 = 20,
#define GR8	GR8
  GR9 = 21,
#define GR9	GR9
  GR10 = 22,
#define GR10	GR10
  GR11 = 23,
#define GR11	GR11
  GR12 = 24,
#define GR12	GR12
  GR13 = 25,
#define GR13	GR13
  GR14 = 26,
#define GR14	GR14
  GR15 = 27,
#define GR15	GR15
  GR16 = 28,
#define GR16	GR16
  GR17 = 29,
#define GR17	GR17
  GR18 = 30,
#define GR18	GR18
  GR19 = 31,
#define GR19	GR19
  GR20 = 32,
#define GR20	GR20
  GR21 = 33,
#define GR21	GR21
  GR22 = 34,
#define GR22	GR22
  GR23 = 35,
#define GR23	GR23
  GR24 = 36,
#define GR24	GR24
  GR25 = 37,
#define GR25	GR25
  GR26 = 38,
#define GR26	GR26
  GR27 = 39,
#define GR27	GR27
  GR28 = 40,
#define GR28	GR28
  GR29 = 41,
#define GR29	GR29
  GR30 = 42,
#define GR30	GR30
  GR31 = 43,
#define GR31	GR31
  GR32 = 44,
#define GR32	GR32
  GR33 = 45,
#define GR33	GR33
  GR34 = 46,
#define GR34	GR34
  GR35 = 47,
#define GR35	GR35
  GR36 = 48,
#define GR36	GR36
  GR37 = 49,
#define GR37	GR37
  GR38 = 50,
#define GR38	GR38
  GR39 = 51,
#define GR39	GR39
  GR40 = 52,
#define GR40	GR40
  GR41 = 53,
#define GR41	GR41
  GR42 = 54,
#define GR42	GR42
  GR43 = 55,
#define GR43	GR43
  GR44 = 56,
#define GR44	GR44
  GR45 = 57,
#define GR45	GR45
  GR46 = 58,
#define GR46	GR46
  GR47 = 59,
#define GR47	GR47
  GR48 = 60,
#define GR48	GR48
  GR49 = 61,
#define GR49	GR49
  GR50 = 62,
#define GR50	GR50
  GR51 = 63,
#define GR51	GR51
  GR52 = 64,
#define GR52	GR52
  GR53 = 65,
#define GR53	GR53
  GR54 = 66,
#define GR54	GR54
  GR55 = 67,
#define GR55	GR55
  GR56 = 68,
#define GR56	GR56
  GR57 = 69,
#define GR57	GR57
  GR58 = 70,
#define GR58	GR58
  GR59 = 71,
#define GR59	GR59
  GR60 = 72,
#define GR60	GR60
  GR61 = 73,
#define GR61	GR61
  GR62 = 74,
#define GR62	GR62
  GR63 = 75,
#define GR63	GR63
};
#endif

typedef unsigned long freg_t;

/* Number of FPU registers.  */
#define NFPREG	(64+2+2+8+2+1)

#ifdef __USE_GNU
/* Number of each register is the `gregset_t' array.  */
enum
{
  FR0 = 0,
#define FR0	FR0
  FR1 = 1,
#define FR1	FR1
  FR2 = 2,
#define FR2	FR2
  FR3 = 3,
#define FR3	FR3
  FR4 = 4,
#define FR4	FR4
  FR5 = 5,
#define FR5	FR5
  FR6 = 6,
#define FR6	FR6
  FR7 = 7,
#define FR7	FR7
  FR8 = 8,
#define FR8	FR8
  FR9 = 9,
#define FR9	FR9
  FR10 = 10,
#define FR10	FR10
  FR11 = 11,
#define FR11	FR11
  FR12 = 12,
#define FR12	FR12
  FR13 = 13,
#define FR13	FR13
  FR14 = 14,
#define FR14	FR14
  FR15 = 15,
#define FR15	FR15
  FR16 = 16,
#define FR16	FR16
  FR17 = 17,
#define FR17	FR17
  FR18 = 18,
#define FR18	FR18
  FR19 = 19,
#define FR19	FR19
  FR20 = 20,
#define FR20	FR20
  FR21 = 21,
#define FR21	FR21
  FR22 = 22,
#define FR22	FR22
  FR23 = 23,
#define FR23	FR23
  FR24 = 24,
#define FR24	FR24
  FR25 = 25,
#define FR25	FR25
  FR26 = 26,
#define FR26	FR26
  FR27 = 27,
#define FR27	FR27
  FR28 = 28,
#define FR28	FR28
  FR29 = 29,
#define FR29	FR29
  FR30 = 30,
#define FR30	FR30
  FR31 = 31,
#define FR31	FR31
  FR32 = 32,
#define FR32	FR32
  FR33 = 33,
#define FR33	FR33
  FR34 = 34,
#define FR34	FR34
  FR35 = 35,
#define FR35	FR35
  FR36 = 36,
#define FR36	FR36
  FR37 = 37,
#define FR37	FR37
  FR38 = 38,
#define FR38	FR38
  FR39 = 39,
#define FR39	FR39
  FR40 = 40,
#define FR40	FR40
  FR41 = 41,
#define FR41	FR41
  FR42 = 42,
#define FR42	FR42
  FR43 = 43,
#define FR43	FR43
  FR44 = 44,
#define FR44	FR44
  FR45 = 45,
#define FR45	FR45
  FR46 = 46,
#define FR46	FR46
  FR47 = 47,
#define FR47	FR47
  FR48 = 48,
#define FR48	FR48
  FR49 = 49,
#define FR49	FR49
  FR50 = 50,
#define FR50	FR50
  FR51 = 51,
#define FR51	FR51
  FR52 = 52,
#define FR52	FR52
  FR53 = 53,
#define FR53	FR53
  FR54 = 54,
#define FR54	FR54
  FR55 = 55,
#define FR55	FR55
  FR56 = 56,
#define FR56	FR56
  FR57 = 57,
#define FR57	FR57
  FR58 = 58,
#define FR58	FR58
  FR59 = 59,
#define FR59	FR59
  FR60 = 60,
#define FR60	FR60
  FR61 = 61,
#define FR61	FR61
  FR62 = 62,
#define FR62	FR62
  FR63 = 63,
#define FR63	FR63
  FNER0 = 64,
#define FNER0	FNER0
  FNER1 = 65,
#define FNER1	FNER1
  MSR0 = 66,
#define MSR0	MSR0
  MSR1 = 67,
#define MSR1	MSR1
  ACC0 = 68,
#define ACC0	ACC0
  ACC1 = 69,
#define ACC1	ACC1
  ACC2 = 70,
#define ACC2	ACC2
  ACC3 = 71,
#define ACC3	ACC3
  ACC4 = 72,
#define ACC4	ACC4
  ACC5 = 73,
#define ACC5	ACC5
  ACC6 = 74,
#define ACC6	ACC6
  ACC7 = 75,
#define ACC7	ACC7
  ACCG0123 = 76,
#define ACCG0123	ACCG0123
  ACCG4567 = 77,
#define ACCG4567	ACCG4567
  FSR0 = 78,
#define FSR0	FSR0
};
#endif

/* Structure to describe FPU registers.  */
typedef freg_t fpregset_t[NFPREG];

/* Context to describe whole processor state.  */
typedef struct
  {
    gregset_t gregs;
    fpregset_t fpregs;
    void *extension;
    unsigned long sc_oldmask; 	/* old sigmask */
  } __attribute__((aligned(8))) mcontext_t;

#ifdef __USE_GNU
struct kernel_user_int_regs
{
  /* integer registers
   * - up to gr[31] mirror pt_regs in the kernel
   */
  unsigned long		psr;		/* Processor Status Register */
  unsigned long		isr;		/* Integer Status Register */
  unsigned long		ccr;		/* Condition Code Register */
  unsigned long		cccr;		/* Condition Code for Conditional Insns Register */
  unsigned long		lr;		/* Link Register */
  unsigned long		lcr;		/* Loop Count Register */
  unsigned long		pc;		/* Program Counter Register */
  unsigned long		__status;	/* exception status */
  unsigned long		syscallno;	/* syscall number or -1 */
  unsigned long		orig_gr8;	/* original syscall arg #1 */
  unsigned long		gner[2];

  union {
    unsigned long	tbr;
    unsigned long	gr[64];
  };
};

struct kernel_user_fpmedia_regs
{
  /* FP/Media registers */
  unsigned long	fr[64];
  unsigned long	fner[2];
  unsigned long	msr[2];
  unsigned long	acc[8];
  unsigned char	accg[8];
  unsigned long	fsr[1];
};

struct kernel_user_context
{
  struct kernel_user_int_regs i;
  struct kernel_user_fpmedia_regs f;

  /* we provide a context extension so that we can save the regs for CPUs that
   * implement many more of Fujitsu's lavish register spec
   */
  void			*extension;

  /* This is not part of the kernel's struct user_context, but
     rather of the enclosing struct sigcontext, but we add it
     here to parallel mcontext_t, just for completeness.  */
  unsigned long sc_oldmask; 	/* old sigmask */
} __attribute__((aligned(8)));

/* This union enables alias-safe casts from mcontext_t* to the union
   type, that can then be dereferenced as_aliases.  */
union kmcontext_t
{
  mcontext_t as_regsets;
  /* as_aliases is actually missing sc_oldmask, that is present in
     mcontext_t.  */
  struct kernel_user_context as_aliases;
};
#endif

/* Userlevel context.  */
typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
  } ucontext_t;

#endif /* sys/ucontext.h */
