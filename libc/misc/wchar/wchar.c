/*  Copyright (C) 2002     Manuel Novoa III
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

/*  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!
 *
 *  Besides uClibc, I'm using this code in my libc for elks, which is
 *  a 16-bit environment with a fairly limited compiler.  It would make
 *  things much easier for me if this file isn't modified unnecessarily.
 *  In particular, please put any new or replacement functions somewhere
 *  else, and modify the makefile to use your version instead.
 *  Thanks.  Manuel
 *
 *  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION! */


/* May 23, 2002     Initial Notes:
 *
 * I'm still tweaking this stuff, but it passes the tests I've thrown
 * at it, and Erik needs it for the gcc port.  The glibc extension
 * __wcsnrtombs() hasn't been tested, as I didn't find a test for it
 * in the glibc source.  I also need to fix the behavior of
 * _wchar_utf8sntowcs() if the max number of wchars to convert is 0.
 *
 * UTF-8 -> wchar -> UTF-8 conversion tests on Markus Kuhn's UTF-8-demo.txt
 * file on my platform (x86) show about 5-10% faster conversion speed than
 * glibc with mbsrtowcs()/wcsrtombs() and almost twice as fast as glibc with
 * individual mbrtowc()/wcrtomb() calls.
 *
 * If 'DECODER' is defined, then _wchar_utf8sntowcs() will be compiled
 * as a fail-safe UTF-8 decoder appropriate for a terminal, etc.  which
 * needs to deal gracefully with whatever is sent to it.  In that mode,
 * it passes Markus Kuhn's UTF-8-test.txt stress test.  I plan to add
 * an arg to force that behavior, so the interface will be changing.
 *
 * I need to fix the error checking for 16-bit wide chars.  This isn't
 * an issue for uClibc, but may be for ELKS.  I'm currently not sure
 * if I'll use 16-bit, 32-bit, or configureable wchars in ELKS.
 *
 * July 1, 2002
 *
 * Fixed _wchar_utf8sntowcs() for the max number of wchars == 0 case.
 * Fixed nul-char bug in btowc(), and another in __mbsnrtowcs() for 8-bit
 *    locales.
 * Enabled building of a C/POSIX-locale-only version, so full locale support
 *    no longer needs to be enabled.
 *
 * Manuel
 */

#define _GNU_SOURCE
#define _ISOC99_SOURCE
#include <errno.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <wchar.h>

#ifdef __UCLIBC_HAS_LOCALE__
#define ENCODING (__global_locale.encoding)
#warning implement __CTYPE_HAS_UTF_8_LOCALES!
#define __CTYPE_HAS_UTF_8_LOCALES
#else
#define ENCODING (__ctype_encoding_7_bit)
#undef __CTYPE_HAS_8_BIT_LOCALES
#undef __CTYPE_HAS_UTF_8_LOCALES
#undef L__wchar_utf8sntowcs
#undef L__wchar_wcsntoutf8s
#endif

#if WCHAR_MAX > 0xffffU
#define UTF_8_MAX_LEN 6
#else
#define UTF_8_MAX_LEN 3
#endif

/*  #define KUHN */

/* Implementation-specific work functions. */

extern size_t _wchar_utf8sntowcs(wchar_t *__restrict pwc, size_t wn,
								 const char **__restrict src, size_t n,
								 mbstate_t *ps, int allow_continuation);

extern size_t _wchar_wcsntoutf8s(char *__restrict s, size_t n,
								 const wchar_t **__restrict src, size_t wn);

/* glibc extensions. */

extern size_t __mbsnrtowcs(wchar_t *__restrict dst,
						   const char **__restrict src,
						   size_t NMC, size_t len, mbstate_t *__restrict ps);

extern size_t __wcsnrtombs(char *__restrict dst,
						   const wchar_t **__restrict src,
						   size_t NWC, size_t len, mbstate_t *__restrict ps);

/**********************************************************************/
#ifdef L_btowc

wint_t btowc(int c)
{
#ifdef __CTYPE_HAS_8_BIT_LOCALES

	wchar_t wc;
	unsigned char buf[1];
	mbstate_t mbstate;

	if (c != EOF) {
		*buf = (unsigned char) c;
		mbstate.mask = 0;		/* Initialize the mbstate. */
		if (mbrtowc(&wc, buf, 1, &mbstate) <= 1) {
			return wc;
		}
	}
	return WEOF;

#else  /*  __CTYPE_HAS_8_BIT_LOCALES */

	/* If we don't have 8-bit locale support, then this is trivial since
	 * anything outside of 0-0x7f is illegal in C/POSIX and UTF-8 locales. */
	return (((unsigned int)c) < 0x80) ? c : WEOF;

#endif /*  __CTYPE_HAS_8_BIT_LOCALES */
}

