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

/* Nov. 1, 2002
 *
 * Reworked setlocale() return values and locale arg processing to
 *   be more like glibc.  Applications expecting to be able to
 *   query locale settings should now work... at the cost of almost
 *   doubling the size of the setlocale object code.
 * Fixed a bug in the internal fixed-size-string locale specifier code.
 */


/*  TODO:
 *  Implement the shared mmap code so non-mmu platforms can use this.
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

#ifndef __LOCALE_C_ONLY

#define CUR_LOCALE_SPEC			(__global_locale.cur_locale)
#undef CODESET_LIST
#define CODESET_LIST			(__locale_mmap->codeset_list)

#endif /* __LOCALE_C_ONLY */

/**********************************************************************/
#ifdef L_setlocale

#ifdef __LOCALE_C_ONLY

link_warning(setlocale,"the 'setlocale' function supports only C|POSIX locales")

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

#define LOCALE_NAMES			(__locale_mmap->locale_names5)
#define LOCALES					(__locale_mmap->locales)
#define LOCALE_AT_MODIFIERS 	(__locale_mmap->locale_at_modifiers)
#define CATEGORY_NAMES			(__locale_mmap->lc_names)

static const char posix[] = "POSIX";
static const char utf8[] = "UTF-8";

#ifdef __UCLIBC_MJN3_ONLY__
#warning REMINDER: redo the MAX_LOCALE_STR stuff...
#endif
#define MAX_LOCALE_STR    256 /* TODO: Only sufficient for current case. */

static char hr_locale[MAX_LOCALE_STR];

static __inline char *human_readable_locale(int category, const unsigned char *s)
{
	const unsigned char *loc;
	char *n;
	int i;

	++s;

	if (category == LC_ALL) {
		for (i = 0 ; i < LC_ALL-1 ; i += 2) {
			if ((s[i] != s[i+2]) || (s[i+1] != s[i+3])) {
				goto SKIP;
			}
		}
		/* All categories the same, so simplify string by using a single
		 * category. */
		category = LC_CTYPE;
	}

 SKIP:
	i = (category == LC_ALL) ? 0 : category;
	s += 2*i;
	n = hr_locale;

	do {
		if ((*s != 0xff) || (s[1] != 0xff)) {
			loc = LOCALES + WIDTH_LOCALES * ((((int)(*s & 0x7f)) << 7) + (s[1] & 0x7f));
			if (category == LC_ALL) {
				n = stpcpy(n, CATEGORY_NAMES + (int) CATEGORY_NAMES[i]);
				*n++ = '=';
			}
			if (*loc == 0) {
				*n++ = 'C';
				*n = 0;
			} else {
				char at = 0;
				memcpy(n, LOCALE_NAMES + 5*((*loc)-1), 5);
				if (n[2] != '_') {
					at = n[2];
					n[2] = '_';
				}
				n += 5;
				*n++ = '.';
				if (loc[2] == 2) {
					n = stpcpy(n, utf8);
				} else if (loc[2] >= 3) {
					n = stpcpy(n, CODESET_LIST + (int)(CODESET_LIST[loc[2] - 3]));
				}
				if (at) {
					const char *q;
					*n++ = '@';
					q = LOCALE_AT_MODIFIERS;
					do {
						if (q[1] == at) {
							n = stpcpy(n, q+2);
							break;
						}
						q += 2 + *q;
					} while (*q);
				}
			}
			*n++ = ';';
		}
		s += 2;
	} while (++i < category);

	*--n = 0;					/* Remove trailing ';' and nul-terminate. */
	assert(n-hr_locale < MAX_LOCALE_STR);
	return hr_locale;
}

