/*
 * regexp.h -- old-style regexp compile and step (emulated with POSIX regex)
 * Copyright (C) 1993 Rick Sladkey <jrs@world.std.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library Public License for more details.
 */

/*
 * Think really hard before you intentionally include this file.
 * You should really be using the POSIX regex interface instead.
 * This emulation file is intended solely for compiling old code.
 *
 * A program that uses this file must define six macros: INIT,
 * GETC, PEEKC, UNGETC, RETURN, and ERROR.  This interface is
 * so arcane that VMS hackers point at it in ridicule.
 */

#ifndef _REGEXP_H
#define _REGEXP_H

#include <sys/types.h>			/* regex.h needs size_t */
#include <regex.h>			/* POSIX.2 regexp routines */
#include <stdlib.h>			/* for malloc, realloc and free */

/*
 * These three advertised external variables record state information
 * for compile and step.  They are so gross, I'm choking as I write this.
 */
char *loc1;				/* the beginning of a match */
char *loc2;				/* the end of a match */
int circf;				/* current pattern begins with '^' */

/*
 * These are the other variables mentioned in the regexp.h manpage.
 * Since we don't emulate them (whatever they do), we want errors if
 * they are referenced.  Therefore they are commented out here.
 */
#if 0
char *locs;
int sed;
int nbra;
#endif

/*
 * We need to stuff a regex_t into an arbitrary buffer so align it.
 * GCC make this easy.  For the others we have to guess.
 */
#ifdef __GNUC__
#define __REGEX_T_ALIGN (__alignof__(regex_t))
#else /* !__GNUC__ */
#define __REGEX_T_ALIGN 8
#endif /* !__GNUC__ */

#define __regex_t_align(p)						\
	((regex_t *) ((((unsigned long) p) + __REGEX_T_ALIGN - 1)	\
		/ __REGEX_T_ALIGN * __REGEX_T_ALIGN))

/*
 * We just slurp the whole pattern into a string and then compile
 * it `normally'.  With this implementation we never use the PEEKC
 * macro.  Please feel free to die laughing when we translate
 * error symbols into hard-coded numbers.
 */
char *
compile(char *instring, char *expbuf, char *endbuf, int eof)
{
	int __c;
	int __len;
	char *__buf;
	int __buflen;
	int __error;
	regex_t *__preg;
	INIT;

	__buflen = 128;
	__buf = malloc(__buflen);
	if (!__buf) {
		ERROR(50);
		return 0;
	}
	__len = 0;
	circf = 0;
	for (;;) {
		__c = GETC();
		if (__c == eof)
			break;
		if (__c == '\0' || __c == '\n') {
			UNGETC(__c);
			break;
		}
		if (__len + 2 > __buflen) {
			__buflen *= 2;
			__buf = realloc(__buf, __buflen);
			if (!__buf) {
				ERROR(50);
				return 0;
			}
		}
		if (__len == 0 && !circf && __c == '^')
			circf = 1;
		else
			__buf[__len++] = __c;
	}
	if (__len == 0 && !circf) {
		free(__buf);
		ERROR(41);
		return 0;
	}
	__buf[__len] = '\0';
	if (endbuf <= expbuf + sizeof(regex_t)) {
		free(__buf);
		ERROR(50);
		return 0;
	}
	__preg = __regex_t_align(expbuf);
	__preg->buffer = (char *) (__preg + 1);
	__preg->allocated = endbuf - (char *) __preg->buffer;
	__error = regcomp(__preg, __buf, REG_NEWLINE);
	free(__buf);
	switch (__error) {
	case 0:
		break;
	case REG_BADRPT:
		__error = 36; /* poor fit */
		break;
	case REG_BADBR:
		__error = 16;
		break;
	case REG_EBRACE:
		__error = 44; /* poor fit */
		break;
	case REG_EBRACK:
		__error = 49;
		break;
	case REG_ERANGE:
		__error = 36; /* poor fit */
		break;
	case REG_ECTYPE:
		__error = 36; /* poor fit */
		break;
	case REG_EPAREN:
		__error = 42;
		break;
	case REG_ESUBREG:
		__error = 36; /* poor fit */
		break;
	case REG_EEND:
		__error = 36; /* poor fit */
		break;
	case REG_EESCAPE:
		__error = 36;
		break;
	case REG_BADPAT:
		__error = 36; /* poor fit */
		break;
	case REG_ESIZE:
		__error = 50;
		break;
	case REG_ESPACE:
		__error = 50;
		break;
	default:
		__error = 36; /* as good as any */
		break;
	}
	if (__error) {
		ERROR(__error);
		return 0;
	}
#ifdef _RX_H
	RETURN((__preg->buffer + __preg->rx.allocated - __preg->rx.reserved));
#else
	RETURN((__preg->buffer + __preg->used));
#endif
}

/*
 * Note how we carefully emulate the gross `circf' hack.  Otherwise,
 * this just looks like an ordinary matching call that records the
 * starting and ending match positions.
 */
int
step(char *string, char *expbuf)
{
	int __result;
	regmatch_t __pmatch[1];

	__result = regexec(__regex_t_align(expbuf), string, 1, __pmatch, 0);
	if (circf && __pmatch[0].rm_so != 0)
		__result = REG_NOMATCH;
	if (__result == 0) {
		loc1 = string + __pmatch[0].rm_so;
		loc2 = string + __pmatch[0].rm_eo;
	}
	return __result == 0;
}

/*
 * For advance we are only supposed to match at the beginning of the
 * string.  You have to read the man page really carefully to find this
 * one.  We'll match them kludge-for-kludge.
 */
int
advance(char *string, char *expbuf)
{
	int __old_circf;
	int __result;

	__old_circf = circf;
	circf = 1;
	__result = step(string, expbuf);
	circf = __old_circf;
	return __result;
}

#endif /* _REGEXP_H */
