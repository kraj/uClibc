/*
 *  vsprintf.c
 *
 *  Copyright (C) 1991-1996  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */

#include <stdarg.h>
#include "string.h"
#include "hash.h"
#include <linux/unistd.h>
#include "syscall.h"

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

#ifndef __sparc__
#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })
#else
#define do_div(n,base) _dl_div ((n)/(base))
#define do_div(n,base) ({ \
int __res; \
__res = _dl_urem(((unsigned long) n),(unsigned) base); \
n = _dl_udiv(((unsigned long) n),(unsigned) base); \
__res; })
#endif

#define ADD_CHAR(s,n,c) ( ((n) > 1) ? *(s)++ = (c), (n)-- : (c) )

static char * number(char * str, int *bufsize, long num, int base, int size, int precision
	,int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0)
		tmp[i++] = digits[do_div(num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			ADD_CHAR(str, *bufsize, ' ');
	if (sign)
		ADD_CHAR(str, *bufsize, sign);
	if (type & SPECIAL) {
		if (base==8)
			ADD_CHAR(str, *bufsize, '0');
		else if (base==16) {
			ADD_CHAR(str, *bufsize, '0');
			ADD_CHAR(str, *bufsize, digits[33]);
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			ADD_CHAR(str, *bufsize, c);
	while (i < precision--)
		ADD_CHAR(str, *bufsize, '0');
	while (i-- > 0)
		ADD_CHAR(str, *bufsize, tmp[i]);
	while (size-- > 0)
		ADD_CHAR(str, *bufsize, ' ');
	return str;
}

int _dl_fdprintf(int fd, const char *fmt, ...)
{
	int len;
	unsigned long num;
	int i, base;
	char * str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	int bufsize;
	char buf[2048];
	va_list(args);

	va_start(args, fmt);

	for (str=buf, bufsize=sizeof buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			ADD_CHAR(str, bufsize, *fmt);
			continue;
		}
			
		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					ADD_CHAR(str, bufsize, ' ');
			ADD_CHAR(str, bufsize, (unsigned char) va_arg(args, int));
			while (--field_width > 0)
				ADD_CHAR(str, bufsize, ' ');
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = _dl_strlen(s);

			if (!(flags & LEFT))
				while (len < field_width--)
					ADD_CHAR(str, bufsize, ' ');
			for (i = 0; i < len; ++i)
			        ADD_CHAR(str, bufsize, *s++);
			while (len < field_width--)
				ADD_CHAR(str, bufsize, ' ');
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str, &bufsize,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			if (*fmt != '%')
				ADD_CHAR(str, bufsize, '%');
			if (*fmt)
				ADD_CHAR(str, bufsize, *fmt);
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h')
			if (flags & SIGN)
				num = va_arg(args, short);
			else
				num = va_arg(args, unsigned short);
		else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, &bufsize, num, base, field_width, precision, flags);
	}
	*str = '\0';
	_dl_write(fd, buf, str-buf);
	return str-buf;
}

