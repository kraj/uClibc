/*  Copyright (C) 2002     Manuel Novoa III
 *  My stdio library for linux and (soon) elks.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* This code needs a lot of clean up.  Some of that is on hold until uClibc
 * gets a better configuration system (on Erik's todo list).
 * The other cleanup will take place during the implementation/integration of
 * the wide char (un)formatted i/o functions which I'm currently working on.
 */

/*  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!
 *
 *  This code is currently under development.  Also, I plan to port
 *  it to elks which is a 16-bit environment with a fairly limited
 *  compiler.  Therefore, please refrain from modifying this code
 *  and, instead, pass any bug-fixes, etc. to me.  Thanks.  Manuel
 *
 *  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION! */


#define _ISOC99_SOURCE			/* for ULLONG primarily... */
#define _GNU_SOURCE
#define _STDIO_UTILITY			/* We're using _uintmaxtostr. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>

#define __PRINTF_INFO_NO_BITFIELD
#include <printf.h>

#ifdef __STDIO_THREADSAFE
#include <pthread.h>
#endif /* __STDIO_THREADSAFE */

/**********************************************************************/

/* These provide some control over printf's feature set */
#define __STDIO_PRINTF_FLOAT
#define __STDIO_PRINTF_M_SUPPORT


/**********************************************************************/

#if defined(__UCLIBC__) && !defined(__UCLIBC_HAS_FLOATS__)
#undef __STDIO_PRINTF_FLOAT
#endif

#ifdef __BCC__
#undef __STDIO_PRINTF_FLOAT
#endif

#ifndef __STDIO_PRINTF_FLOAT
#undef L__dtostr
#endif

/**********************************************************************/

#define __STDIO_GLIBC_CUSTOM_PRINTF

/* TODO -- move these to a configuration section? */
#define MAX_FIELD_WIDTH		4095
#define MAX_USER_SPEC 10
#define MAX_POS_ARGS 10

/* TODO - fix the defs below */
#define MAX_ARGS_PER_SPEC   (MAX_POS_ARGS-2)

#if MAX_ARGS_PER_SPEC + 2 > MAX_POS_ARGS
#define MAX_ARGS		MAX_ARGS_PER_SPEC + 2
#else
#define MAX_ARGS		MAX_POS_ARGS
#endif

/**********************************************************************/

#define __PA_FLAG_INTMASK \
	(__PA_FLAG_CHAR|PA_FLAG_SHORT|__PA_FLAG_INT|PA_FLAG_LONG|PA_FLAG_LONG_LONG)

extern printf_function *_custom_printf_handler[MAX_USER_SPEC];
extern printf_arginfo_function *_custom_printf_arginfo[MAX_USER_SPEC];
extern char *_custom_printf_spec;

/**********************************************************************/

#define SPEC_FLAGS		" +0-#'I"
enum {
	FLAG_SPACE		=	0x01,
	FLAG_PLUS		=	0x02,	/* must be 2 * FLAG_SPACE */
	FLAG_ZERO		=	0x04,
	FLAG_MINUS		=	0x08,	/* must be 2 * FLAG_ZERO */
	FLAG_HASH		=	0x10,
	FLAG_THOUSANDS	=	0x20,
	FLAG_I18N		=	0x40	/* only works for d, i, u */
};	  

/**********************************************************************/

/* float layout          01234567890123456789   TODO: B?*/
#define SPEC_CHARS		"npxXoudifFeEgGaACScs"
enum {
	CONV_n = 0,
	CONV_p,
	CONV_x, CONV_X,	CONV_o,	CONV_u,	CONV_d,	CONV_i,
	CONV_f, CONV_F, CONV_e, CONV_E, CONV_g, CONV_G, CONV_a, CONV_A,
	CONV_C, CONV_S, CONV_c, CONV_s,
#ifdef __STDIO_PRINTF_M_SUPPORT
	CONV_m,
#endif
	CONV_custom0				/* must be last */
};

/*                         p   x   X  o   u   d   i */
#define SPEC_BASE		{ 16, 16, 16, 8, 10, 10, 10 }

#define SPEC_RANGES		{ CONV_n, CONV_p, CONV_i, CONV_A, \
						  CONV_C, CONV_S, CONV_c, CONV_s, CONV_custom0 }

#define SPEC_OR_MASK		 { \
	/* n */			(PA_FLAG_PTR|PA_INT), \
	/* p */			PA_POINTER, \
	/* oxXudi */	PA_INT, \
	/* fFeEgGaA */	PA_DOUBLE, \
	/* C */			PA_WCHAR, \
	/* S */			PA_WSTRING, \
	/* c */			PA_CHAR, \
	/* s */			PA_STRING, \
}

#define SPEC_AND_MASK		{ \
	/* n */			(PA_FLAG_PTR|__PA_INTMASK), \
	/* p */			PA_POINTER, \
	/* oxXudi */	(__PA_INTMASK), \
	/* fFeEgGaA */	(PA_FLAG_LONG_DOUBLE|PA_DOUBLE), \
	/* C */			(PA_WCHAR), \
	/* S */			(PA_WSTRING), \
	/* c */			(PA_CHAR), \
	/* s */			(PA_STRING), \
}

/**********************************************************************/
/*
 * In order to ease translation to what arginfo and _print_info._flags expect,
 * we map:  0:int  1:char  2:longlong 4:long  8:short
 * and then _flags |= (((q << 7) + q) & 0x701) and argtype |= (_flags & 0x701)
 */

/* TODO -- Fix the table below to take into account stdint.h. */
#if PTRDIFF_MAX != INT_MAX
#error fix QUAL_CHARS ptrdiff_t entry 't'!
#endif
#if SIZE_MAX != UINT_MAX
#error fix QUAL_CHARS size_t entries 'z', 'Z'!
#endif
#ifndef LLONG_MAX
#error fix QUAL_CHARS for no long long!  Affects 'L', 'j', 'q', 'll'.
#else
#if LLONG_MAX != INTMAX_MAX
#error fix QUAL_CHARS intmax_t entry 'j'!
#endif
#endif

#define QUAL_CHARS		{ \
	/* j:(u)intmax_t z:(s)size_t  t:ptrdiff_t  \0:int */ \
	/* q:long_long  Z:(s)size_t */ \
	'h',   'l',  'L',  'j',  'z',  't',  'q', 'Z',  0, \
	 2,     4,    8,    8,    0,    0,    8,   0,   0, /* TODO -- fix!!! */\
     1,     8 \
}

/**********************************************************************/

#ifdef __STDIO_VA_ARG_PTR
#ifdef __BCC__
#define __va_arg_ptr(ap,type)		(((type *)(ap += sizeof(type))) - 1)
#endif

#if 1
#ifdef __GNUC__
/* TODO -- need other than for 386 as well! */

#ifndef __va_rounded_size
#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))
#endif
#define __va_arg_ptr(AP, TYPE)						\
 (AP = (va_list) ((char *) (AP) + __va_rounded_size (TYPE)),	\
  ((void *) ((char *) (AP) - __va_rounded_size (TYPE))))
#endif
#endif
#endif /* __STDIO_VA_ARG_PTR */

#ifdef __va_arg_ptr
#define GET_VA_ARG(AP,F,TYPE,ARGS)	(*(AP) = __va_arg_ptr(ARGS,TYPE))
#define GET_ARG_VALUE(AP,F,TYPE)	(*((TYPE *)(*(AP))))
#else
typedef union {
	wchar_t wc;
	unsigned int u;
	unsigned long ul;
#ifdef ULLONG_MAX
	unsigned long long ull;
#endif
#ifdef __STDIO_PRINTF_FLOAT
	double d;
	long double ld;
#endif /* __STDIO_PRINTF_FLOAT */
	void *p;
} argvalue_t;

#define GET_VA_ARG(AU,F,TYPE,ARGS)	(AU->F = va_arg(ARGS,TYPE))
#define GET_ARG_VALUE(AU,F,TYPE)	((TYPE)((AU)->F))
#endif

