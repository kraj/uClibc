/* localeconv.c
 *
 * Written by Erik Andersen <andersee@debian.org> 
 *
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Library General Public License as 
 * published by the Free Software Foundation; either version 2 of the 
 * License, or (at your option) any later version.  
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * Library General Public License for more details.  
 *
 * You should have received a copy of the GNU Library General Public 
 * License along with this library; see the file COPYING.LIB.  If not, 
 * write to the Free Software Foundation, Inc., 675 Mass Ave, 
 * Cambridge, MA 02139, USA.  */

#include <string.h>
#include <locale.h>

/* Return monetary and numeric information about the current locale.  */
struct lconv * localeconv __P ((void))
{
  static struct lconv result;
  static char *blank = "";
  static char *decimal = ".";
  char junk = '\177';

  result.decimal_point = decimal;
  result.thousands_sep = blank;
  result.grouping = "\177";
  result.int_curr_symbol = blank;
  result.currency_symbol = blank;
  result.mon_decimal_point = blank;
  result.mon_thousands_sep = blank;
  result.mon_grouping = blank;
  result.positive_sign = blank;
  result.negative_sign = blank;
  result.int_frac_digits = junk;
  result.frac_digits = junk;
  result.p_cs_precedes = junk;
  result.p_sep_by_space = junk;
  result.n_cs_precedes = junk;
  result.n_sep_by_space = junk;
  result.p_sign_posn = junk;
  result.n_sign_posn = junk;

  return &result;
}

