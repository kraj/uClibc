/* setlocale.c
 * Load LC_CTYPE locale only special for uclibc
 *
 * Written by Vladimir Oleynik (c) vodz@usa.net
 *
 * This file is part of the uClibc C library and is distributed
 * under the GNU Library General Public License.
 * used ideas is part of the GNU C Library.
 */

/*
 * No-locale-support setlocale() added.
 */

#include <locale.h>
#include <stdio.h>      /* NULL, fopen */
#include <stdlib.h>     /* malloc */
#include <string.h>
#include <limits.h>     /* PATH_MAX */

#include "../ctype/ctype.h"

#undef TEST_LOCALE


#ifdef L_setlocale

#ifdef __UCLIBC_HAS_LOCALE__

static char C_LOCALE_NAME[]="C";

#ifdef TEST_LOCALE
static const char PATH_LOCALE[]="./";
#else
static const char PATH_LOCALE[]=__UCLIBC_LOCALE_DIR;
#endif

static const char LC_CTYPE_STR[]="/LC_CTYPE";

struct SAV_LOADED_LOCALE {
	char *locale;
	const unsigned char *buf;
	struct SAV_LOADED_LOCALE *next;
};


static struct SAV_LOADED_LOCALE sav_loaded_locale [1] = {
	{ C_LOCALE_NAME, _uc_ctype_b_C, 0 }
};

static struct SAV_LOADED_LOCALE * old_locale = sav_loaded_locale;

static char *set_new_locale(struct SAV_LOADED_LOCALE * s_locale)
{
	_uc_ctype_b     = s_locale->buf;
	_uc_ctype_trans = s_locale->buf+LOCALE_BUF_SIZE/2;
	old_locale      = s_locale;
	return            s_locale->locale;
}

/* Current support only LC_CTYPE or LC_ALL category */

char *setlocale(int category, const char *locale)
{
	FILE * fl;
	struct SAV_LOADED_LOCALE *cur;
	struct SAV_LOADED_LOCALE *bottom;
	char   full_path[PATH_MAX];
	char * buf = 0;
	int    l;

	if(category!=LC_CTYPE && category!=LC_ALL)
		return NULL;

	if(locale==0)
		return set_new_locale(old_locale);

	if(strcmp(locale, "POSIX")==0)
		return set_new_locale(sav_loaded_locale);
	 else if(*locale == '\0') {

		locale = getenv(LC_CTYPE_STR+1);
		if(locale == 0 || *locale == 0)
			locale = getenv("LANG");
		if(locale == 0 || *locale == '\0')
			return set_new_locale(old_locale);
		if(strcmp(locale, "POSIX")==0)
			return set_new_locale(sav_loaded_locale);
	}

	for(cur = sav_loaded_locale; cur; cur = cur->next)
		if(strcmp(cur->locale, locale)==0)
			return set_new_locale(cur);

	l = strlen(locale);
	if((l+sizeof(PATH_LOCALE)+sizeof(LC_CTYPE_STR))>=PATH_MAX)
		return NULL;

	strcpy(full_path, PATH_LOCALE);
	strcat(full_path, locale);
	strcat(full_path, LC_CTYPE_STR);
	fl = fopen(full_path, "r");
	if(fl==0)
		return NULL;

	cur = malloc(sizeof(struct SAV_LOADED_LOCALE)+LOCALE_BUF_SIZE+l);
	if(cur) {
		buf = (char *)(cur+1);
		if(fread(buf, 1, LOCALE_BUF_SIZE+1, fl)!=(LOCALE_BUF_SIZE)) {
			/* broken locale file */
			free(cur);
			buf = 0;
#ifdef TEST_LOCALE
			fprintf(stderr, "\nbroken locale file\n");
#endif
		}
	}

	fclose(fl);
	if(cur==0)      /* not enough memory */
		return NULL;
	if(buf==0)      /* broken locale file, set to "C" */
		return set_new_locale(sav_loaded_locale);

	cur->next = 0;
	strcpy(buf+LOCALE_BUF_SIZE, locale);

	bottom = sav_loaded_locale;
	while(bottom->next!=0)
		bottom = bottom->next;
	bottom->next = cur;

	/* next two line only pedantic */
	cur->buf    = buf;
	cur->locale = buf+LOCALE_BUF_SIZE;

	return set_new_locale(cur);
}

#else /* no locale support */

char *setlocale(int category, const char *locale)
{
	/* Allow locales "C" and "" (native).  Both are "C" for our purposes. */
	if (locale) {
		if (*locale == 'C') {
			++locale;
		}
		if (*locale) {				/* locale wasn't "C" or ""!! */
			return NULL;
		}
	}

	/* Allow any legal category for "C" or "" (native) locale. */
	if((category < LC_CTYPE) || (category > LC_ALL)) { /* Illegal category! */
		return NULL;
	}

	return "C";
}

#endif

#endif /* L_setlocale */