typedef struct {
	const char *fmtpos;			/* TODO: move below struct?? */
	struct printf_info info;
	int maxposarg;				/* > 0 if args are positional, 0 if not, -1 if unknown */
	int num_data_args;			/* TODO: use sentinal??? */
	unsigned int conv_num;
	unsigned char argnumber[4]; /* width | prec | 1st data | unused */
	int argtype[MAX_ARGS];
	va_list arg;
#ifdef __va_arg_ptr
	void *argptr[MAX_ARGS];
#else
	/* While this is wasteful of space in the case where pos args aren't
	 * enabled, it is also needed to support custom printf handlers. */
	argvalue_t argvalue[MAX_ARGS];
#endif
} ppfs_t;						/* parse printf format state */

/**********************************************************************/

/* TODO: fix printf to return 0 and set errno if format error.  Standard says
   only returns -1 if sets error indicator for the stream. */

/* TODO -- __isdigit() macro */
#define __isdigit(c) (((unsigned int)(c - '0')) < 10)

#ifdef __STDIO_PRINTF_FLOAT
extern size_t _dtostr(FILE * fp, long double x, struct printf_info *info);
#endif

#define _outnstr(stream, string, len)	_stdio_fwrite(s, len, stream)	/* TODO */

extern int _do_one_spec(FILE * __restrict stream, ppfs_t *ppfs, int *count);

extern int _ppfs_init(ppfs_t *ppfs, const char *fmt0); /* validates */
extern void _ppfs_prepargs(ppfs_t *ppfs, va_list arg); /* sets posargptrs */
extern void _ppfs_setargs(ppfs_t *ppfs); /* sets argptrs for current spec */
extern int _ppfs_parsespec(ppfs_t *ppfs); /* parses specifier */

/**********************************************************************/
#ifdef L_parse_printf_format

/* NOTE: This function differs from the glibc version in that parsing stops
 * upon encountering an invalid conversion specifier.  Since this is the way
 * my printf functions work, I think it makes sense to do it that way here.
 * Unfortunately, since glibc sets the return type as size_t, we have no way
 * of returning that the template is illegal, other than returning 0.
 */

size_t parse_printf_format(register const char *template,
						   size_t n, int *argtypes)
{
	ppfs_t ppfs;
	size_t i;
	size_t count = 0;

	if (_ppfs_init(&ppfs, template) >= 0) {
		if (ppfs.maxposarg > 0)  { /* Using positional args. */
			count = ppfs.maxposarg;
			if (n > count) {
				n = count;
			}
			for (i = 0 ; i < n ; i++) {
				*argtypes++ = ppfs.argtype[i];
			}
		} else {				/* Not using positional args. */
			while (*template) {
				if ((*template == '%') && (*++template != '%')) {
					ppfs.fmtpos = template;
					_ppfs_parsespec(&ppfs); /* Can't fail. */
					template = ppfs.fmtpos; /* Update to one past spec end. */
					if (ppfs.info.width == INT_MIN) {
						++count;
						if (n > 0) {
							*argtypes++ = PA_INT;
							--n;
						}
					}
					if (ppfs.info.prec == INT_MIN) {
						++count;
						if (n > 0) {
							*argtypes++ = PA_INT;
							--n;
						}
					}
					for (i = 0 ; i < ppfs.num_data_args ; i++) {
						if ((ppfs.argtype[i]) != __PA_NOARG) {
							++count;
							if (n > 0) {
								*argtypes++ = ppfs.argtype[i];
								--n;
							}
						}
					}
				} else {
					++template;
				}
			}
		}
	}

	return count;
}

#endif
/**********************************************************************/
#ifdef L_vfprintf

/* We only support ascii digits (or their USC equivalent codes) in
 * precision and width settings in *printf (wide) format strings.
 * In other words, we don't currently support glibc's 'I' flag.
 * We do accept it, but it is currently ignored. */

/* TODO -- Implement grouping flag... on hold (locale support). */

int vfprintf(FILE * __restrict stream, register const char * __restrict format,
			 va_list arg)
{
	ppfs_t ppfs;
	int count, r;
	register const char *s;

	__STDIO_THREADLOCK(stream);

	count = 0;
	s = format;

	if (_ppfs_init(&ppfs, format) < 0) { /* Bad format string. */
		_outnstr(stream, format, strlen(format));
		count = -1;
	} else {
		_ppfs_prepargs(&ppfs, arg);

		do {
			while (*format && (*format != '%')) {
				++format;
			}

			if (format-s) {		/* output any literal text in format string */
				if ( (r = _outnstr(stream, s, format-s)) < 0) {
					count = -1;
					break;
				}
				count += r;
			}

			if (!*format) {			/* we're done */
				break;
			}
		
			if (format[1] != '%') {	/* if we get here, *format == '%' */
				/* TODO: _do_one_spec needs to know what the output funcs are!!! */
				ppfs.fmtpos = ++format;
				/* TODO: check -- should only fail on stream error */
				if ( (r = _do_one_spec(stream, &ppfs, &count)) < 0) {
					count = -1;
					break;
				}
				s = format = ppfs.fmtpos;
			} else {			/* %% means literal %, so start new string */
				s = ++format;
				++format;
			}
		} while (1);
	}

	__STDIO_THREADUNLOCK(stream);

	return count;
}
#endif
/**********************************************************************/
#ifdef L__ppfs_init

int _ppfs_init(register ppfs_t *ppfs, const char *fmt0)
{
	int r;

	/* First, zero out everything... argnumber[], argtype[], argptr[] */
	memset(ppfs, 0, sizeof(ppfs_t)); /* TODO: nonportable???? */
	--ppfs->maxposarg;			/* set to -1 */
	/* now set all argtypes to no-arg */
	{
#if 1
		/* TODO - use memset here since already "paid for"? */
		register int *p = ppfs->argtype;
		
		r = MAX_ARGS;
		do {
			*p++ = __PA_NOARG;
		} while (--r);
#else
		/* TODO -- get rid of this?? */
		register char *p = (char *) ((MAX_ARGS-1) * sizeof(int));

		do {
			*((int *)(((char *)ppfs) + ((int)p) + offsetof(ppfs_t,argtype))) = __PA_NOARG;
			p -= sizeof(int);
		} while (p);
#endif
	}

	/*
	 * Run through the entire format string to validate it and initialize
	 * the positional arg numbers (if any).
	 */
	{
		register const char *fmt = fmt0;

		while (*fmt) {
			if ((*fmt == '%') && (*++fmt != '%')) {
				ppfs->fmtpos = fmt; /* back up to the '%' */
				if ((r = _ppfs_parsespec(ppfs)) < 0) {
					return -1;
				}
				fmt = ppfs->fmtpos;	/* update to one past end of spec */
			} else {
				++fmt;
			}
		}
		ppfs->fmtpos = fmt0;		/* rewind */
	}

	/* If we have positional args, make sure we know all the types. */
	{
		register int *p = ppfs->argtype;
		r = ppfs->maxposarg;
		while (--r >= 0) {
			if ( *p == __PA_NOARG ) { /* missing arg type!!! */
				return -1;
			}
			++p;
		}
	}

	return 0;
}
#endif
/**********************************************************************/
#ifdef L__ppfs_prepargs
void _ppfs_prepargs(register ppfs_t *ppfs, va_list arg)
{
	int i;

#ifdef __va_copy
	__va_copy(ppfs->arg, arg);
#else
	/* TODO -- maybe create a bits/vacopy.h for arch specific versions
	 * to ensure we get the right behavior?  Either that or fall back
	 * on the portable (but costly in size) method of using a va_list *.
	 * That means a pointer derefs in the va_arg() invocations... */
#warning __va_copy is not defined, using a simple copy instead...
	/* the glibc manual suggests that this will usually suffice when
        __va_copy doesn't exist.  */
	ppfs->arg = arg;
#endif

	if ((i = ppfs->maxposarg) > 0)  { /* init for positional args */
		ppfs->num_data_args = i;
		ppfs->info.width = ppfs->info.prec = ppfs->maxposarg = 0;
		_ppfs_setargs(ppfs);
		ppfs->maxposarg = i;
	}
}
#endif
/**********************************************************************/
#ifdef L__ppfs_setargs