#endif
/**********************************************************************/
#ifdef L_wctob

/* Note: We completely ignore ps in all currently supported conversions. */

int wctob(wint_t c)
{
#ifdef __CTYPE_HAS_8_BIT_LOCALES

	unsigned char buf[MB_LEN_MAX];

	return (wcrtomb(buf, c, NULL) == 1) ? *buf : EOF;

#else  /*  __CTYPE_HAS_8_BIT_LOCALES */

	/* If we don't have 8-bit locale support, then this is trivial since
	 * anything outside of 0-0x7f is illegal in C/POSIX and UTF-8 locales. */
	
	/* TODO: need unsigned version of wint_t... */
/*  	return (((unsigned int)c) < 0x80) ? c : WEOF; */
	return ((c >= 0) && (c < 0x80)) ? c : EOF;

#endif /*  __CTYPE_HAS_8_BIT_LOCALES */
}

#endif
/**********************************************************************/
#ifdef L_mbsinit

int mbsinit(const mbstate_t *ps)
{
	return !ps || !ps->mask;
}

#endif
/**********************************************************************/
#ifdef L_mbrlen

size_t mbrlen(const char *__restrict s, size_t n, mbstate_t *__restrict ps)
	 __attribute__ ((__weak__, __alias__("__mbrlen")));

size_t __mbrlen(const char *__restrict s, size_t n, mbstate_t *__restrict ps)
{
	static mbstate_t mbstate;	/* Rely on bss 0-init. */

	return mbrtowc(NULL, s, n, (ps != NULL) ? ps : &mbstate);
}

#endif
/**********************************************************************/
#ifdef L_mbrtowc

size_t mbrtowc(wchar_t *__restrict pwc, const char *__restrict s,
			   size_t n, mbstate_t *__restrict ps)
{
	static mbstate_t mbstate;	/* Rely on bss 0-init. */
	wchar_t wcbuf[1];
	const char *p;
	size_t r;
	char empty_string[1];		/* Avoid static to be fPIC friendly. */

	if (!ps) {
		ps = &mbstate;
	}

	if (!s) {
		pwc = (wchar_t *) s;	/* NULL */
		empty_string[0] = 0;	/* Init the empty string when necessary. */
		s = empty_string;
		n = 1;
	} else if (!n) {
		return (ps->mask && (ps->wc == 0xffffU)) /* TODO: change error code? */
			? ((size_t) -1) : ((size_t) -2);
	}

	p = s;

#ifdef __CTYPE_HAS_UTF_8_LOCALES
	/* Need to do this here since mbsrtowcs doesn't allow incompletes. */
	if (ENCODING == __ctype_encoding_utf8) {
		r = _wchar_utf8sntowcs(pwc, 1, &p, n, ps, 1);
		return (r == 1) ? (p-s) : r; /* Need to return 0 if nul char. */
	}
#endif

	r = __mbsnrtowcs(wcbuf, &p, SIZE_MAX, 1, ps);

	if (((ssize_t) r) >= 0) {
		if (pwc) {
			*pwc = *wcbuf;
		}
	}
	return (size_t) r;
}

#endif
/**********************************************************************/
#ifdef L_wcrtomb

/* Note: We completely ignore ps in all currently supported conversions. */
/* TODO: Check for valid state anyway? */

size_t wcrtomb(register char *__restrict s, wchar_t wc,
			   mbstate_t *__restrict ps)
{
	wchar_t wcbuf[2];
	const wchar_t *pwc;
	size_t r;
	char buf[MB_LEN_MAX];

	if (!s) {
		s = buf;
		wc = 0;
	}

	pwc = wcbuf;
	wcbuf[0] = wc;
	wcbuf[1] = 0;

	r = __wcsnrtombs(s, &pwc, SIZE_MAX, MB_LEN_MAX, ps);
	return (r != 0) ? r : 1;
}

#endif
/**********************************************************************/
#ifdef L_mbsrtowcs

size_t mbsrtowcs(wchar_t *__restrict dst, const char **__restrict src,
				 size_t len, mbstate_t *__restrict ps)
{
	static mbstate_t mbstate;	/* Rely on bss 0-init. */

	return __mbsnrtowcs(dst, src, SIZE_MAX, len,
						((ps != NULL) ? ps : &mbstate));
}

