/* setlocale.c
 * Load LC_CTYPE and LC_COLLATE locale only special for uclibc
 *
 * Written by Vladimir Oleynik (c) vodz@usa.net
 *
 * This file is part of the uClibc C library and is distributed
 * under the GNU Library General Public License.
 * used ideas is part of the GNU C Library.
 */

#include <locale.h>
#include <stdio.h>      /* NULL, fopen */
#include <stdlib.h>     /* malloc */
#include <string.h>
#include <limits.h>     /* PATH_MAX */
#include <errno.h>      /* EINVAL */
#include <unistd.h>     /* get(e)[u|g]id */

#include "_locale.h"

static char C_LOCALE_NAME    []="C";
static char POSIX_LOCALE_NAME[]="POSIX";
static char composite_name_C []=
"LC_CTYPE=C;LC_NUMERIC=C;LC_TIME=C;LC_COLLATE=C;LC_MONETARY=C;LC_MESSAGES=C";

#ifdef __UCLIBC_HAS_LOCALE__

#ifdef TEST_LOCALE
static const char PATH_LOCALE[]="./";
#else
static const char PATH_LOCALE[]=__UCLIBC_LOCALE_DIR;
#endif

struct SAV_LOADED_LOCALE {
	int category;
	char *locale;
	const unsigned char *buf;
	struct SAV_LOADED_LOCALE *next;
};

static struct SAV_LOADED_LOCALE sll_C_LC_MESSAGES = {
  LC_MESSAGES, C_LOCALE_NAME,  0, 0
};

static struct SAV_LOADED_LOCALE sll_C_LC_MONETARY = {
  LC_MONETARY, C_LOCALE_NAME, 0, &sll_C_LC_MESSAGES
};

static struct SAV_LOADED_LOCALE sll_C_LC_COLLATE = {
  LC_COLLATE,  C_LOCALE_NAME, 0, &sll_C_LC_MONETARY
};

static struct SAV_LOADED_LOCALE sll_C_LC_TIME = {
  LC_TIME,     C_LOCALE_NAME, 0, &sll_C_LC_COLLATE
};

static struct SAV_LOADED_LOCALE sll_C_LC_NUMERIC = {
  LC_NUMERIC,  C_LOCALE_NAME, 0, &sll_C_LC_TIME
};

static struct SAV_LOADED_LOCALE sll_C_LC_CTYPE = {
  LC_CTYPE,    C_LOCALE_NAME, _uc_ctype_b_C, &sll_C_LC_NUMERIC
};

static struct SAV_LOADED_LOCALE *sll = &sll_C_LC_CTYPE;


#endif /* __UCLIBC_HAS_LOCALE__ */


static char *nl_current[LC_ALL+1] = {
	C_LOCALE_NAME, C_LOCALE_NAME, C_LOCALE_NAME,
	C_LOCALE_NAME, C_LOCALE_NAME, C_LOCALE_NAME,
	composite_name_C
};

static const char * const LC_strs[LC_ALL+1] = {
	"/LC_CTYPE",
	"/LC_NUMERIC",
	"/LC_TIME",
	"/LC_COLLATE",
	"/LC_MONETARY",
	"/LC_MESSAGES",
	"/LC_ALL"
};

static char *find_locale(int c, const char **plocale)
{
#ifdef __UCLIBC_HAS_LOCALE__
	struct SAV_LOADED_LOCALE *cur;
#endif
	const char *name = *plocale;

	if (name[0] == '\0') {
	/* The user decides which locale to use by setting environment
								variables. */
		name = getenv (&LC_strs[LC_ALL][1]);
		if (name == NULL || name[0] == '\0')
			name = getenv (&LC_strs[c][1]);
		if (name == NULL || name[0] == '\0')
			name = getenv ("LANG");
		if (name == NULL || name[0] == '\0')
			name = C_LOCALE_NAME;
	}

	if (strcmp (name, C_LOCALE_NAME) == 0 ||
				strcmp (name, POSIX_LOCALE_NAME) == 0 ||
		/* TODO! */     (c!=LC_CTYPE && c!=LC_COLLATE))
	    name = C_LOCALE_NAME;

	*plocale = name;

#ifdef __UCLIBC_HAS_LOCALE__
	for(cur = sll; cur; cur = cur->next)
		if(cur->category == c && strcmp(cur->locale, name)==0)
			return cur->locale;
#else
	if(name == C_LOCALE_NAME)
		return C_LOCALE_NAME;
#endif
	return NULL;
}


