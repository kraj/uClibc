/* Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>


/* The difference here is that the sigaction structure used in the
   kernel is not the same as we use in the libc.  Therefore we must
   translate it here.  */
#define HAVE_SA_RESTORER


#if defined(__alpha__)
#undef HAVE_SA_RESTORER
/* This is the sigaction struction from the Linux 2.1.20 kernel.  */
struct old_kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned long sa_mask;
    unsigned int sa_flags;
};
/* This is the sigaction structure from the Linux 2.1.68 kernel.  */
struct kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned int sa_flags;
    sigset_t sa_mask;
};
#elif defined(__hppa__)
/* We do not support SA_RESTORER on hppa. */
#undef HAVE_SA_RESTORER
/* This is the sigaction struction from the Linux 2.1.20 kernel.  */
/* Blah.  This is bogus.  We don't ever use it. */
struct old_kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned long sa_mask;
    unsigned long sa_flags;
};
/* This is the sigaction structure from the Linux 2.1.68 kernel.  */
struct kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned long sa_flags;
    sigset_t sa_mask;
};
#elif defined(__mips__)
/* This is the sigaction structure from the Linux 2.1.24 kernel.  */
#include <sgidefs.h>
struct old_kernel_sigaction {
    unsigned int    sa_flags;
    __sighandler_t  k_sa_handler;
    unsigned long   sa_mask;
    unsigned int    __pad0[3]; /* reserved, keep size constant */

    /* Abi says here follows reserved int[2] */
    void            (*sa_restorer)(void);
#if (_MIPS_ISA == _MIPS_ISA_MIPS1) || (_MIPS_ISA == _MIPS_ISA_MIPS2)
    /* For 32 bit code we have to pad struct sigaction to get 
     * constant size for the ABI */
    int             pad1[1]; /* reserved */
#endif
};

#define _KERNEL_NSIG           128
#define _KERNEL_NSIG_BPW       32
#define _KERNEL_NSIG_WORDS     (_KERNEL_NSIG / _KERNEL_NSIG_BPW)

typedef struct {
    unsigned long sig[_KERNEL_NSIG_WORDS];
} kernel_sigset_t;

/* This is the sigaction structure from the Linux 2.1.68 kernel.  */
struct kernel_sigaction {
    unsigned int    sa_flags;
    __sighandler_t  k_sa_handler;
    kernel_sigset_t sa_mask;
    void            (*sa_restorer)(void);
    int             s_resv[1]; /* reserved */
};
#else
/* This is the sigaction structure from the Linux 2.1.20 kernel.  */
struct old_kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned long sa_mask;
    unsigned long sa_flags;
    void (*sa_restorer) (void);
};      
        
/* This is the sigaction structure from the Linux 2.1.68 kernel.  */
struct kernel_sigaction {
    __sighandler_t k_sa_handler;
    unsigned long sa_flags;
    void (*sa_restorer) (void);
    sigset_t sa_mask;
};
#endif



#if defined __NR_rt_sigaction

extern int __rt_sigaction (int, const struct kernel_sigaction *__unbounded,
				   struct kernel_sigaction *__unbounded, size_t);

/* If ACT is not NULL, change the action for SIG to *ACT.
   If OACT is not NULL, put the old action for SIG in *OACT.  */
int __libc_sigaction (int sig, const struct sigaction *act, struct sigaction *oact)
{
    int result;
    struct kernel_sigaction kact, koact;

    if (act) {
	kact.k_sa_handler = act->sa_handler;
	memcpy (&kact.sa_mask, &act->sa_mask, sizeof (kact.sa_mask));
	kact.sa_flags = act->sa_flags;
# ifdef HAVE_SA_RESTORER
	kact.sa_restorer = act->sa_restorer;
# endif
    }

    /* XXX The size argument hopefully will have to be changed to the
       real size of the user-level sigset_t.  */
    result = __rt_sigaction(sig, act ? __ptrvalue (&kact) : NULL,
	    oact ? __ptrvalue (&koact) : NULL, _NSIG / 8);

    if (oact && result >= 0) {
	oact->sa_handler = koact.k_sa_handler;
	memcpy (&oact->sa_mask, &koact.sa_mask, sizeof (oact->sa_mask));
	oact->sa_flags = koact.sa_flags;
# ifdef HAVE_SA_RESTORER
	oact->sa_restorer = koact.sa_restorer;
# endif
    }
    return result;
}
weak_alias(__libc_sigaction, sigaction)




#else

extern int __sigaction (int, const struct old_kernel_sigaction *__unbounded,
				struct old_kernel_sigaction *__unbounded);

/* If ACT is not NULL, change the action for SIG to *ACT.
   If OACT is not NULL, put the old action for SIG in *OACT.  */
int __libc_sigaction (int sig, const struct sigaction *act, struct sigaction *oact)
{
    struct old_kernel_sigaction k_sigact, k_osigact;
    int result;

    if (act) {
	k_sigact.k_sa_handler = act->sa_handler;
	k_sigact.sa_mask = act->sa_mask.__val[0];
	k_sigact.sa_flags = act->sa_flags;
# ifdef HAVE_SA_RESTORER
	k_sigact.sa_restorer = act->sa_restorer;
# endif
    }
    result = __sigaction(sig, act ? __ptrvalue (&k_sigact) : NULL,
	    oact ? __ptrvalue (&k_osigact) : NULL);

    if (oact && result >= 0) {
	oact->sa_handler = k_osigact.k_sa_handler;
	oact->sa_mask.__val[0] = k_osigact.sa_mask;
	oact->sa_flags = k_osigact.sa_flags;
# ifdef HAVE_SA_RESTORER
	oact->sa_restorer = k_osigact.sa_restorer;
# endif
    }
    return result;
}

weak_alias(__libc_sigaction, sigaction)

#endif