#endif
/**********************************************************************/
#ifdef L_wcsrtombs

/* Note: We completely ignore ps in all currently supported conversions.

 * TODO: Check for valid state anyway? */

size_t wcsrtombs(char *__restrict dst, const wchar_t **__restrict src,
				 size_t len, mbstate_t *__restrict ps)
{
	return __wcsnrtombs(dst, src, SIZE_MAX, len, ps);
}

#endif
/**********************************************************************/
#ifdef L__wchar_utf8sntowcs

/* Define DECODER to generate a UTF-8 decoder which passes Markus Kuhn's
 * UTF-8-test.txt strss test.
 */
/*  #define DECODER */

#ifdef DECODER
#ifndef KUHN
#define KUHN
#endif
#endif

size_t _wchar_utf8sntowcs(wchar_t *__restrict pwc, size_t wn,
						  const char **__restrict src, size_t n,
						  mbstate_t *ps, int allow_continuation)
{
	register const char *s;
	__uwchar_t mask;
	__uwchar_t wc;
	wchar_t wcbuf[1];
	size_t count;
	int incr;

	s = *src;

	assert(s != NULL);
	assert(ps != NULL);

	incr = 1;
	if (!pwc) {
		pwc = wcbuf;
		wn = SIZE_MAX;
		incr = 0;
	}

	/* This is really here only to support the glibc extension function
	 * __mbsnrtowcs which apparently returns 0 if wn == 0 without any
	 * check on the validity of the mbstate. */
	if (!(count = wn)) {
		return 0;
	}

	if ((mask = (__uwchar_t) ps->mask) != 0) { /* A continuation... */
#ifdef DECODER
		wc = (__uwchar_t) ps->wc;
		if (n) {
			goto CONTINUE;
		}
		goto DONE;
#else
		if ((wc = (__uwchar_t) ps->wc) != 0xffffU) {
			/* TODO: change error code here and below? */
			if (n) {
				goto CONTINUE;
			}
			goto DONE;
		}
		return (size_t) -1;		/* We're in an error state. */
#endif
	}

	do {
		if (!n) {
			goto DONE;
		}
		--n;
		if ((wc = ((unsigned char) *s++)) >= 0x80) { /* Not ASCII... */
			mask = 0x40;
#warning fix range for 16 bit wides
			if ( ((unsigned char)(s[-1] - 0xc0)) < (0xfe - 0xc0) ) {
				goto START;
			}
		BAD:
#ifdef DECODER
			wc = 0xfffd;
			goto COMPLETE;
#else
			ps->mask = mask;
			ps->wc = 0xffffU;
			return (size_t) -1;	/* Illegal start byte! */
#endif

		CONTINUE:
			while (n) {
				--n;
				if ((*s & 0xc0) != 0x80) {
					goto BAD;
				}
				mask <<= 5;
				wc <<= 6;
				wc += (*s & 0x3f);	/* keep seperate for bcc (smaller code) */
				++s;
			START:
				wc &= ~(mask << 1);

				if ((wc & mask) == 0) {	/* Character completed. */
					if ((mask >>= 5) == 0x40) {
						mask += mask;
					}
					/* Check for invalid sequences (longer than necessary)
					 * and invalid chars.  */
					if ( (wc < mask) /* Sequence not minimal length. */
#ifdef KUHN
#if UTF_8_MAX_LEN == 3
#error broken since mask can overflow!!
						 /* For plane 0, these are the only defined values.*/
						 || (wc > 0xfffdU)
#else
						 /* Note that we don't need to worry about exceeding */
						 /* 31 bits as that is the most that UTF-8 provides. */
						 || ( ((__uwchar_t)(wc - 0xfffeU)) < 2)
#endif
						 || ( ((__uwchar_t)(wc - 0xd800U)) < (0xe000U - 0xd800U) )
#endif /* KUHN */
						 ) {
						goto BAD;
					}
					goto COMPLETE;
				}
			}
			/* Character potentially valid but incomplete. */
			if (!allow_continuation) {
				if (count != wn) {
					return 0;
				}
				/* NOTE: The following can fail if you allow and then disallow
				 * continuation!!! */
#if UTF_8_MAX_LEN == 3
#error broken since mask can overflow!!
#endif
				/* Need to back up... */
				do {
					--s;
				} while ((mask >>= 5) >= 0x40);
				goto DONE;
			}
			ps->mask = (wchar_t) mask;
			ps->wc = (wchar_t) wc;
			*src = s;
			return (size_t) -2;
		}
	COMPLETE:
		*pwc = wc;
		pwc += incr;

	}
#ifdef DECODER
	while (--count);
#else
	while (wc && --count);

	if (!wc) {
		s = NULL;
	}
#endif

 DONE:
	/* ps->wc is irrelavent here. */
	ps->mask = 0;
	if (pwc != wcbuf) {
		*src = s;
	}

	return wn - count;
}