void _ppfs_setargs(register ppfs_t *ppfs)
{
#ifdef __va_arg_ptr
	register void **p = ppfs->argptr;
#else
	register argvalue_t *p = ppfs->argvalue;
#endif
	int i;

	if (ppfs->maxposarg == 0) {	/* initing for or no pos args */
		if (ppfs->info.width == INT_MIN) {
			ppfs->info.width =
#ifdef __va_arg_ptr
				*(int *)
#endif
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
		} 
		if (ppfs->info.prec == INT_MIN) {
			ppfs->info.prec =
#ifdef __va_arg_ptr
				*(int *)
#endif
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
		}
		i = 0;
		while (i < ppfs->num_data_args) {
			switch(ppfs->argtype[i++]) {
				case (PA_INT|PA_FLAG_LONG_LONG):
#ifdef ULLONG_MAX
					GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
					break;
#endif
				case (PA_INT|PA_FLAG_LONG):
#if ULONG_MAX != UINT_MAX
					GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
					break;
#endif
				case PA_CHAR:	/* TODO - be careful */
 					/* ... users could use above and really want below!! */
				case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
				case (PA_INT|PA_FLAG_SHORT):
				case PA_INT:
					GET_VA_ARG(p,u,unsigned int,ppfs->arg);
					break;
				case PA_WCHAR:	/* TODO -- assume int? */
					/* we're assuming wchar_t is at least an int */
					GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
					break;
#ifdef __STDIO_PRINTF_FLOAT
					/* PA_FLOAT */
				case PA_DOUBLE:
					GET_VA_ARG(p,d,double,ppfs->arg);
					break;
				case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
					GET_VA_ARG(p,ld,long double,ppfs->arg);
					break;
#else  /* __STDIO_PRINTF_FLOAT */
				case PA_DOUBLE:
				case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
					assert(0);
					continue;
#endif /* __STDIO_PRINTF_FLOAT */
				default:
					/* TODO -- really need to ensure this can't happen */
					assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
				case PA_POINTER:
				case PA_STRING:
				case PA_WSTRING:
					GET_VA_ARG(p,p,void *,ppfs->arg);
					break;				
				case __PA_NOARG:
					continue;
			}
			++p;
		}
	} else {
		if (ppfs->info.width == INT_MIN) {
			ppfs->info.width
				= (int) GET_ARG_VALUE(p + ppfs->argnumber[0] - 1,u,unsigned int);
		} 
		if (ppfs->info.prec == INT_MIN) {
			ppfs->info.prec
				= (int) GET_ARG_VALUE(p + ppfs->argnumber[1] - 1,u,unsigned int);
		}
	}

	/* Now we know the width and precision. */
	if (ppfs->info.width < 0) {
		ppfs->info.width = -ppfs->info.width;
		PRINT_INFO_SET_FLAG(&(ppfs->info),left);
		PRINT_INFO_CLR_FLAG(&(ppfs->info),space);
		ppfs->info.pad = ' ';
	}
#if 0
	/* NOTE -- keep neg for now so float knows! */
	if (ppfs->info.prec < 0) {	/* spec says treat as omitted. */
		/* so use default prec... 1 for everything but floats and strings. */
		ppfs->info.prec = 1;
	}
#endif
}
#endif
/**********************************************************************/
#ifdef L__ppfs_parsespec

/* Notes: argtype differs from glibc for the following:
 *         mine              glibc
 *  lc     PA_WCHAR          PA_CHAR       the standard says %lc means %C
 *  ls     PA_WSTRING        PA_STRING     the standard says %ls means %S
 *  {*}n   {*}|PA_FLAG_PTR   PA_FLAG_PTR   size of n can be qualified
 */

/* TODO: store positions of positional args */

/* TODO -- WARNING -- assumes aligned on integer boundaries!!! */

/* TODO -- disable if not using positional args!!! */
#define _OVERLAPPING_DIFFERENT_ARGS

/* TODO -- rethink this -- perhaps we should set to largest type??? */

#ifdef _OVERLAPPING_DIFFERENT_ARGS 

#define PROMOTED_SIZE_OF(X)		((sizeof(X) + sizeof(int) - 1) / sizeof(X))

static const short int type_codes[] = {
	__PA_NOARG,					/* must be first entry */
	PA_POINTER,
	PA_STRING,
	PA_WSTRING,
	PA_CHAR,
	PA_INT|PA_FLAG_SHORT,
	PA_INT,
	PA_INT|PA_FLAG_LONG,
	PA_INT|PA_FLAG_LONG_LONG,
	PA_WCHAR,
#ifdef __STDIO_PRINTF_FLOAT
	/* PA_FLOAT, */
	PA_DOUBLE,
	PA_DOUBLE|PA_FLAG_LONG_DOUBLE,
#endif /* __STDIO_PRINTF_FLOAT */
};

static const unsigned char type_sizes[] = {
	/* unknown type consumes no arg */
	0,							/* must be first entry */
	PROMOTED_SIZE_OF(void *),
	PROMOTED_SIZE_OF(char *),
	PROMOTED_SIZE_OF(wchar_t *),
	PROMOTED_SIZE_OF(char),
	PROMOTED_SIZE_OF(short),
	PROMOTED_SIZE_OF(int),
	PROMOTED_SIZE_OF(long),
#ifdef ULLONG_MAX
	PROMOTED_SIZE_OF(long long),
#else
	PROMOTED_SIZE_OF(long),		/* TODO -- is this correct? (above too) */
#endif
	PROMOTED_SIZE_OF(wchar_t),
#ifdef __STDIO_PRINTF_FLOAT
	/* PROMOTED_SIZE_OF(float), */
	PROMOTED_SIZE_OF(double),
	PROMOTED_SIZE_OF(long double),
#endif /* __STDIO_PRINTF_FLOAT */
};

static int _promoted_size(int argtype)
{
	register const short int *p;

	/* note -- since any unrecognized type is treated as a pointer */
	p = type_codes + sizeof(type_codes)/sizeof(type_codes[0]);
	do {
		if (*--p == argtype) {
			break;
		}
	} while (p > type_codes);

	return type_sizes[(int)(p - type_codes)];
}

static int _is_equal_or_bigger_arg(int curtype, int newtype)
{
	/* Quick test */
	if (newtype == __PA_NOARG) {
		return 0;
	}
	if ((curtype == __PA_NOARG) || (curtype == newtype)) {
		return 1;
	}
	/* Ok... slot is already filled and types are different in name. */
	/* So, compare promoted sizes of curtype and newtype args. */
	return _promoted_size(curtype) <= _promoted_size(newtype);
}

#else

#define _is_equal_or_bigger_arg(C,N)	(((C) == __PA_NOARG) || ((C) == (N)))

#endif

/* TODO - do this differently? */
static char _bss_custom_printf_spec[MAX_USER_SPEC]; /* 0-init'd for us.  */

char *_custom_printf_spec = _bss_custom_printf_spec;
printf_arginfo_function *_custom_printf_arginfo[MAX_USER_SPEC];

