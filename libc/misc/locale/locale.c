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

/*  TODO:
 *  Implement the shared mmap code so non-mmu platforms can use this.
 *  Implement nl_langinfo() for the stub locale support.
 *  Add some basic collate functionality similar to what the previous
 *    locale support had (8-bit codesets only).
 */

#define _GNU_SOURCE
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

#ifdef __LOCALE_C_ONLY

#ifdef __WCHAR_ENABLED
#error wide char support requires full locale support
#endif

#else  /* __LOCALE_C_ONLY */

#define CUR_LOCALE_SPEC			(__global_locale.cur_locale)
#undef CODESET_LIST
#define CODESET_LIST			(__locale_mmap->codeset_list)

/* TODO: Optional... See below. */
#define __LOCALE_STRICTER_SETLOCALE

#endif /* __LOCALE_C_ONLY */

/**********************************************************************/
#ifdef L_setlocale

#ifdef __LOCALE_C_ONLY

link_warning(setlocale,"the 'setlocale' function supports only C|POSIX locales");

static const char C_string[] = "C";

char *setlocale(int category, register const char *locale)
{
	return ( (((unsigned int)(category)) <= LC_ALL)
			 && ( (!locale)		/* Request for locale category string. */
				  || (!*locale)	/* Implementation-defined default is C. */
				  || ((*locale == 'C') && !locale[1])
				  || (!strcmp(locale, "POSIX"))) )
		? (char *) C_string		/* Always in C/POSIX locale. */
		: NULL;
}

#else  /* ---------------------------------------------- __LOCALE_C_ONLY */

#if !defined(NUM_LOCALES) || (NUM_LOCALES <= 1)
#error locales enabled, but not data other than for C locale!
#endif

static unsigned char setlocale_buf[LOCALE_STRING_SIZE];


#define LOCALE_NAMES			(__locale_mmap->locale_names5)
#define LOCALES					(__locale_mmap->locales)
#define LOCALE_AT_MODIFIERS 	(__locale_mmap->locale_at_modifiers)
#define CATEGORY_NAMES			(__locale_mmap->lc_names)

static const char posix[] = "POSIX";

static int find_locale(int category, const char *p, unsigned char *new_locale)
{
	int i;
	const unsigned char *s;
	uint16_t n;
	unsigned char lang_cult, codeset;

#if defined(LOCALE_AT_MODIFIERS_LENGTH) && 1
	/* Support standard locale handling for @-modifiers. */
	char buf[18];	/* TODO: 7+{max codeset name length} */
	const char *q;

	if ((q = strchr(p,'@')) != NULL) {
		if ((((size_t)((q-p)-5)) > (sizeof(buf) - 5)) || (p[2] != '_')) {
			return 0;
		}
		/* locale name at least 5 chars long and 3rd char is '_' */
		s = LOCALE_AT_MODIFIERS;
		do {
			if (!strcmp(s+2, q+1)) {
				break;
			}
			s += 2 + *s;		/* TODO - fix this throughout */
		} while (*s);
		if (!*s) {
			return 0;
		}
		memcpy(buf, p, q-p);
		buf[q-p] = 0;
		buf[2] = s[1];
		p = buf;
	}
#endif

	lang_cult = codeset = 0;	/* Assume C and default codeset.  */
	if (((*p == 'C') && !p[1]) || !strcmp(p, posix)) {
		goto FIND_LOCALE;
	}

	if (p[5] == '.') {		/* Codeset specified in locale name? */
		/* TODO: maybe CODESET_LIST + *s ??? */
		/* 7bit is 1, UTF-8 is 2, 8-bit is >= 3 */
		codeset = 2;
		if (strcmp("UTF-8",p+6) != 0) {/* TODO - fix! */
			s = CODESET_LIST;
			do {
				++codeset;		/* Increment codeset first. */
				if (!strcmp(CODESET_LIST+*s, p+6)) {
					goto FIND_LANG_CULT;
				}
			} while (*++s);
			return 0;			/* No matching codeset! */
		}
	}

 FIND_LANG_CULT:				/* Find language_culture number. */
	s = LOCALE_NAMES;
	do {						/* TODO -- do a binary search? */
		/* TODO -- fix gen_mmap!*/
		++lang_cult;			/* Increment first since C/POSIX is 0. */
		if (!strncmp(s,p,5)) { /* Found a matching locale name; */
			goto FIND_LOCALE;
		}
		s += 5;
	} while (lang_cult < NUM_LOCALE_NAMES);
	return 0;					/* No matching language_culture! */

 FIND_LOCALE:					/* Find locale row matching name and codeset */
	s = LOCALES;
	n = 1;
	do {						/* TODO -- do a binary search? */
		if ((lang_cult == *s) && ((codeset == s[1]) || (codeset == s[2]))) {
			i = ((category == LC_ALL) ? 0 : category);
			s = new_locale + 2*i;
			do {
				/* Encode current locale row number. */
				*((unsigned char *) ++s) = (n >> 8) | 0x80;
				*((unsigned char *) ++s) = n & 0xff;
			} while (++i < category);

			return i;			/* Return non-zero */
		}
		s += WIDTH_LOCALES;
		++n;
	} while (n <= NUM_LOCALES);	/* We started at 1!!! */

	return 0;					/* Unsupported locale. */
}

