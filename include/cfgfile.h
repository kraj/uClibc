#ifndef __CFGFILE_H__
#define __CFGFILE_H__

#include <stdio.h>

__BEGIN_DECLS
char ** cfgread(FILE *fp);
char ** cfgfind(FILE *fp, char *var);
__END_DECLS

#endif /* __CFGFILE_H__ */