#endif
/**********************************************************************/
#ifdef L__wchar_wcsntoutf8s

size_t _wchar_wcsntoutf8s(char *__restrict s, size_t n,
						  const wchar_t **__restrict src, size_t wn)
{
	register char *p;
	size_t len, t;
	__uwchar_t wc;
	const __uwchar_t *swc;
	int store;
	char buf[MB_LEN_MAX];
	char m;

	store = 1;
	if (!s) {
		s = buf;
		n = SIZE_MAX;
		store = 0;
	}

	t = n;
	swc = (const __uwchar_t *) *src;

	assert(swc != NULL);

	while (wn && t) {
		wc = *swc;

		*s = wc;
		len = 1;

		if (wc >= 0x80) {
#ifdef KUHN
			if (
#if UTF_8_MAX_LEN == 3
				/* For plane 0, these are the only defined values.*/
				/* Note that we don't need to worry about exceeding */
				/* 31 bits as that is the most that UTF-8 provides. */
				(wc > 0xfffdU)
#else
				/* UTF_8_MAX_LEN == 6 */
				(wc > 0x7fffffffUL)
				|| ( ((__uwchar_t)(wc - 0xfffeU)) < 2)
#endif
				|| ( ((__uwchar_t)(wc - 0xd800U)) < (0xe000U - 0xd800U) )
				) {
				return (size_t) -1;
			}
#else  /* KUHN */
#if UTF_8_MAX_LEN != 3
			if (wc > 0x7fffffffUL) { /* Value too large. */
				return (size_t) -1;
			}
#endif
#endif /* KUHN */

			wc >>= 1;
			p = s;
			do {
				++p;
			} while (wc >>= 5);
			wc = *swc;
			if ((len = p - s) > t) { /* Not enough space. */
				break;
			}

			m = 0x80;
			while( p>s ) {
				m = (m >> 1) | 0x80;
				*--p = (wc & 0x3f) | 0x80;
				wc >>= 6;
			}
			*s |= (m << 1);
		} else if (wc == 0) {	/* End of string. */
			swc = NULL;
			break;
		}

		++swc;
		--wn;
		t -= len;
		if (store) {
			s += len;
		}
	}

	*src = (const wchar_t *) swc;

	return n - t;
}


#endif
/**********************************************************************/
#ifdef L___mbsnrtowcs

/* WARNING: We treat len as SIZE_MAX when dst is NULL! */

size_t mbsnrtowcs(wchar_t *__restrict dst, const char **__restrict src,
				  size_t NMC, size_t len, mbstate_t *__restrict ps)
	 __attribute__ ((__weak__, __alias__("__mbsnrtowcs")));

size_t __mbsnrtowcs(wchar_t *__restrict dst, const char **__restrict src,
					size_t NMC, size_t len, mbstate_t *__restrict ps)
{
	static mbstate_t mbstate;	/* Rely on bss 0-init. */
	wchar_t wcbuf[1];
	const char *s;
	size_t count, r;
	int incr;

	if (!ps) {
		ps = &mbstate;
	}

#ifdef __CTYPE_HAS_UTF_8_LOCALES
	if (ENCODING == __ctype_encoding_utf8) {
		return ((r = _wchar_utf8sntowcs(dst, len, src, NMC, ps, 1))
				!= (size_t) -2) ? r : 0;
	}
#endif
	incr = 1;
	if (!dst) {
		dst = wcbuf;
		len = SIZE_MAX;
		incr = 0;
	}

	/* Since all the following encodings are single-byte encodings... */
	if (len > NMC) {
		len = NMC;
	}

	count = len;
	s = *src;

#ifdef __CTYPE_HAS_8_BIT_LOCALES
	if (ENCODING == __ctype_encoding_8_bit) {
		wchar_t wc;
		while (count) {
			if ((wc = ((unsigned char)(*s))) >= 0x80) {	/* Non-ASCII... */
				wc -= 0x80;
				wc = __global_locale.tbl8c2wc[
						  (__global_locale.idx8c2wc[wc >> Cc2wc_IDX_SHIFT]
						   << Cc2wc_IDX_SHIFT) + (wc & (Cc2wc_ROW_LEN - 1))];
				if (!wc) {
					goto BAD;
				}
			}
			if (!(*dst = wc)) {
				s = NULL;
				break;
			}
			dst += incr;
			++s;
			--count;
		}
		if (dst != wcbuf) {
			*src = s;
		}
		return len - count;
	}
#endif

	assert(ENCODING == __ctype_encoding_7_bit);

	while (count) {
		if ((*dst = (unsigned char) *s) == 0) {
			s = NULL;
			break;
		}
		if (*dst >= 0x80) {
#ifdef __CTYPE_HAS_8_BIT_LOCALES
		BAD:
#endif
			__set_errno(EILSEQ);
			return (size_t) -1;
		}
		++s;
		dst += incr;
		--count;
	}
	if (dst != wcbuf) {
		*src = s;
	}
	return len - count;
}