char *setlocale(int category, const char *locale)
{
	const unsigned char *p;
	unsigned char *s;
	int i;
	unsigned lc_mask;
	unsigned char new_locale[LOCALE_STRING_SIZE];

	if (((unsigned int)(category)) > LC_ALL) {
		/* TODO - set errno?  SUSv3 doesn't say too. */
		return NULL;			/* Illegal/unsupported category. */
	}

	lc_mask = 1 << category;
	if (category == LC_ALL) {
		--lc_mask;
	}

	if (!locale) {				/* Request for locale category string... */
	DONE:
		strcpy(setlocale_buf, CUR_LOCALE_SPEC);
#ifdef __LOCALE_STRICTER_SETLOCALE
		/* The standard says you can only use the string returned to restore
		 * the category (categories) requested.  This could be optional.
		 * See below as well. */
		s = setlocale_buf + 1;
		lc_mask |= (1 << LC_ALL);
		do {
			if (!(lc_mask & 1)) {
				/* Encode non-selected locale flag. */
				s[1] = *s = 0xff;
			}
			s += 2;
		} while ((lc_mask >>= 1) > 1);
#endif /* __LOCALE_STRICTER_SETLOCALE */
		return (char *) setlocale_buf;
	}

	strcpy(new_locale, CUR_LOCALE_SPEC); /* Start with current. */

	if (!*locale) {				/* locale == "", so check environment. */
		i = ((category == LC_ALL) ? 0 : category);
		do {
			/* Note: SUSv3 doesn't define a fallback mechanism here.  So,
			 * if LC_ALL is invalid, we do _not_ continue trying the other
			 * environment vars. */
			if (!(p = getenv("LC_ALL"))) {
				if (!(p = getenv(CATEGORY_NAMES + CATEGORY_NAMES[i]))) {
					if (!(p = getenv("LANG"))) {
						p = posix;
					}
				}
			}

			/* The user set something... is it valid? */
			/* Note: Since we don't support user-supplied locales and
			 * alternate paths, we don't need to worry about special
			 * handling for suid/sgid apps. */
			if (!find_locale(i, p, new_locale)) {
				return NULL;
			}
		} while (++i < category);
	} else if (*locale == '#') { /* Previsouly returned value. */
		assert(strlen(locale) == LOCALE_STRING_SIZE - 1);

		i = ((category == LC_ALL) ? 0 : category);
		p = locale + 2*i;
		s = new_locale + 2*i;
		do {
#ifdef __LOCALE_STRICTER_SETLOCALE
			/* Only set categories that were selected in the previous
			 * return value.  Could be optional.  See above as well.
			 * NOTE: This still isn't quite right for non-LC_ALL
			 * as it only checks the category selected to set. */
			if ((*p == 0xff) && (p[1] == 0xff)) {
				return NULL;
			}
#endif /* __LOCALE_STRICTER_SETLOCALE */
			/* Note: Validate settings below. */
			*++s = *++p;
			*++s = *++p;
		} while (++i < category);
	} else if (!find_locale(category, locale, new_locale)) {
		return NULL;
	}


	/* TODO: Ok, everything checks out, so install the new locale. */
	_locale_set(new_locale);

	/* Everything ok, so make a copy in setlocale_buf and return. */
	goto DONE;
}

