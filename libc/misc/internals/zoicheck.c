
/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * This is a utility routine for strtod errno support.
 * As the name implies, it checks if a double is either 0 or +/-infinity.
 * Doing this inline doesn't work on i386 because of excess precission
 * stored in the FPU.
 *
 * TODO: Check bitmasks directly?
 */

int _zero_or_inf_check(double x)
{
	return ( x == x/4 );
}