#endif
/**********************************************************************/
#ifdef L___wcsnrtombs

/* WARNING: We treat len as SIZE_MAX when dst is NULL! */

/* Note: We completely ignore ps in all currently supported conversions.
 * TODO: Check for valid state anyway? */

size_t wcsnrtombs(char *__restrict dst, const wchar_t **__restrict src,
				  size_t NWC, size_t len, mbstate_t *__restrict ps)
	 __attribute__ ((__weak__, __alias__("__wcsnrtombs")));

size_t __wcsnrtombs(char *__restrict dst, const wchar_t **__restrict src,
					size_t NWC, size_t len, mbstate_t *__restrict ps)
{
	const __uwchar_t *s;
	size_t count;
	int incr;
	char buf[MB_LEN_MAX];

#ifdef __CTYPE_HAS_UTF_8_LOCALES
	if (ENCODING == __ctype_encoding_utf8) {
		return _wchar_wcsntoutf8s(dst, len, src, NWC);
	}
#endif /* __CTYPE_HAS_UTF_8_LOCALES */

	incr = 1;
	if (!dst) {
		dst = buf;
		len = SIZE_MAX;
		incr = 0;
	}

	/* Since all the following encodings are single-byte encodings... */
	if (len > NWC) {
		len = NWC;
	}

	count = len;
	s = (const __uwchar_t *) *src;

#ifdef __CTYPE_HAS_8_BIT_LOCALES
	if (ENCODING == __ctype_encoding_8_bit) {
		__uwchar_t wc;
		__uwchar_t u;
		while (count) {
			if ((wc = *s) <= 0x7f) {
				if (!(*dst = (unsigned char) wc)) {
					s = NULL;
					break;
				}
			} else {
				u = 0;
				if (wc <= Cwc2c_DOMAIN_MAX) {
					u = __global_locale.idx8wc2c[wc >> (Cwc2c_TI_SHIFT
														+ Cwc2c_TT_SHIFT)];
					u = __global_locale.tbl8wc2c[(u << Cwc2c_TI_SHIFT)
									+ ((wc >> Cwc2c_TT_SHIFT)
									   & ((1 << Cwc2c_TI_SHIFT)-1))];
					u = __global_locale.tbl8wc2c[Cwc2c_TI_LEN
									+ (u << Cwc2c_TT_SHIFT)
									+ (wc & ((1 << Cwc2c_TT_SHIFT)-1))];
				}

/*  #define __WCHAR_REPLACEMENT_CHAR '?' */
#ifdef __WCHAR_REPLACEMENT_CHAR
				*dst = (unsigned char) ( u ? u : __WCHAR_REPLACEMENT_CHAR );
#else  /* __WCHAR_REPLACEMENT_CHAR */
				if (!u) {
					goto BAD;
				}
				*dst = (unsigned char) u;
#endif /* __WCHAR_REPLACEMENT_CHAR */
			}
			++s;
			dst += incr;
			--count;
		}
		if (dst != buf) {
			*src = (const wchar_t *) s;
		}
		return len - count;
	}
#endif /* __CTYPE_HAS_8_BIT_LOCALES */

	assert(ENCODING == __ctype_encoding_7_bit);

	while (count) {
		if (*s >= 0x80) {
#if defined(__CTYPE_HAS_8_BIT_LOCALES) && !defined(__WCHAR_REPLACEMENT_CHAR)
		BAD:
#endif
			__set_errno(EILSEQ);
			return (size_t) -1;

		}
		if ((*dst = (unsigned char) *s) == 0) {
			s = NULL;
			break;
		}
		++s;
		dst += incr;
		--count;
	}
	if (dst != buf) {
		*src = (const wchar_t *) s;
	}
	return len - count;
}

#endif
/**********************************************************************/