extern int _ppfs_parsespec(ppfs_t *ppfs)
{
	register const char *fmt;
	register const char *p;
	int preci;
	int width;
	int flags;
	int dataargtype;
	int i;
	int dpoint;
	int maxposarg;
	int p_m_spec_chars;
	int n;
	int argtype[MAX_ARGS_PER_SPEC+2];
	int argnumber[3];			/* width, precision, 1st data arg */
	unsigned int conv_num;
	static const char spec_flags[] = SPEC_FLAGS;
	static const char spec_chars[] = SPEC_CHARS;/* TODO: b? */
	static const char spec_ranges[] = SPEC_RANGES;
	static const short spec_or_mask[] = SPEC_OR_MASK;
	static const short spec_and_mask[] = SPEC_AND_MASK;
	static const char qual_chars[] = QUAL_CHARS;

	/* WIDE note: we can test against '%' here since we don't allow */
	/* WIDE note: other mappings of '%' in the wide char set. */
	preci = -1;
	width = flags = dpoint = 0;
	argnumber[0] = 0;
	argnumber[1] = 0;
	argtype[0] = __PA_NOARG;
	argtype[1] = __PA_NOARG;
	maxposarg = ppfs->maxposarg;
	fmt = ppfs->fmtpos;

	assert(fmt[-1] == '%');
	assert(fmt[0] != '%');

	/* Process arg pos and/or flags and/or width and/or precision. */
 width_precision:
	p = fmt;
	if (*fmt == '*') {
		argtype[-dpoint] = PA_INT;
		++fmt;
	}
	i = 0;
	while (__isdigit(*fmt)) {
		if (i < MAX_FIELD_WIDTH) { /* Avoid overflow. */
			i = (i * 10) + (*fmt - '0');
		}
		++fmt;
	}
	if (p[-1] == '%') { /* Check for a position. */

		/* TODO: if val not in range, then error */

		if ((*fmt == '$') && (i > 0)) {/* Positional spec. */
			++fmt;
			if (maxposarg == 0) {
				return -1;
			}
			if ((argnumber[2] = i) > maxposarg) {
				maxposarg = i;
			}
			/* Now fall through to check flags. */
		} else {
			if (maxposarg > 0) {
				return -1;
			}
			maxposarg = 0;		/* Possible redundant store, but cuts size. */

			if ((fmt > p) && (*p != '0')) {
				goto PREC_WIDTH;
			}

			fmt = p;			/* Back up for possible '0's flag. */
			/* Now fall through to check flags. */
		}

	restart_flags:		/* Process flags. */
		i = 1;
		p = spec_flags;
	
		do {
			if (*fmt == *p++) {
				++fmt;
				flags |= i;
				goto restart_flags;
			}
			i += i;				/* Better than i <<= 1 for bcc */
		} while (*p);
		i = 0;

		/* If '+' then ignore ' ', and if '-' then ignore '0'. */
		/* Note: Need to ignore '0' when prec is an arg with val < 0, */
		/*       but that test needs to wait until the arg is retrieved. */
		flags &= ~((flags & (FLAG_PLUS|FLAG_MINUS)) >> 1);
		/* Note: Ignore '0' when prec is specified < 0 too (in printf). */

		if (fmt[-1] != '%') {	/* If we've done anything, loop for width. */
			goto width_precision;
		}
	}
 PREC_WIDTH:
	if (*p == '*') {			/* Prec or width takes an arg. */
		if (maxposarg) {
			if ((*fmt++ != '$') || (i <= 0)) {
				/* Using pos args and no $ or invalid arg number. */
				return -1;
			}
			argnumber[-dpoint] = i;
		} else if (++p != fmt) {
			 /* Not using pos args but digits followed *. */
			return -1;
		}
		i = INT_MIN;
	}

	if (!dpoint) {
		width = i;
		if (*fmt == '.') {
			++fmt;
			dpoint = -1;		/* To use as default precison. */
			goto width_precision;
		}
	} else {
		preci = i;
	}

	/* Process qualifier. */
	p = qual_chars;
	do {
		if (*fmt == *p) {
			++fmt;
			break;
		}
	} while (*++p);
	if ((p - qual_chars < 2) && (*fmt == *p)) {
		p += ((sizeof(qual_chars)-2) / 2);
		++fmt;
	}
	dataargtype = ((int)(p[(sizeof(qual_chars)-2) / 2])) << 8;

	/* Process conversion specifier. */
	if (!*fmt) {
		return -1;
	}

	p = spec_chars;

	do {
		if (*fmt == *p) {
			p_m_spec_chars = p - spec_chars;

			if ((p_m_spec_chars >= CONV_c)
				&& (dataargtype & PA_FLAG_LONG)) {
				p_m_spec_chars -= 2; /* lc -> C and ls -> S */
			}

			conv_num = p_m_spec_chars;
			p = spec_ranges-1;
			while (p_m_spec_chars > *++p) {}

			i = p - spec_ranges;
			argtype[2] = (dataargtype | spec_or_mask[i]) & spec_and_mask[i];
			p = spec_chars;
			break;
		}
	} while(*++p);

	ppfs->info.spec = *fmt;
	ppfs->info.prec = preci;
	ppfs->info.width = width;
	ppfs->info.pad = ((flags & FLAG_ZERO) ? '0' : ' ');
	ppfs->info._flags = (flags & ~FLAG_ZERO) | (dataargtype & __PA_INTMASK);
	ppfs->num_data_args = 1;

	if (!*p) {
#ifdef __STDIO_GLIBC_CUSTOM_PRINTF
		/* TODO -- gnu %m support build option. */
#ifdef __STDIO_PRINTF_M_SUPPORT
		if (*fmt == 'm') {
			conv_num = CONV_m;
			ppfs->num_data_args = 0;
			goto DONE;
		}
#endif

		/* Handle custom arg -- WARNING -- overwrites p!!! */
		conv_num = CONV_custom0;
		p = _custom_printf_spec;
		do {
			if (*p == *fmt) {
				if ((ppfs->num_data_args
					 = ((*_custom_printf_arginfo[(int)(p-_custom_printf_spec)])
						(&(ppfs->info), MAX_ARGS_PER_SPEC, argtype+2)))
					> MAX_ARGS_PER_SPEC) {
					break;		/* Error -- too many args! */
				}
				goto DONE;
			}
		} while (++p < (_custom_printf_spec + MAX_USER_SPEC));
#endif /* __STDIO_GLIBC_CUSTOM_PRINTF */
		/* Otherwise error. */
		return -1;
	}
		
#ifdef __STDIO_GLIBC_CUSTOM_PRINTF
 DONE:
#endif

	if (maxposarg > 0) {
		i = 0;
		do {
			/* Update maxposarg and check that NL_ARGMAX is not exceeded. */
			n = ((i <= 2)
				 ? (ppfs->argnumber[i] = argnumber[i])
				 : argnumber[2] + (i-2));
			if (n > maxposarg) {
				if ((maxposarg = n) > NL_ARGMAX) {
					return -1;
				}
			}
			--n;
			/* Record argtype with largest size (current, new). */
			if (_is_equal_or_bigger_arg(ppfs->argtype[n], argtype[i])) {
				ppfs->argtype[n] = argtype[i];
			}
		} while (++i < ppfs->num_data_args + 2);
	} else {
		ppfs->argnumber[2] = 1;
		memcpy(ppfs->argtype, argtype + 2, ppfs->num_data_args * sizeof(int));
	}

	ppfs->maxposarg = maxposarg;
	ppfs->fmtpos = ++fmt;
	ppfs->conv_num = conv_num;

 	return ppfs->num_data_args + 2;
}

#endif
/**********************************************************************/
#ifdef L_register_printf_function

int register_printf_function(int spec, printf_function handler,
							 printf_arginfo_function arginfo)
{
	register char *r;
	register char *p;

	if (spec && (arginfo != NULL)) { /* TODO -- check if spec is valid char */
		r = NULL;
		p = _custom_printf_spec + MAX_USER_SPEC;
		do {
			--p;
			if (!*p) {
				r = p;
			}
#ifdef __BCC__
			else				/* bcc generates less code with fall-through */
#endif
			if (*p == spec) {
				r = p;
				p = _custom_printf_spec;
			}
		} while (p > _custom_printf_spec);

		if (r) {
			if (handler) {
				*r = spec;
				_custom_printf_handler[(int)(r - p)] = handler;
				_custom_printf_arginfo[(int)(r - p)] = arginfo;
			} else {
				*r = 0;
			}
			return 0;
		}
		/* TODO -- if asked to unregister a non-existent spec, return what? */
	}
	return -1;
}
#endif
/**********************************************************************/
#ifdef L__do_one_spec

printf_function *_custom_printf_handler[MAX_USER_SPEC];

extern void _store_inttype(void *dest, int desttype, uintmax_t val);
extern uintmax_t _load_inttype(int desttype, const void *src, int uflag);
extern void _charpad(FILE * __restrict stream, char padchar, size_t numpad);
extern void output(FILE * __restrict stream, const char *s);

#define output(F,S)			fputs(S,F)

#undef putc
void _charpad(FILE * __restrict stream, char padchar, size_t numpad)
{
	/* TODO -- Use a buffer to cut down on function calls... */
	char pad[1];

	*pad = padchar;
	while (numpad) {
		_stdio_fwrite(pad, 1, stream);
		--numpad;
	}
}