static int find_locale(int category, const char *p, unsigned char *new_locale)
{
	int i;
	const unsigned char *s;
	uint16_t n;
	unsigned char lang_cult, codeset;

#if defined(LOCALE_AT_MODIFIERS_LENGTH) && 1
	/* Support standard locale handling for @-modifiers. */

#ifdef __UCLIBC_MJN3_ONLY__
#warning REMINDER: fix buf size in find_locale
#endif
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
		assert(q - p < sizeof(buf));
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

	if ((strlen(p) > 5) && (p[5] == '.')) {	/* Codeset in locale name? */
		/* TODO: maybe CODESET_LIST + *s ??? */
		/* 7bit is 1, UTF-8 is 2, 8-bit is >= 3 */
		codeset = 2;
		if (strcmp(utf8,p+6) != 0) {/* TODO - fix! */
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
	n = 0;
	do {						/* TODO -- do a binary search? */
		if ((lang_cult == *s) && ((codeset == s[1]) || (codeset == s[2]))) {
			i = ((category == LC_ALL) ? 0 : category);
			s = new_locale + 2*i;
			do {
				/* Encode current locale row number. */
				*((unsigned char *) ++s) = (n >> 7) | 0x80;
				*((unsigned char *) ++s) = (n & 0x7f) | 0x80;
			} while (++i < category);

			return i;			/* Return non-zero */
		}
		s += WIDTH_LOCALES;
		++n;
	} while (n <= NUM_LOCALES);	/* We started at 1!!! */

	return 0;					/* Unsupported locale. */
}

static unsigned char *composite_locale(int category, const char *locale, unsigned char *new_locale)
{
	char buf[MAX_LOCALE_STR];
	char *t;
	char *e;
	int c;

	if (!strchr(locale,'=')) {
		if (!find_locale(category, locale, new_locale)) {
			return NULL;
		}
		return new_locale;
	}

	if (strlen(locale) >= sizeof(buf)) {
		return NULL;
	}
	stpcpy(buf, locale);

	t = strtok_r(buf, "=", &e);	/* This can't fail because of strchr test above. */
	do {
		for (c = 0 ; c < LC_ALL ; c++) { /* Find the category... */
			if (!strcmp(CATEGORY_NAMES + (int) CATEGORY_NAMES[c], t)) {
				break;
			}
		}
		t = strtok_r(NULL, ";", &e);
		if ((category == LC_ALL) || (c == category)) {
			if (!t || !find_locale(c, t, new_locale)) {
				return NULL;
			}
		}
	} while ((t = strtok_r(NULL, "=", &e)) != NULL);

	return new_locale;
}

char *setlocale(int category, const char *locale)
{
	const unsigned char *p;
	int i;
	unsigned char new_locale[LOCALE_STRING_SIZE];

	if (((unsigned int)(category)) > LC_ALL) {
		/* TODO - set errno?  SUSv3 doesn't say too. */
		return NULL;			/* Illegal/unsupported category. */
	}

	if (locale != NULL) {  /* Not just a query... */
		stpcpy(new_locale, CUR_LOCALE_SPEC); /* Start with current. */

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
		} else if (!composite_locale(category, locale, new_locale)) {
			return NULL;
		}

		/* TODO: Ok, everything checks out, so install the new locale. */
		_locale_set(new_locale);
	}

	/* Either a query or a successful set, so return current locale string. */
	return human_readable_locale(category, CUR_LOCALE_SPEC);
}

#endif /* __LOCALE_C_ONLY */

#endif
/**********************************************************************/
#ifdef L_localeconv

/* Note: We assume here that the compiler does the sane thing regarding
 * placement of the fields in the struct.  If necessary, we could ensure
 * this usings an array of offsets but at some size cost. */

#ifdef __LOCALE_C_ONLY

link_warning(localeconv,"the 'localeconv' function is hardwired for C/POSIX locale only")

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

#define C_LOCALE_SELECTOR "\x23\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80"
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
/* 	__global_locale.tblwcomb */
/* 		= (const unsigned char *) &__locale_mmap->tblwcomb; */
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
			row  = (((int)(*p & 0x7f)) << 7) + (p[1] & 0x7f);
			assert(row < NUM_LOCALES);

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
#ifdef __UCLIBC_MJN3_ONLY__
#warning REMINDER: update 8 bit mb_cur_max when trasnlit implemented!
#endif
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

#include <langinfo.h>
#include <nl_types.h>

#ifdef __LOCALE_C_ONLY

/* We need to index 300 bytes of data, so you might initially think we
 * need to store the offsets in shorts.  But since the offset of the
 * 64th item is 231, we'll store "offset - 64" for all items >= 64
 * and always calculate the data offset as "offset[i] + (i & 64)".
 * This allows us to pack the data offsets in an unsigned char while
 * also avoiding an "if".
 *
 * Note: Category order is assumed to be:
 *   ctype, numeric, monetary, time, collate, messages, all
 */

#define C_LC_ALL 6

/* Combine the data to avoid size penalty for seperate char arrays when
 * compiler aligns objects.  The original code is left in as documentation. */
#define cat_start nl_data
#define C_locale_data (nl_data + C_LC_ALL + 1 + 78)

static const unsigned char nl_data[C_LC_ALL + 1 + 78 + 300] = {
/*  static const unsigned char cat_start[C_LC_ALL + 1] = { */
	'\x00', '\x01', '\x04', '\x1a', '\x4c', '\x4c', '\x4e', 
/*  }; */
/*  static const unsigned char item_offset[78] = { */
	'\x00', '\x06', '\x07', '\x07', '\x07', '\x07', '\x07', '\x07', 
	'\x07', '\x07', '\x07', '\x08', '\x08', '\x08', '\x08', '\x08', 
	'\x08', '\x08', '\x08', '\x08', '\x08', '\x08', '\x08', '\x08', 
	'\x08', '\x0a', '\x0c', '\x10', '\x14', '\x18', '\x1c', '\x20', 
	'\x24', '\x28', '\x2f', '\x36', '\x3e', '\x48', '\x51', '\x58', 
	'\x61', '\x65', '\x69', '\x6d', '\x71', '\x75', '\x79', '\x7d', 
	'\x81', '\x85', '\x89', '\x8d', '\x91', '\x99', '\xa2', '\xa8', 
	'\xae', '\xb2', '\xb7', '\xbc', '\xc3', '\xcd', '\xd5', '\xde', 
	'\xa7', '\xaa', '\xad', '\xc2', '\xcb', '\xd4', '\xdf', '\xdf', 
	'\xdf', '\xdf', '\xdf', '\xdf', '\xe0', '\xe6', 
/*  }; */
/*  static const unsigned char C_locale_data[300] = { */
	   'A',    'S',    'C',    'I',    'I', '\x00',    '.', '\x00', 
	'\x7f', '\x00',    '-', '\x00',    'S',    'u',    'n', '\x00', 
	   'M',    'o',    'n', '\x00',    'T',    'u',    'e', '\x00', 
	   'W',    'e',    'd', '\x00',    'T',    'h',    'u', '\x00', 
	   'F',    'r',    'i', '\x00',    'S',    'a',    't', '\x00', 
	   'S',    'u',    'n',    'd',    'a',    'y', '\x00',    'M', 
	   'o',    'n',    'd',    'a',    'y', '\x00',    'T',    'u', 
	   'e',    's',    'd',    'a',    'y', '\x00',    'W',    'e', 
	   'd',    'n',    'e',    's',    'd',    'a',    'y', '\x00', 
	   'T',    'h',    'u',    'r',    's',    'd',    'a',    'y', 
	'\x00',    'F',    'r',    'i',    'd',    'a',    'y', '\x00', 
	   'S',    'a',    't',    'u',    'r',    'd',    'a',    'y', 
	'\x00',    'J',    'a',    'n', '\x00',    'F',    'e',    'b', 
	'\x00',    'M',    'a',    'r', '\x00',    'A',    'p',    'r', 
	'\x00',    'M',    'a',    'y', '\x00',    'J',    'u',    'n', 
	'\x00',    'J',    'u',    'l', '\x00',    'A',    'u',    'g', 
	'\x00',    'S',    'e',    'p', '\x00',    'O',    'c',    't', 
	'\x00',    'N',    'o',    'v', '\x00',    'D',    'e',    'c', 
	'\x00',    'J',    'a',    'n',    'u',    'a',    'r',    'y', 
	'\x00',    'F',    'e',    'b',    'r',    'u',    'a',    'r', 
	   'y', '\x00',    'M',    'a',    'r',    'c',    'h', '\x00', 
	   'A',    'p',    'r',    'i',    'l', '\x00',    'M',    'a', 
	   'y', '\x00',    'J',    'u',    'n',    'e', '\x00',    'J', 
	   'u',    'l',    'y', '\x00',    'A',    'u',    'g',    'u', 
	   's',    't', '\x00',    'S',    'e',    'p',    't',    'e', 
	   'm',    'b',    'e',    'r', '\x00',    'O',    'c',    't', 
	   'o',    'b',    'e',    'r', '\x00',    'N',    'o',    'v', 
	   'e',    'm',    'b',    'e',    'r', '\x00',    'D',    'e', 
	   'c',    'e',    'm',    'b',    'e',    'r', '\x00',    'A', 
	   'M', '\x00',    'P',    'M', '\x00',    '%',    'a',    ' ', 
	   '%',    'b',    ' ',    '%',    'e',    ' ',    '%',    'H', 
	   ':',    '%',    'M',    ':',    '%',    'S',    ' ',    '%', 
	   'Y', '\x00',    '%',    'm',    '/',    '%',    'd',    '/', 
	   '%',    'y', '\x00',    '%',    'H',    ':',    '%',    'M', 
	   ':',    '%',    'S', '\x00',    '%',    'I',    ':',    '%', 
	   'M',    ':',    '%',    'S',    ' ',    '%',    'p', '\x00', 
	   '^',    '[',    'y',    'Y',    ']', '\x00',    '^',    '[', 
	   'n',    'N',    ']', '\x00', 
};

char *nl_langinfo(nl_item item)
{
	unsigned int c;
	unsigned int i;

	if ((c = _NL_ITEM_CATEGORY(item)) < C_LC_ALL) {
		if ((i = cat_start[c] + _NL_ITEM_INDEX(item)) < cat_start[c+1]) {
/*  			return (char *) C_locale_data + item_offset[i] + (i & 64); */
			return (char *) C_locale_data + nl_data[C_LC_ALL+1+i] + (i & 64);
		}
	}
	return (char *) cat_start;	/* Conveniently, this is the empty string. */
}

#else  /* __LOCALE_C_ONLY */

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
