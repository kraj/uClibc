/*
 *
 * Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>,
 *                     The Silver Hammer Group, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 */
#define __FORCE_GLIBC
#include <features.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

unsigned int if_nametoindex(const char* blub) {
	struct ifreq ifr;
	int fd;
	char *tmp;
	int len=sizeof(ifr.ifr_name);

#ifdef __UCLIBC_HAS_IPV6__
	fd=socket(AF_INET6,SOCK_DGRAM,0);
	if (fd<0)
#endif /* __UCLIBC_HAS_IPV6__ */
		fd=socket(AF_INET,SOCK_DGRAM,0);

	for (tmp=ifr.ifr_name; len>0; --len) {
		if ((*tmp++ = *blub++)==0) break;
	}

	if (ioctl(fd,SIOCGIFINDEX,&ifr)==0) {
		close(fd);
		return ifr.ifr_ifindex;
	}
	close(fd);
	return 0;
}