/* TODO -- Dynamically allocate work space to accomodate stack-poor archs? */

int _do_one_spec(FILE * __restrict stream, register ppfs_t *ppfs, int *count)
{
	static const char spec_base[] = SPEC_BASE;
	static const char prefix[] = "+\0-\0 \0000x\0000X";
	/*                            0  2  4  6   9 11*/
	enum {
		PREFIX_PLUS = 0,
		PREFIX_MINUS = 2,
		PREFIX_SPACE = 4,
		PREFIX_LWR_X = 6,
		PREFIX_UPR_X = 9,
		PREFIX_NONE = 11
	};

#ifdef __va_arg_ptr
	const void * const *argptr;
#else
	const void * argptr[MAX_ARGS_PER_SPEC];
#endif
	int *argtype;
	size_t slen;
	int base;
	int numpad;
	int numfill = 0;			/* TODO: fix */
	int prefix_num = PREFIX_NONE;
	char padchar = ' ';
	char buf[64];

#ifdef NDEBUG
	_ppfs_parsespec(ppfs);
#else
	if (_ppfs_parsespec(ppfs) < 0) { /* TODO: just for debugging */
		abort();
	}
#endif
	_ppfs_setargs(ppfs);

	argtype = ppfs->argtype + ppfs->argnumber[2] - 1;
	/* Deal with the argptr vs argvalue issue. */
#ifdef __va_arg_ptr
	argptr = (const void * const *) ppfs->argptr;
	if (ppfs->maxposarg > 0) {	/* Using positional args... */
		argptr += ppfs->argnumber[2] - 1;
	}
#else
	/* Need to build a local copy... */
	{
		register argvalue_t *p = ppfs->argvalue;
		int i;
		if (ppfs->maxposarg > 0) {	/* Using positional args... */
			p += ppfs->argnumber[2] - 1;
		}
		for (i = 0 ; i < ppfs->num_data_args ; i++ ) {
			argptr[i] = (void *) p++;
		}
	}
#endif
	{
		register char *s;

		if (ppfs->conv_num == CONV_n) {
			_store_inttype(*(void **)*argptr,
						   ppfs->info._flags & __PA_INTMASK,
						   (intmax_t) (*count));
			return 0;
		}
		if (ppfs->conv_num <= CONV_i) {	/* pointer or (un)signed int */
			base = spec_base[(int)(ppfs->conv_num - CONV_p)];
			if (ppfs->conv_num <= CONV_u) { /* pointer or unsigned int */
				if (ppfs->conv_num == CONV_p) { /* pointer */
					prefix_num = PREFIX_LWR_X;
				} else {		/* unsigned int */
				}
			} else {			/* signed int */
				base = -base;
			}
			if (ppfs->info.prec < 0) { /* Ignore '0' flag if prec specified. */
				padchar = ppfs->info.pad;
			}
			s = _uintmaxtostr(buf + sizeof(buf) - 1,
							  (uintmax_t)
							  _load_inttype(*argtype & __PA_INTMASK,
											*argptr, base), base,
							  ((ppfs->conv_num == CONV_X)
							   ? __UIM_UPPER : __UIM_LOWER));
			if (ppfs->conv_num > CONV_u) { /* signed int */
				if (*s == '-') {
					PRINT_INFO_SET_FLAG(&(ppfs->info),showsign);
					++s;		/* handle '-' in the prefix string */
					prefix_num = PREFIX_MINUS;
				} else if (PRINT_INFO_FLAG_VAL(&(ppfs->info),showsign)) {
					prefix_num = PREFIX_PLUS;
				} else if (PRINT_INFO_FLAG_VAL(&(ppfs->info),space)) {
					prefix_num = PREFIX_SPACE;
				}
			}
			slen = (char *)(buf + sizeof(buf) - 1) - s;
			numfill = ((ppfs->info.prec < 0) ? 1 : ppfs->info.prec);
			if (PRINT_INFO_FLAG_VAL(&(ppfs->info),alt)) {
				if (ppfs->conv_num <= CONV_x) {	/* x or p */
					prefix_num = PREFIX_LWR_X;
				}
				if (ppfs->conv_num == CONV_X) {
					prefix_num = PREFIX_UPR_X;
				}
				if ((ppfs->conv_num == CONV_o) && (numfill < slen)) {
					numfill = ((*s == '0') ? 1 : slen + 1);
				}
			}
			if (*s == '0') {
				if (prefix_num >= PREFIX_LWR_X) {
					prefix_num = PREFIX_NONE;
				}
				if (ppfs->conv_num == CONV_p) {/* null pointer */
					s = "(nil)";
					slen = 5;
					numfill = 0;
				} else if (numfill == 0) {	/* if precision 0, no output */
					slen = 0;
				}
			}
			numfill = ((numfill > slen) ? numfill - slen : 0);
		} else if (ppfs->conv_num <= CONV_A) {	/* floating point */
#ifdef __STDIO_PRINTF_FLOAT
			*count += _dtostr(stream,
							  (PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
							   ? *(long double *) *argptr
							   : (long double) (* (double *) *argptr)),
							  &ppfs->info);
			return 0;
#else  /* __STDIO_PRINTF_FLOAT */
			return -1;			/* TODO -- try ton continue? */
#endif /* __STDIO_PRINTF_FLOAT */
		} else if (ppfs->conv_num <= CONV_S) {	/* wide char or string */
#if 1
			return -1;			/* TODO -- wide */
#else
			if (ppfs->conv_num == CONV_S) { /* wide string */

			} else {			/* wide char */
				
			}
#endif
		} else if (ppfs->conv_num <= CONV_s) {	/* char or string */
			if (ppfs->conv_num == CONV_s) { /* string */
				s = *((char **) (*argptr));
				if (s) {
				SET_STRING_LEN:
					slen = strnlen(s, ((ppfs->info.prec >= 0)
									   ? ppfs->info.prec : SIZE_MAX));
				} else {
					s = "(null)";
					slen = 6;
				}
			} else {			/* char */
				s = (char *) buf;
				*s = *((const char *) *argptr);
				s[1] = 0;
				slen = 1;
			}
#ifdef __STDIO_PRINTF_M_SUPPORT
		} else if (ppfs->conv_num == CONV_m) {
			s = strerror(errno);
			goto SET_STRING_LEN;
#endif
		} else {
			assert(ppfs->conv_num == CONV_custom0);

			s = _custom_printf_spec;
			do {
				if (*s == ppfs->info.spec) {
					int rv;
					/* TODO -- check return value for sanity? */
					rv = (*_custom_printf_handler
						  [(int)(s-_custom_printf_spec)])
						(stream, &ppfs->info, argptr);
					if (rv < 0) {
						return -1;
					}
					*count += rv;
					return 0;
				}
			} while (++s < (_custom_printf_spec + MAX_USER_SPEC));
			assert(0);
			return -1;
		}

		{
			size_t t;

			t = slen + numfill;
			if (prefix_num != PREFIX_NONE) {
				t += ((prefix_num < PREFIX_LWR_X) ? 1 : 2);
			}
			numpad = ((ppfs->info.width > t) ? (ppfs->info.width - t) : 0);
			*count += t + numpad;
		}
		if (padchar == '0') { /* TODO: check this */
			numfill += numpad;
			numpad = 0;
		}

		/* Now handle the output itself. */
		if (!PRINT_INFO_FLAG_VAL(&(ppfs->info),left)) {
			_charpad(stream, ' ', numpad);
			numpad = 0;
		}
		output(stream, prefix + prefix_num);
		_charpad(stream, '0', numfill);
		_outnstr(stream, s, slen);
		_charpad(stream, ' ', numpad);
	}

	return 0;
}
#endif
/**********************************************************************/
#ifdef L_vsnprintf
#ifdef __STDIO_BUFFERS
int vsnprintf(char *__restrict buf, size_t size,
			  const char * __restrict format, va_list arg)
{
	FILE f;
	int rv;

#ifdef __STDIO_GETC_MACRO
	f.bufgetc =
#endif
	f.bufrpos = f.bufwpos = f.bufstart = buf;

	if (size > SIZE_MAX - (size_t) buf) {
		size = SIZE_MAX - (size_t) buf;
	}
#ifdef __STDIO_PUTC_MACRO
	f.bufputc =
#endif
	f.bufend = buf + size;

#if 0							/* shouldn't be necessary */
/*  #ifdef __STDIO_GLIBC_CUSTOM_STREAMS */
	f.cookie = &(f.filedes);
	f.gcs.read = 0;
	f.gcs.write = 0;
	f.gcs.seek = 0;
	f.gcs.close = 0;
#endif
	f.filedes = -2;				/* for debugging */
	f.modeflags = (__FLAG_NARROW|__FLAG_WRITEONLY|__FLAG_WRITING);

#ifdef __STDIO_THREADSAFE
	__stdio_init_mutex(&f.lock);
#endif

	rv = vfprintf(&f, format, arg);
	if (size) {
		if (f.bufwpos == f.bufend) {
			--f.bufwpos;
		}
		*f.bufwpos = 0;
	}
	return rv;
}
#else  /* __STDIO_BUFFERS */
#ifdef __STDIO_GLIBC_CUSTOM_STREAMS