#ifdef __UCLIBC_HAS_LOCALE__
static char *load_locale(int category, const char *locale)
{
	FILE * fl;
	char   full_path[PATH_MAX];
	char * buf = 0;
	struct SAV_LOADED_LOCALE *cur;
	struct SAV_LOADED_LOCALE *bottom;
	int    bufsize;
	int    l = strlen(locale);

	if((l+sizeof(PATH_LOCALE)+strlen(LC_strs[category]))>=PATH_MAX)
		return NULL;

	/* Not allow acces suid/sgid binaries to outside PATH_LOCALE */
	if((geteuid()!=getuid() || getegid()!=getgid()) &&
		strchr(locale, '/')!=NULL)
			return NULL;

	strcpy(full_path, PATH_LOCALE);
	strcat(full_path, locale);
	strcat(full_path, LC_strs[category]);
	fl = fopen(full_path, "r");
	if(fl==0)
		return NULL;

	switch(category) {
		case LC_CTYPE:
			bufsize = LOCALE_BUF_SIZE;
			break;
		case LC_COLLATE:
			bufsize = 256;
			break;
		default:        /* TODO */
			bufsize = 0;
			break;
	}

	cur = malloc(sizeof(struct SAV_LOADED_LOCALE)+bufsize+l+2);
	if(cur) {
		buf = (char *)(cur+1);
		if(bufsize!=0 && fread(buf, 1, bufsize+1, fl)!=(bufsize)) {
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
	if(buf==0) {    /* broken locale file, set to "C" */
		return C_LOCALE_NAME;
	}

	cur->next     = 0;
	cur->buf      = buf;
	cur->category = category;
	cur->locale   = buf+bufsize;
	strcpy(cur->locale, locale);

	bottom = sll;
	while(bottom->next!=0)
		bottom = bottom->next;
	bottom->next = cur;

	return cur->locale;
}

static char *set_composite(int category, char *locale)
{
	int i, l;
	char *old_composite_name = nl_current[LC_ALL];
	char *new_composite_name;
	struct SAV_LOADED_LOCALE *cur;

	for(l=i=0; i<LC_ALL; i++) {
		new_composite_name = i == category ? locale : nl_current[i];
						/* '=' + ';' or '\0' */
		l += strlen(&LC_strs[i][1])+strlen(new_composite_name)+2;
	}

	new_composite_name = malloc(l);
	if(new_composite_name==NULL)
		return NULL;
	if(old_composite_name!=composite_name_C)
		free(old_composite_name);
	nl_current[category] = locale;  /* change after malloc */

	*new_composite_name = 0;
	for(i=0; i<LC_ALL; i++) {
		if(i)
			strcat(new_composite_name, ";");
		strcat(new_composite_name, &LC_strs[i][1]);
		strcat(new_composite_name, "=");
		strcat(new_composite_name, nl_current[i]);
	}
	nl_current[LC_ALL] = new_composite_name;

	/* set locale data for ctype and strcollate functions */
	for(cur = sll; ; cur = cur->next)
		if(cur->category == category && cur->locale == locale)
			break;

	switch(category) {
		case LC_CTYPE:
			_uc_ctype_b     = cur->buf;
			_uc_ctype_trans = cur->buf+LOCALE_BUF_SIZE/2;
			break;
		case LC_COLLATE:
			_uc_collate_b   = cur->buf;
			break;
		default:        /* TODO */
			break;
	}
	return locale;
}

#endif /* __UCLIBC_HAS_LOCALE__ */

char *setlocale(int category, const char *locale)
{
	char * tl;
#ifdef __UCLIBC_HAS_LOCALE__
	int    i;
#endif

	if (category < 0 || category > LC_ALL) {
#ifdef __UCLIBC_HAS_LOCALE__
einval:
#endif
		errno = EINVAL;
		return NULL;
	}

	if(locale==NULL)
		return nl_current[category];

	if(category!=LC_ALL) {
		tl = find_locale(category, &locale);
#ifdef __UCLIBC_HAS_LOCALE__
		if(tl==NULL)
			tl = load_locale(category, locale);
		if(tl) {
			if(nl_current[category] != tl)
				tl = set_composite(category, tl);
		}
#endif
		return tl;
	}
	/* LC_ALL */
#ifdef __UCLIBC_HAS_LOCALE__
	/* The user wants to set all categories.  The desired locales
	 for the individual categories can be selected by using a
	 composite locale name.  This is a semi-colon separated list
	 of entries of the form `CATEGORY=VALUE'.  */
	tl = strchr(locale, ';');
	if(tl==NULL) {
	/* This is not a composite name. Load the data for each category. */
		for(i=0; i<LC_ALL; i++)
			setlocale(i, locale);   /* recursive */
	} else {
	/* This is a composite name.  Make a copy and split it up. */
	  const char *newnames[LC_ALL];
	  char *np;
	  char *cp;

	  i  = strlen(locale);
	  np = alloca (i);
	  if(np)
		strcpy(np, locale);
	  else
		return NULL;
	  for (i = 0; i < LC_ALL; ++i)
		newnames[i] = 0;

	  while ((cp = strchr (np, '=')) != NULL) {
	      for (i = 0; i < LC_ALL; ++i)
		if ((size_t) (cp - np) == strlen(&LC_strs[i][1])
		    && memcmp (np, &LC_strs[i][1], (cp - np)) == 0)
		  break;

	      if (i == LC_ALL)
		/* Bogus category name.  */
		goto einval;

	      /* Found the category this clause sets.  */
	      newnames[i] = ++cp;
	      cp = strchr (cp, ';');
	      if (cp != NULL) {
		  /* Examine the next clause.  */
		  *cp = '\0';
		  np = cp + 1;
	      } else
		/* This was the last clause.  We are done.  */
		break;
	  }

	  for (i = 0; i < LC_ALL; ++i)
		setlocale(i, newnames[i]);   /* recursive */
	}

#endif
	return nl_current[LC_ALL];
}
