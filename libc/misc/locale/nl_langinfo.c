/* User interface for extracting locale-dependent parameters.
   Copyright (C) 1995, 1996, 1997, 1999, 2000 Free Software Foundation, Inc.
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


#include <stddef.h>
#include <time.h>
#include <langinfo.h>
#include <limits.h>
#include <errno.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/types.h>

/* Structure describing locale data in core for a category.  */
struct locale_data
{
  const char *name;
  const char *filedata;		/* Region mapping the file data.  */
  off_t filesize;		/* Size of the file (and the region).  */
  int mmaped;			/* If nonzero the data is mmaped.  */

  unsigned int usage_count;	/* Counter for users.  */

  unsigned int nstrings;	/* Number of strings below.  */
  union locale_data_value
  {
    const wchar_t *wstr;
    const char *string;
    unsigned int word;
  }
  values[0];	/* Items, usually pointers into `filedata'.  */
};


/* For each category declare two external variables (with weak references):
 * extern const struct locale_data *_nl_current_CATEGORY;
 * This points to the current locale's in-core data for CATEGORY.
 * extern const struct locale_data _nl_C_CATEGORY;
 * This contains the built-in "C"/"POSIX" locale's data for CATEGORY.
 * Both are weak references; if &_nl_current_CATEGORY is zero,
 * then nothing is using the locale data.  */
#define DEFINE_CATEGORY(category, category_name, items, a, b, c, d) \
extern struct locale_data *_nl_current_##category;                            \
extern struct locale_data _nl_C_##category;
#include "categories.c"
#undef  DEFINE_CATEGORY



/* Array indexed by category of pointers to _nl_C_CATEGORY slots.
 *    Elements are zero for categories whose data is never used.  */
struct locale_data *const _nl_C[] =
{
#define DEFINE_CATEGORY(category, category_name, items, a, b, c, d) \
    [category] = &_nl_C_##category,
#include "categories.c"
#undef  DEFINE_CATEGORY
};



/* Return a string with the data for locale-dependent parameter ITEM.  */

char * nl_langinfo (nl_item item)
{
  int category = _NL_ITEM_CATEGORY (item);
  unsigned int index = _NL_ITEM_INDEX (item);
  const struct locale_data *data;

  if (category < 0 || category >= LC_ALL)
    /* Bogus category: bogus item.  */
    return (char *) "";

  data = _nl_C[category];

  if (index >= data->nstrings)
    /* Bogus index for this category: bogus item.  */
    return (char *) "";

  /* Return the string for the specified item.  */
  return (char *) data->values[index].string;
}