#endif /* __LOCALE_C_ONLY */

#endif
/**********************************************************************/
#ifdef L_localeconv

/* Note: We assume here that the compiler does the sane thing regarding
 * placement of the fields in the struct.  If necessary, we could ensure
 * this usings an array of offsets but at some size cost. */

#ifdef __LOCALE_C_ONLY

#warning localeconv is hardwired for C/POSIX locale only
link_warning(localeconv,"the 'localeconv' function is hardwired for C/POSIX locale only");

static struct lconv the_lconv;

static const char decpt[] = ".";

struct lconv *localeconv(void)
{
	register char *p = (char *)(&the_lconv);

	*((char **)p) = (char *) decpt;
	do {
		p += sizeof(char **);
		*((char **)p) = (char *) (decpt+1);
	} while (p < (char *) &the_lconv.negative_sign);

	p = (&the_lconv.int_frac_digits);
	do {
		*p = CHAR_MAX;
		++p;
	} while (p <= &the_lconv.int_n_sign_posn);

	return &the_lconv;
}

#else  /* __LOCALE_C_ONLY */

static struct lconv the_lconv;

struct lconv *localeconv(void)
{
	register char *p = (char *) &the_lconv;
	register char **q = (char **) &__global_locale.decimal_point;

	do {
		*((char **)p) = *q;
		p += sizeof(char **);
		++q;
	} while (p < &the_lconv.int_frac_digits);

	do {
		*p = **q;
		++p;
		++q;
	} while (p <= &the_lconv.int_n_sign_posn);

	return &the_lconv;
}

#endif /* __LOCALE_C_ONLY */

#endif
/**********************************************************************/
#ifdef L__locale_init

#ifndef __LOCALE_C_ONLY

#define C_LOCALE_SELECTOR "\x23\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01"
#define LOCALE_INIT_FAILED "locale init failed!\n"

#define CUR_LOCALE_SPEC			(__global_locale.cur_locale)

__locale_t __global_locale;

void _locale_init(void)
{
	/* TODO: mmap the locale file  */

	/* TODO - ??? */
	memset(CUR_LOCALE_SPEC, 0, LOCALE_STRING_SIZE);
	CUR_LOCALE_SPEC[0] = '#';

	memcpy(__global_locale.category_item_count,
		   __locale_mmap->lc_common_item_offsets_LEN,
		   LC_ALL);

	__global_locale.category_offsets[0] = offsetof(__locale_t, codeset);
	__global_locale.category_offsets[1] = offsetof(__locale_t, decimal_point);
	__global_locale.category_offsets[2] = offsetof(__locale_t, int_curr_symbol);
	__global_locale.category_offsets[3] = offsetof(__locale_t, abday_1);
/*  	__global_locale.category_offsets[4] = offsetof(__locale_t, collate???); */
	__global_locale.category_offsets[5] = offsetof(__locale_t, yesexpr);

#ifdef __CTYPE_HAS_8_BIT_LOCALES
	__global_locale.tbl8ctype
		= (const unsigned char *) &__locale_mmap->tbl8ctype;
    __global_locale.tbl8uplow
		= (const unsigned char *) &__locale_mmap->tbl8uplow;
#ifdef __WCHAR_ENABLED
	__global_locale.tbl8c2wc
		= (const uint16_t *) &__locale_mmap->tbl8c2wc;
	__global_locale.tbl8wc2c
		= (const unsigned char *) &__locale_mmap->tbl8wc2c;
	/* translit  */
#endif /* __WCHAR_ENABLED */
#endif /* __CTYPE_HAS_8_BIT_LOCALES */
#ifdef __WCHAR_ENABLED
	__global_locale.tblwctype
		= (const unsigned char *) &__locale_mmap->tblwctype;
	__global_locale.tblwuplow
		= (const unsigned char *) &__locale_mmap->tblwuplow;
	__global_locale.tblwuplow_diff
		= (const uint16_t *) &__locale_mmap->tblwuplow_diff;
	__global_locale.tblwcomb
		= (const unsigned char *) &__locale_mmap->tblwcomb;
	/* width?? */
#endif /* __WCHAR_ENABLED */

	_locale_set(C_LOCALE_SELECTOR);
}

