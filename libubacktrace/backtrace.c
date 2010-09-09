/*
 * Perform stack unwinding by using the _Unwind_Backtrace.
 *
 * User application that wants to use backtrace needs to be
 * compiled with -fexceptions option and -rdynamic to get full
 * symbols printed.

 * Copyright (C) 2010 STMicroelectronics Ltd
 * Author(s): Carmelo Amoroso <carmelo.amoroso@st.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 */
#error "Arch specific implementation must be provided to properly work"
int backtrace (void **array, int size)
{
	return -1;
}