typedef struct {
	size_t pos;
	size_t len;
	unsigned char *buf;
	FILE *fp;
} __snpf_cookie;

#define COOKIE ((__snpf_cookie *) cookie)

static ssize_t snpf_write(void *cookie, const char *buf, size_t bufsize)
{
	size_t count;
	char *p;

	/* Note: bufsize < SSIZE_MAX because of _stdio_WRITE. */

	if (COOKIE->len > COOKIE->pos) {
		count = COOKIE->len - COOKIE->pos - 1; /* Leave space for nul. */
		if (count > bufsize) {
			count = bufsize;
		}

		p = COOKIE->buf + COOKIE->pos;
		while (count) {
			*p++ = *buf++;
			--count;
		}
		*p = 0;
	}

	COOKIE->pos += bufsize;

	return bufsize;
}

#undef COOKIE

int vsnprintf(char *__restrict buf, size_t size,
			  const char * __restrict format, va_list arg)
{
	FILE f;
	__snpf_cookie cookie;
	int rv;

	cookie.buf = buf;
	cookie.len = size;
	cookie.pos = 0;
	cookie.fp = &f;

	f.cookie = &cookie;
	f.gcs.write = snpf_write;
	f.gcs.read = NULL;
	f.gcs.seek = NULL;
	f.gcs.close = NULL;

	f.filedes = -1;				/* For debugging. */
	f.modeflags = (__FLAG_NARROW|__FLAG_WRITEONLY|__FLAG_WRITING);

#ifdef __STDIO_THREADSAFE
	__stdio_init_mutex(&f.lock);
#endif

	rv = vfprintf(&f, format, arg);

	return rv;
}

#else  /* __STDIO_GLIBC_CUSTOM_STREAMS */
#warning skipping vsnprintf since no buffering and no custom streams!
#endif /* __STDIO_GLIBC_CUSTOM_STREAMS */
#endif /* __STDIO_BUFFERS */
#endif
/**********************************************************************/
#ifdef L_vdprintf
int vdprintf(int filedes, const char * __restrict format, va_list arg)
{
	FILE f;
	int rv;
#ifdef __STDIO_BUFFERS
	char buf[64];				/* TODO: provide _optional_ buffering? */

#ifdef __STDIO_GETC_MACRO
	f.bufgetc =
#endif
	f.bufrpos = f.bufwpos = f.bufstart = buf;
#ifdef __STDIO_PUTC_MACRO
	f.bufputc = 
#endif
	f.bufend = buf + sizeof(buf);
#endif /* __STDIO_BUFFERS */
#ifdef __STDIO_GLIBC_CUSTOM_STREAMS
	f.cookie = &(f.filedes);
	f.gcs.read = _cs_read;
	f.gcs.write = _cs_write;
	f.gcs.seek = 0;				/* The internal seek func handles normals. */
	f.gcs.close = _cs_close;
#endif
	f.filedes = filedes;
	f.modeflags = (__FLAG_NARROW|__FLAG_WRITEONLY|__FLAG_WRITING);

#ifdef __STDIO_THREADSAFE
	__stdio_init_mutex(&f.lock);
#endif

	rv = vfprintf(&f, format, arg);

	return fflush(&f) ? -1 : rv;
}
#endif
/**********************************************************************/
#ifdef L_vasprintf

#if !defined(__STDIO_BUFFERS) && !defined(__STDIO_GLIBC_CUSTOM_STREAMS)
#warning skipping vasprintf since no buffering and no custom streams!
#else

int vasprintf(char **__restrict buf, const char * __restrict format,
			 va_list arg)
{
	/* TODO -- change the default implementation? */
#ifndef __STDIO_GLIBC_CUSTOM_STREAMS
	/* This implementation actually calls the printf machinery twice, but only
	 * only does one malloc.  This can be a problem though when custom printf
	 * specs or the %m specifier are involved because the results of the
	 * second call might be different from the first. */

 	int rv = vsnprintf(NULL, 0, format, arg);

	return (((rv >= 0) && ((*buf = malloc(++rv)) != NULL))
			? vsnprintf(*buf, rv, format, arg)
			: -1);
#else
	FILE *f;
	size_t size;
	int rv;

	/* TODO - do the memstream stuff inline here to avoid fclose and the openlist? */
	if ((f = open_memstream(buf, &size)) == NULL) {
		return -1;
	}
	rv = vfprintf(f, format, arg);
	fclose(f);
	if (rv < 0) {
		free(*buf);
		*buf = NULL;
		return -1;
	}
	return rv;
#endif
}
#endif
#endif
/**********************************************************************/
#ifdef L_vprintf
int vprintf(const char * __restrict format, va_list arg)
{
	return vfprintf(stdout, format, arg);
}
#endif
/**********************************************************************/
#ifdef L_vsprintf

#if !defined(__STDIO_BUFFERS) && !defined(__STDIO_GLIBC_CUSTOM_STREAMS)
#warning skipping vsprintf since no buffering and no custom streams!
#else

int vsprintf(char *__restrict buf, const char * __restrict format,
			 va_list arg)
{
	return vsnprintf(buf, SIZE_MAX, format, arg);
}

#endif
#endif
/**********************************************************************/
#ifdef L_fprintf
int fprintf(FILE * __restrict stream, const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vfprintf(stream, format, arg);
	va_end(arg);

	return rv;
}
#endif
/**********************************************************************/
#ifdef L_snprintf

#if !defined(__STDIO_BUFFERS) && !defined(__STDIO_GLIBC_CUSTOM_STREAMS)
#warning skipping snprintf since no buffering and no custom streams!
#else

int snprintf(char *__restrict buf, size_t size,
			 const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vsnprintf(buf, size, format, arg);
	va_end(arg);
	return rv;
}

#endif
#endif
/**********************************************************************/
#ifdef L_dprintf
int dprintf(int filedes, const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vdprintf(filedes, format, arg);
	va_end(arg);

	return rv;
}
#endif
/**********************************************************************/
#ifdef L_asprintf

#if !defined(__STDIO_BUFFERS) && !defined(__STDIO_GLIBC_CUSTOM_STREAMS)
#warning skipping asprintf and __asprintf since no buffering and no custom streams!
#else

weak_alias(__asprintf,asprintf)

int __asprintf(char **__restrict buf, const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vasprintf(buf, format, arg);
	va_end(arg);

	return rv;
}

#endif
#endif
/**********************************************************************/
#ifdef L_printf
int printf(const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vfprintf(stdout, format, arg);
	va_end(arg);

	return rv;
}
#endif
/**********************************************************************/
#ifdef L_sprintf

#if !defined(__STDIO_BUFFERS) && !defined(__STDIO_GLIBC_CUSTOM_STREAMS)
#warning skipping sprintf since no buffering and no custom streams!
#else

int sprintf(char *__restrict buf, const char * __restrict format, ...)
{
	va_list arg;
	int rv;

	va_start(arg, format);
	rv = vsnprintf(buf, SIZE_MAX, format, arg);
	va_end(arg);

	return rv;
}