static const char ascii[] = "ASCII";
static const char utf8[] = "UTF-8";

void _locale_set(const unsigned char *p)
{
	const char **x;
	unsigned char *s = CUR_LOCALE_SPEC + 1;
	const size_t *stp;
	const unsigned char *r;
	const uint16_t *io;
	const uint16_t *ii;
	const unsigned char *d;
	int row;					/* locale row */
	int crow;					/* category row */
	int len;
	int c;
	int i = 0;

	++p;
	do {
		if ((*p != *s) || (p[1] != s[1])) {
			row  = (((int)(*p & 0x7f)) << 8) + p[1] - 1;
#ifndef NDEBUG
			assert(row < NUM_LOCALES);
#endif
			*s = *p;
			s[1] = p[1];

			if (i == LC_CTYPE) {
				c = __locale_mmap->locales[ WIDTH_LOCALES * row + 2 ]; /* codeset */
				if (c <= 2) {
					if (c == 2) {
						__global_locale.codeset = utf8;
						__global_locale.encoding = __ctype_encoding_utf8;
						/* TODO - fix for bcc */
						__global_locale.mb_cur_max = 6;
					} else {
						assert(c==1);
						__global_locale.codeset = ascii;
						__global_locale.encoding = __ctype_encoding_7_bit;
						__global_locale.mb_cur_max = 1;
					}
				} else {
					const codeset_8_bit_t *c8b;
					r = CODESET_LIST;
					__global_locale.codeset = r + r[c -= 3];
					__global_locale.encoding = __ctype_encoding_8_bit;
#warning REMINDER: update 8 bit mb_cur_max when trasnlit implemented!
					/* TODO - update when translit implemented! */
					__global_locale.mb_cur_max = 1;
					c8b = __locale_mmap->codeset_8_bit + c;
#ifdef __CTYPE_HAS_8_BIT_LOCALES
					__global_locale.idx8ctype = c8b->idx8ctype;
					__global_locale.idx8uplow = c8b->idx8uplow;
#ifdef __WCHAR_ENABLED
					__global_locale.idx8c2wc = c8b->idx8c2wc;
					__global_locale.idx8wc2c = c8b->idx8wc2c;
					/* translit  */
#endif /* __WCHAR_ENABLED */
#endif /* __CTYPE_HAS_8_BIT_LOCALES */
				}

			} else if ((len = __locale_mmap->lc_common_item_offsets_LEN[i]) != 0) {
				crow = __locale_mmap->locales[ WIDTH_LOCALES * row + 3 + i ]
					* len;
				x = (const char **)(((char *) &__global_locale)
									+ __global_locale.category_offsets[i]);
				stp = __locale_mmap->lc_common_tbl_offsets + 4*i;
				r = (const unsigned char *)( ((char *)__locale_mmap) + *stp );
				io = (const uint16_t *)( ((char *)__locale_mmap) + *++stp );
				ii = (const uint16_t *)( ((char *)__locale_mmap) + *++stp );
				d = (const unsigned char *)( ((char *)__locale_mmap) + *++stp );
				for (c=0 ; c < len ; c++) {
					*(x + c) = d + ii[ r[crow + c] + io[c] ];
				}
			}

		}
		++i;
		p += 2;
		s += 2;
	} while (i < LC_ALL);
}

#endif /* __LOCALE_C_ONLY */

#endif
/**********************************************************************/
#ifdef L_nl_langinfo

#ifndef __LOCALE_C_ONLY

#include <langinfo.h>
#include <nl_types.h>

static const char empty[] = "";

char *nl_langinfo(nl_item item)
{
	unsigned int c = _NL_ITEM_CATEGORY(item);
	unsigned int i = _NL_ITEM_INDEX(item);

	if ((c < LC_ALL) && (i < __global_locale.category_item_count[c])) {
		return ((char **)(((char *) &__global_locale)
						  + __global_locale.category_offsets[c]))[i];

	}
	return (char *) empty;
}

#endif /* __LOCALE_C_ONLY */

#endif
/**********************************************************************/
