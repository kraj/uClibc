#ifndef __ARPA_INET_H
#define __ARPA_INET_H

#include <netinet/in.h>

int inet_aton(const char *cp, struct in_addr *inp);
      
unsigned long int inet_addr(const char *cp);

char *inet_ntoa(struct in_addr in);
       
#endif