#endif
#endif
/**********************************************************************/
#ifdef L__dtostr
/*
 * Copyright (C) 2000, 2001 Manuel Novoa III
 *
 * Function:  size_t _dtostr(FILE *fp, long double x, struct printf_info *info)
 *
 * This was written for uClibc to provide floating point support for
 * the printf functions.  It handles +/- infinity and nan on i386.
 *
 * Notes:
 *
 * At most MAX_DIGITS significant digits are kept.  Any trailing digits
 * are treated as 0 as they are really just the results of rounding noise
 * anyway.  If you want to do better, use an arbitary precision arithmetic
 * package.  ;-)
 *
 * It should also be fairly portable, as not assumptions are made about the
 * bit-layout of doubles.
 *
 * It should be too difficult to convert this to handle long doubles on i386.
 * For information, see the comments below.
 *
 * TODO: 
 *   long double and/or float version?  (note: for float can trim code some).
 *   
 *   Decrease the size.  This is really much bigger than I'd like.
 */

/*****************************************************************************/
/* Don't change anything that follows unless you know what you're doing.     */
/*****************************************************************************/

/*
 * Configuration for the scaling power table.  Ignoring denormals, you
 * should have 2**EXP_TABLE_SIZE >= LDBL_MAX_EXP >= 2**(EXP_TABLE_SIZE-1).
 * The minimum for standard C is 6.  For IEEE 8bit doubles, 9 suffices.
 * For long doubles on i386, use 13.
 */
#define EXP_TABLE_SIZE       13

/* 
 * Set this to the maximum number of digits you want converted.
 * Conversion is done in blocks of DIGITS_PER_BLOCK (9 by default) digits.
 * (20) 17 digits suffices to uniquely determine a (long) double on i386.
 */
#define MAX_DIGITS          20

/*
 * Set this to the smallest integer type capable of storing a pointer.
 */
#define INT_OR_PTR int

/*
 * This is really only used to check for infinities.  The macro produces
 * smaller code for i386 and, since this is tested before any floating point
 * calculations, it doesn't appear to suffer from the excess precision problem
 * caused by the FPU that strtod had.  If it causes problems, call the function
 * and compile zoicheck.c with -ffloat-store.
 */
#define _zero_or_inf_check(x) ( x == (x/4) )

/*
 * Fairly portable nan check.  Bitwise for i386 generated larger code.
 * If you have a better version, comment this out.
 */
#define isnan(x) (x != x)

/*****************************************************************************/
/* Don't change anything that follows peroid!!!  ;-)                         */
/*****************************************************************************/

#include <float.h>

/*****************************************************************************/

/*
 * Set things up for the scaling power table.
 */

#if EXP_TABLE_SIZE < 6
#error EXP_TABLE_SIZE should be at least 6 to comply with standards
#endif

#define EXP_TABLE_MAX      (1U<<(EXP_TABLE_SIZE-1))

/*
 * Only bother checking if this is too small.
 */

#if LDBL_MAX_10_EXP/2 > EXP_TABLE_MAX
#error larger EXP_TABLE_SIZE needed
#endif

/*
 * With 32 bit ints, we can get 9 digits per block.
 */
#define DIGITS_PER_BLOCK     9

#if UINT_MAX >= 4294967295UL
#define DIGIT_BLOCK_TYPE     int
#define DB_FMT               "%.*d"
#elif LONG_MAX >= 4294967295UL
#define DIGIT_BLOCK_TYPE     long
#define DB_FMT               "%.*ld"
#else
#error need at least 32 bit longs
#endif

/* Maximum number of calls to fnprintf to output double. */
#define MAX_CALLS 8

/*****************************************************************************/

#define NUM_DIGIT_BLOCKS   ((MAX_DIGITS+DIGITS_PER_BLOCK-1)/DIGITS_PER_BLOCK)

/* extra space for '-', '.', 'e+###', and nul */
#define BUF_SIZE  ( 3 + NUM_DIGIT_BLOCKS * DIGITS_PER_BLOCK )
/*****************************************************************************/

static const char *fmts[] = {
	"%0*d", "%.*s", ".", "inf", "INF", "nan", "NAN", "%*s"
};

/*****************************************************************************/

