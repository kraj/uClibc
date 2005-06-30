/* 
 * Environment variable to be removed for SUID programs.  The names are all
 * stuffed in a single string which means they have to be terminated with a
 * '\0' explicitly.
 */

#define UNSECURE_ENVVARS \
	"LD_AOUT_PRELOAD\0" \
	"LD_AOUT_LIBRARY_PATH\0" \
	"LD_PRELOAD\0" \
	"LD_LIBRARY_PATH\0" \
	"LD_DEBUG\0" \
	"LD_DEBUG_OUTPUT\0" \
	"LD_TRACE_LOADED_OBJECTS\0" \
	"HOSTALIASES\0" \
	"LOCALDOMAIN\0" \
	"RES_OPTIONS\0" \
	"TMPDIR\0"

/* 
 * These environment variables are defined by glibc but ignored in
 * uClibc, but may very well have an equivalent in uClibc.
 *
 * MALLOC_TRACE, RESOLV_HOST_CONF, TZDIR, GCONV_PATH, LD_USE_LOAD_BIAS,
 * LD_PROFILE, LD_ORIGIN_PATH, LOCPATH, NLSPATH
 */