size_t _dtostr(FILE * fp, long double x, struct printf_info *info)
{
	long double exp_table[EXP_TABLE_SIZE];
	long double p10;
	DIGIT_BLOCK_TYPE digit_block; /* int of at least 32 bits */
	int i, j;
	int round, o_exp;
	int exp, exp_neg;
	int width, preci;
	char *s;
	char *e;
	char buf[BUF_SIZE];
	INT_OR_PTR pc_fwi[2*MAX_CALLS];
	INT_OR_PTR *ppc;
	char exp_buf[8];
	char drvr[8];
	char *pdrvr;
	int npc;
	int cnt;
	char sign_str[2];
	char o_mode;
	char mode;

	/* check that INT_OR_PTR is sufficiently large */
	assert( sizeof(INT_OR_PTR) == sizeof(char *) );

	width = info->width;
	preci = info->prec;
	mode = info->spec;
	if (mode == 'a') {
		mode = 'g';			/* TODO -- fix */
	}
	if (mode == 'A') {
		mode = 'G';			/* TODO -- fix */
	}

	if (preci < 0) {
		preci = 6;
	}

	*sign_str = '\0';
	if (PRINT_INFO_FLAG_VAL(info,showsign)) {
		*sign_str = '+';
	} else if (PRINT_INFO_FLAG_VAL(info,space)) {
		*sign_str = ' ';
	}
/*  	*sign_str = flag[FLAG_PLUS]; */
	*(sign_str+1) = 0;
	if (isnan(x)) {				/* nan check */
		pdrvr = drvr + 1;
		*pdrvr++ = 5 + (mode < 'a');
		pc_fwi[2] = 3;
		info->pad = ' ';
/*  		flag[FLAG_0_PAD] = 0; */
		goto EXIT_SPECIAL;
	}

	if (x == 0) {				/* handle 0 now to avoid false positive */
		exp = -1;
		goto GENERATE_DIGITS;
	}

	if (x < 0) {				/* convert negatives to positives */
		*sign_str = '-';
		x = -x;
	}

	if (_zero_or_inf_check(x)) { /* must be inf since zero handled above */
		pdrvr = drvr + 1;
		*pdrvr++ = 3 +  + (mode < 'a');
		pc_fwi[2] = 3;
		info->pad = ' ';
/*  		flag[FLAG_0_PAD] = 0; */
		goto EXIT_SPECIAL;
	}

	/* need to build the scaling table */
	for (i = 0, p10 = 10 ; i < EXP_TABLE_SIZE ; i++) {
		exp_table[i] = p10;
		p10 *= p10;
	}

	exp_neg = 0;
	if (x < 1e8) {				/* do we need to scale up or down? */
		exp_neg = 1;
	}

	exp = DIGITS_PER_BLOCK - 1;

	i = EXP_TABLE_SIZE;
	j = EXP_TABLE_MAX;
	while ( i-- ) {				/* scale x such that 1e8 <= x < 1e9 */
		if (exp_neg) {
			if (x * exp_table[i] < 1e9) {
				x *= exp_table[i];
				exp -= j;
			}
		} else {
			if (x / exp_table[i] >= 1e8) {
				x /= exp_table[i];
				exp += j;
			}
		}
		j >>= 1;
	}
	if (x >= 1e9) {				/* handle bad rounding case */
		x /= 10;
		++exp;
	}
	assert(x < 1e9);

 GENERATE_DIGITS:
	s = buf + 2; /* leave space for '\0' and '0' */
#if 1
#define ONE_E_NINE 1000000000L
#else
#define ONE_E_NINE 1e9
#endif   
	for (i = 0 ; i < NUM_DIGIT_BLOCKS ; ++i ) {
		digit_block = (DIGIT_BLOCK_TYPE) x;
		x = (x - digit_block) * ONE_E_NINE;
		s += sprintf(s, DB_FMT, DIGITS_PER_BLOCK, digit_block);
	}

	/*************************************************************************/

	*exp_buf = 'e';
	if (mode < 'a') {
		*exp_buf = 'E';
		mode += ('a' - 'A');
	} 

	o_mode = mode;

	round = preci;

	if ((mode == 'g') && (round > 0)){
		--round;
	}

	if (mode == 'f') {
		round += exp;
	}

	s = buf;
	*s++ = 0;					/* terminator for rounding and 0-triming */
	*s = '0';					/* space to round */

	i = 0;
	e = s + MAX_DIGITS + 1;
	if (round < MAX_DIGITS) {
		e = s + round + 2;
		if (*e >= '5') {
			i = 1;
		}
	}

	do {						/* handle rounding and trim trailing 0s */
		*--e += i;				/* add the carry */
	} while ((*e == '0') || (*e > '9'));

	o_exp = exp;
	if (e <= s) {				/* we carried into extra digit */
		++o_exp;
		e = s;					/* needed if all 0s */
	} else {
		++s;
	}
	*++e = 0;					/* ending nul char */

	if ((mode == 'g') && ((o_exp >= -4) && (o_exp <= round))) {
		mode = 'f';
	}

	exp = o_exp;
	if (mode != 'f') {
		o_exp = 0;
	}

	if (o_exp < 0) {
		*--s = '0';				/* fake the first digit */
	}

	pdrvr = drvr+1;
	ppc = pc_fwi+2;

	*pdrvr++ = 0;
	*ppc++ = 1;
	*ppc++ = (INT_OR_PTR)(*s++ - '0');

	i = e - s;					/* total digits */
	if (o_exp >= 0) {
		if (o_exp >= i) {		/* all digit(s) left of decimal */
			*pdrvr++ = 1;
			*ppc++ = i;
			*ppc++ = (INT_OR_PTR)(s);
			o_exp -= i;
			i = 0;
			if (o_exp>0) {		/* have 0s left of decimal */
				*pdrvr++ = 0;
				*ppc++ = o_exp;
				*ppc++ = 0;
			}
		} else if (o_exp > 0) {	/* decimal between digits */
			*pdrvr++ = 1;
			*ppc++ = o_exp;
			*ppc++ = (INT_OR_PTR)(s);
			s += o_exp;
			i -= o_exp;
		}
		o_exp = -1;
	}

	if (PRINT_INFO_FLAG_VAL(info,alt)
/*  		flag[FLAG_HASH] */
		|| (i) || ((o_mode != 'g') && (preci > 0))) {
		*pdrvr++ = 2;			/* need decimal */
		*ppc++ = 1;				/* needed for width calc */
		ppc++;
	}

	if (++o_exp < 0) {			/* have 0s right of decimal */
		*pdrvr++ = 0;
		*ppc++ = -o_exp;
		*ppc++ = 0;
	}
	if (i) {					/* have digit(s) right of decimal */
		*pdrvr++ = 1;
		*ppc++ = i;
		*ppc++ = (INT_OR_PTR)(s);
	}

	if (o_mode != 'g') {
		i -= o_exp;
		if (i < preci) {		/* have 0s right of digits */
			i = preci - i;
			*pdrvr++ = 0;
			*ppc++ = i;
			*ppc++ = 0;
		}
	}

	/* build exponent string */
	if (mode != 'f') {
		*pdrvr++ = 1;
		*ppc++ = sprintf(exp_buf,"%c%+.2d", *exp_buf, exp);
		*ppc++ = (INT_OR_PTR) exp_buf;
	}

 EXIT_SPECIAL:
	npc = pdrvr - drvr;
	ppc = pc_fwi + 2;
	for (i=1 ; i< npc ; i++) {
		width -= *(ppc++);
		ppc++;
	}
	i = 0;
	if (*sign_str) {
		i = 1;
	}
	width -= i;
	if (width <= 0) {
		width = 0;
	} else {
		if (PRINT_INFO_FLAG_VAL(info,left)) { /* padding on right */
/*  			flag[FLAG_MINUS_LJUSTIFY] */
			++npc;
			*pdrvr++ = 7;
			*ppc = width;
			*++ppc = (INT_OR_PTR)("");
			width = 0;
		} else if (info->pad == '0') { /* 0 padding */
/*  			(flag[FLAG_0_PAD] == '0') */
			pc_fwi[2] += width;
			width = 0;
		}
	}
	*drvr = 7;
	ppc = pc_fwi;
	*ppc++ = width + i;
	*ppc = (INT_OR_PTR) sign_str;

	pdrvr = drvr;
	ppc = pc_fwi;
	cnt = 0;
	for (i=0 ; i<npc ; i++) {
#if 1
		fprintf(fp, fmts[(int)(*pdrvr++)], (INT_OR_PTR)(*(ppc)), 
				 (INT_OR_PTR)(*(ppc+1)));
#else
		j = fprintf(fp, fmts[(int)(*pdrvr++)], (INT_OR_PTR)(*(ppc)), 
					  (INT_OR_PTR)(*(ppc+1)));
		assert(j == *ppc);
#endif
/*  		if (size > *ppc) { */
/*  			size -= *ppc; */
/*  		} */
		cnt += *ppc;			/* to avoid problems if j == -1 */
		ppc += 2;
	}

	return cnt;
}
#endif
/**********************************************************************/
#ifdef L__store_inttype
/* TODO -- right now, assumes intmax_t is either long or long long */

/* We assume int may be short or long, but short and long are different. */

void _store_inttype(register void *dest, int desttype, uintmax_t val)
{
	if (desttype == __PA_FLAG_CHAR) { /* assume char not int */
		*((unsigned char *) dest) = val;
		return;
	}
#if defined(LLONG_MAX) && (LONG_MAX != LLONG_MAX)
	if (desttype == PA_FLAG_LONG_LONG) {
		*((unsigned long long int *) dest) = val;
		return;
	}
#endif /* LLONG_MAX */
#if SHRT_MAX != INT_MAX
	if (desttype == PA_FLAG_SHORT) {
		*((unsigned short int *) dest) = val;
		return;
	}
#endif /* SHRT_MAX */
#if LONG_MAX != INT_MAX
	if (desttype == PA_FLAG_LONG) {
		*((unsigned long int *) dest) = val;
		return;
	}
#endif /* LONG_MAX */

	*((unsigned int *) dest) = val;
}

#endif
/**********************************************************************/
#ifdef L__load_inttype

extern uintmax_t _load_inttype(int desttype, const void *src, int uflag)
{
	if (uflag >= 0) {			/* unsigned */
#if LONG_MAX != INT_MAX
		if (desttype & (PA_FLAG_LONG|PA_FLAG_LONG_LONG)) {
#ifdef LLONG_MAX
			if (desttype == PA_FLAG_LONG_LONG) {
				return *((unsigned long long int *) src);
			}
#endif
			return *((unsigned long int *) src);
		}
#else  /* LONG_MAX != INT_MAX */
#ifdef LLONG_MAX
		if (desttype & PA_FLAG_LONG_LONG) {
			return *((unsigned long long int *) src);
		}
#endif
#endif /* LONG_MAX != INT_MAX */
		{
			unsigned int x;
			x = *((unsigned int *) src);
			if (desttype == __PA_FLAG_CHAR) x = (unsigned char) x;
#if SHRT_MAX != INT_MAX
			if (desttype == PA_FLAG_SHORT) x = (unsigned short int) x;
#endif
			return x;
		}
	} else {					/* signed */
#if LONG_MAX != INT_MAX
		if (desttype & (PA_FLAG_LONG|PA_FLAG_LONG_LONG)) {
#ifdef LLONG_MAX
			if (destsize == PA_FLAG_LONG_LONG) {
				return *((long long int *) src);
			}
#endif
			return *((long int *) src);
		}
#else  /* LONG_MAX != INT_MAX */
#ifdef LLONG_MAX
		if (desttype & PA_FLAG_LONG_LONG) {
			return *((long long int *) src);
		}
#endif
#endif /* LONG_MAX != INT_MAX */
		{
			int x;
			x = *((int *) src);
			if (desttype == __PA_FLAG_CHAR) x = (char) x;
#if SHRT_MAX != INT_MAX
			if (desttype == PA_FLAG_SHORT) x = (short int) x;
#endif
			return x;
		}
	}
}

#endif
/**********************************************************************/
