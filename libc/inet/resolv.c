/* resolv.c: DNS Resolver
 *
 * Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>,
 *                     The Silver Hammer Group, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 *  5-Oct-2000 W. Greathouse  wgreathouse@smva.com
 *                              Fix memory leak and memory corruption.
 *                              -- Every name resolution resulted in
 *                                 a new parse of resolv.conf and new
 *                                 copy of nameservers allocated by
 *                                 strdup.
 *                              -- Every name resolution resulted in
 *                                 a new read of resolv.conf without
 *                                 resetting index from prior read...
 *                                 resulting in exceeding array bounds.
 *
 *                              Limit nameservers read from resolv.conf
 *
 *                              Add "search" domains from resolv.conf
 *
 *                              Some systems will return a security
 *                              signature along with query answer for
 *                              dynamic DNS entries.
 *                              -- skip/ignore this answer
 *
 *                              Include arpa/nameser.h for defines.
 *
 *                              General cleanup
 *
 * 20-Jun-2001 Michal Moskal <malekith@pld.org.pl>
 *   partial IPv6 support (i.e. gethostbyname2() and resolve_address2()
 *   functions added), IPv6 nameservers are also supported.
 *
 * 6-Oct-2001 Jari Korva <jari.korva@iki.fi>
 *   more IPv6 support (IPv6 support for gethostbyaddr();
 *   address family parameter and improved IPv6 support for get_hosts_byname
 *   and read_etc_hosts; getnameinfo() port from glibc; defined
 *   defined ip6addr_any and in6addr_loopback)
 *
 * 2-Feb-2002 Erik Andersen <andersee@debian.org>
 * Added gethostent(), sethostent(), and endhostent()
 *
 */

#define __FORCE_GLIBC
#include <features.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <resolv.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/nameser.h>
#include <sys/utsname.h>
#include <sys/un.h>

#define MAX_RECURSE 5
#define REPLY_TIMEOUT 10
#define MAX_RETRIES 15
#define MAX_SERVERS 3
#define MAX_SEARCH 4

#undef DEBUG
/*#define DEBUG*/

#ifdef DEBUG
#define DPRINTF(X,args...) fprintf(stderr, X, ##args)
#else
#define DPRINTF(X,args...)
#endif /* DEBUG */


struct resolv_header {
	int id;
	int qr,opcode,aa,tc,rd,ra,rcode;
	int qdcount;
	int ancount;
	int nscount;
	int arcount;
};

struct resolv_question {
	char * dotted;
	int qtype;
	int qclass;
};

struct resolv_answer {
	char * dotted;
	int atype;
	int aclass;
	int ttl;
	int rdlength;
	unsigned char * rdata;
	int rdoffset;
};

enum etc_hosts_action {
    GET_HOSTS_BYNAME = 0,
    GETHOSTENT,
    GET_HOSTS_BYADDR,
};


extern int nameservers;
extern char * nameserver[MAX_SERVERS];
extern int searchdomains;
extern char * searchdomain[MAX_SEARCH];
extern struct hostent * get_hosts_byname(const char * name, int type);
extern struct hostent * get_hosts_byaddr(const char * addr, int len, int type);
extern void __open_etc_hosts(FILE **fp);
extern struct hostent * read_etc_hosts(FILE *fp, const char * name, int type, enum etc_hosts_action action);
extern int resolve_address(const char * address, int nscount, 
	char ** nsip, struct in_addr * in);
extern int resolve_mailbox(const char * address, int nscount, 
	char ** nsip, struct in_addr * in);
extern int dns_lookup(const char * name, int type, int nscount, 
	char ** nsip, unsigned char ** outpacket, struct resolv_answer * a);

int encode_dotted(const char * dotted, unsigned char * dest, int maxlen);
int decode_dotted(const unsigned char * message, int offset, 
	char * dest, int maxlen);
int length_dotted(const unsigned char * message, int offset);
int encode_header(struct resolv_header * h, unsigned char * dest, int maxlen);
int decode_header(unsigned char * data, struct resolv_header * h);
int encode_question(struct resolv_question * q,
	unsigned char * dest, int maxlen);
int decode_question(unsigned char * message, int offset,
	struct resolv_question * q);
int encode_answer(struct resolv_answer * a,
	unsigned char * dest, int maxlen);
int decode_answer(unsigned char * message, int offset,
	struct resolv_answer * a);
int length_question(unsigned char * message, int offset);
extern int open_nameservers(void);


#ifdef L_encodeh
int encode_header(struct resolv_header *h, unsigned char *dest, int maxlen)
{
	if (maxlen < HFIXEDSZ)
		return -1;

	dest[0] = (h->id & 0xff00) >> 8;
	dest[1] = (h->id & 0x00ff) >> 0;
	dest[2] = (h->qr ? 0x80 : 0) |
		((h->opcode & 0x0f) << 3) |
		(h->aa ? 0x04 : 0) | 
		(h->tc ? 0x02 : 0) | 
		(h->rd ? 0x01 : 0);
	dest[3] = (h->ra ? 0x80 : 0) | (h->rcode & 0x0f);
	dest[4] = (h->qdcount & 0xff00) >> 8;
	dest[5] = (h->qdcount & 0x00ff) >> 0;
	dest[6] = (h->ancount & 0xff00) >> 8;
	dest[7] = (h->ancount & 0x00ff) >> 0;
	dest[8] = (h->nscount & 0xff00) >> 8;
	dest[9] = (h->nscount & 0x00ff) >> 0;
	dest[10] = (h->arcount & 0xff00) >> 8;
	dest[11] = (h->arcount & 0x00ff) >> 0;

	return HFIXEDSZ;
}
#endif

#ifdef L_decodeh
int decode_header(unsigned char *data, struct resolv_header *h)
{
	h->id = (data[0] << 8) | data[1];
	h->qr = (data[2] & 0x80) ? 1 : 0;
	h->opcode = (data[2] >> 3) & 0x0f;
	h->aa = (data[2] & 0x04) ? 1 : 0;
	h->tc = (data[2] & 0x02) ? 1 : 0;
	h->rd = (data[2] & 0x01) ? 1 : 0;
	h->ra = (data[3] & 0x80) ? 1 : 0;
	h->rcode = data[3] & 0x0f;
	h->qdcount = (data[4] << 8) | data[5];
	h->ancount = (data[6] << 8) | data[7];
	h->nscount = (data[8] << 8) | data[9];
	h->arcount = (data[10] << 8) | data[11];

	return HFIXEDSZ;
}
#endif

#ifdef L_encoded
/* Encode a dotted string into nameserver transport-level encoding.
   This routine is fairly dumb, and doesn't attempt to compress
   the data */

int encode_dotted(const char *dotted, unsigned char *dest, int maxlen)
{
	int used = 0;

	while (dotted && *dotted) {
		char *c = strchr(dotted, '.');
		int l = c ? c - dotted : strlen(dotted);

		if (l >= (maxlen - used - 1))
			return -1;

		dest[used++] = l;
		memcpy(dest + used, dotted, l);
		used += l;

		if (c)
			dotted = c + 1;
		else
			break;
	}

	if (maxlen < 1)
		return -1;

	dest[used++] = 0;

	return used;
}
#endif

#ifdef L_decoded
/* Decode a dotted string from nameserver transport-level encoding.
   This routine understands compressed data. */

int decode_dotted(const unsigned char *data, int offset,
				  char *dest, int maxlen)
{
	int l;
	int measure = 1;
	int total = 0;
	int used = 0;

	if (!data)
		return -1;

	while ((l=data[offset++])) {
		if (measure)
		    total++;
		if ((l & 0xc0) == (0xc0)) {
			if (measure)
				total++;
			/* compressed item, redirect */
			offset = ((l & 0x3f) << 8) | data[offset];
			measure = 0;
			continue;
		}

		if ((used + l + 1) >= maxlen)
			return -1;

		memcpy(dest + used, data + offset, l);
		offset += l;
		used += l;
		if (measure)
			total += l;

		if (data[offset] != 0)
			dest[used++] = '.';
		else
			dest[used++] = '\0';
	}

	DPRINTF("Total decode len = %d\n", total);

	return total;
}
#endif

#ifdef L_lengthd

int length_dotted(const unsigned char *data, int offset)
{
	int orig_offset = offset;
	int l;

	if (!data)
		return -1;

	while ((l = data[offset++])) {

		if ((l & 0xc0) == (0xc0)) {
			offset++;
			break;
		}

		offset += l;
	}

	return offset - orig_offset;
}
#endif

#ifdef L_encodeq
int encode_question(struct resolv_question *q,
					unsigned char *dest, int maxlen)
{
	int i;

	i = encode_dotted(q->dotted, dest, maxlen);
	if (i < 0)
		return i;

	dest += i;
	maxlen -= i;

	if (maxlen < 4)
		return -1;

	dest[0] = (q->qtype & 0xff00) >> 8;
	dest[1] = (q->qtype & 0x00ff) >> 0;
	dest[2] = (q->qclass & 0xff00) >> 8;
	dest[3] = (q->qclass & 0x00ff) >> 0;

	return i + 4;
}
#endif

#ifdef L_decodeq
int decode_question(unsigned char *message, int offset,
					struct resolv_question *q)
{
	char temp[256];
	int i;

	i = decode_dotted(message, offset, temp, sizeof(temp));
	if (i < 0)
		return i;

	offset += i;

	q->dotted = strdup(temp);
	q->qtype = (message[offset + 0] << 8) | message[offset + 1];
	q->qclass = (message[offset + 2] << 8) | message[offset + 3];

	return i + 4;
}
#endif

#ifdef L_lengthq
int length_question(unsigned char *message, int offset)
{
	int i;

	i = length_dotted(message, offset);
	if (i < 0)
		return i;

	return i + 4;
}
#endif

#ifdef L_encodea
int encode_answer(struct resolv_answer *a, unsigned char *dest, int maxlen)
{
	int i;

	i = encode_dotted(a->dotted, dest, maxlen);
	if (i < 0)
		return i;

	dest += i;
	maxlen -= i;

	if (maxlen < (RRFIXEDSZ+a->rdlength))
		return -1;

	*dest++ = (a->atype & 0xff00) >> 8;
	*dest++ = (a->atype & 0x00ff) >> 0;
	*dest++ = (a->aclass & 0xff00) >> 8;
	*dest++ = (a->aclass & 0x00ff) >> 0;
	*dest++ = (a->ttl & 0xff000000) >> 24;
	*dest++ = (a->ttl & 0x00ff0000) >> 16;
	*dest++ = (a->ttl & 0x0000ff00) >> 8;
	*dest++ = (a->ttl & 0x000000ff) >> 0;
	*dest++ = (a->rdlength & 0xff00) >> 8;
	*dest++ = (a->rdlength & 0x00ff) >> 0;
	memcpy(dest, a->rdata, a->rdlength);

	return i + RRFIXEDSZ + a->rdlength;
}
#endif

#ifdef L_decodea
int decode_answer(unsigned char *message, int offset,
				  struct resolv_answer *a)
{
	char temp[256];
	int i;

	i = decode_dotted(message, offset, temp, sizeof(temp));
	if (i < 0)
		return i;

	message += offset + i;

	a->dotted = strdup(temp);
	a->atype = (message[0] << 8) | message[1];
	message += 2;
	a->aclass = (message[0] << 8) | message[1];
	message += 2;
	a->ttl = (message[0] << 24) |
		(message[1] << 16) | (message[2] << 8) | (message[3] << 0);
	message += 4;
	a->rdlength = (message[0] << 8) | message[1];
	message += 2;
	a->rdata = message;
	a->rdoffset = offset + i + RRFIXEDSZ;

	DPRINTF("i=%d,rdlength=%d\n", i, a->rdlength);

	return i + RRFIXEDSZ + a->rdlength;
}
#endif

#ifdef L_encodep
int encode_packet(struct resolv_header *h,
	struct resolv_question **q,
	struct resolv_answer **an,
	struct resolv_answer **ns,
	struct resolv_answer **ar,
	unsigned char *dest, int maxlen)
{
	int i, total = 0;
	int j;

	i = encode_header(h, dest, maxlen);
	if (i < 0)
		return i;

	dest += i;
	maxlen -= i;
	total += i;

	for (j = 0; j < h->qdcount; j++) {
		i = encode_question(q[j], dest, maxlen);
		if (i < 0)
			return i;
		dest += i;
		maxlen -= i;
		total += i;
	}

	for (j = 0; j < h->ancount; j++) {
		i = encode_answer(an[j], dest, maxlen);
		if (i < 0)
			return i;
		dest += i;
		maxlen -= i;
		total += i;
	}
	for (j = 0; j < h->nscount; j++) {
		i = encode_answer(ns[j], dest, maxlen);
		if (i < 0)
			return i;
		dest += i;
		maxlen -= i;
		total += i;
	}
	for (j = 0; j < h->arcount; j++) {
		i = encode_answer(ar[j], dest, maxlen);
		if (i < 0)
			return i;
		dest += i;
		maxlen -= i;
		total += i;
	}

	return total;
}
#endif

#ifdef L_decodep
int decode_packet(unsigned char *data, struct resolv_header *h)
{
	return decode_header(data, h);
}
#endif

#ifdef L_formquery
int form_query(int id, const char *name, int type, unsigned char *packet,
			   int maxlen)
{
	struct resolv_header h;
	struct resolv_question q;
	int i, j;

	memset(&h, 0, sizeof(h));
	h.id = id;
	h.qdcount = 1;

	q.dotted = (char *) name;
	q.qtype = type;
	q.qclass = C_IN; /* CLASS_IN */

	i = encode_header(&h, packet, maxlen);
	if (i < 0)
		return i;

	j = encode_question(&q, packet + i, maxlen - i);
	if (j < 0)
		return j;

	return i + j;
}
#endif

#ifdef L_dnslookup

int dns_caught_signal = 0;
void dns_catch_signal(int signo)
{
	dns_caught_signal = 1;
}

int dns_lookup(const char *name, int type, int nscount, char **nsip,
			   unsigned char **outpacket, struct resolv_answer *a)
{
	static int id = 1;
	int i, j, len, fd, pos;
	static int ns = 0;
	struct sockaddr_in sa;
#ifdef __UCLIBC_HAS_IPV6__
	struct sockaddr_in6 sa6;
#endif /* __UCLIBC_HAS_IPV6__ */
	int oldalarm;
	__sighandler_t oldhandler;
	struct resolv_header h;
	struct resolv_question q;
	int retries = 0;
	unsigned char * packet = malloc(PACKETSZ);
	char * lookup = malloc(MAXDNAME);
	int variant = 0;
#ifdef __UCLIBC_HAS_IPV6__
	int v6;
#endif /* __UCLIBC_HAS_IPV6__ */

	fd = -1;

	if (!packet || !lookup || !nscount)
	    goto fail;

	DPRINTF("Looking up type %d answer for '%s'\n", type, name);

	ns %= nscount;

	while (retries++ < MAX_RETRIES) {
#ifdef __UCLIBC_HAS_IPV6__
		v6 = (inet_pton(AF_INET6, nsip[ns], &sa6.sin6_addr) > 0);
#endif /* __UCLIBC_HAS_IPV6__ */

		if (fd != -1)
			close(fd);

#ifndef __UCLIBC_HAS_IPV6__
		fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#else /* __UCLIBC_HAS_IPV6__ */
		fd = socket(v6 ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif /* __UCLIBC_HAS_IPV6__ */

		if (fd == -1)
			goto fail;

		memset(packet, 0, PACKETSZ);

		memset(&h, 0, sizeof(h));
		h.id = ++id;
		h.qdcount = 1;
		h.rd = 1;

		DPRINTF("encoding header\n", h.rd);

		i = encode_header(&h, packet, PACKETSZ);
		if (i < 0)
			goto fail;

		strncpy(lookup,name,MAXDNAME);
		if (variant < searchdomains && strchr(lookup, '.') == NULL)
		{
		    strncat(lookup,".", MAXDNAME);
		    strncat(lookup,searchdomain[variant], MAXDNAME);
		}
		DPRINTF("lookup name: %s\n", lookup);
		q.dotted = (char *)lookup;
		q.qtype = type;
		q.qclass = C_IN; /* CLASS_IN */

		j = encode_question(&q, packet+i, PACKETSZ-i);
		if (j < 0)
			goto fail;

		len = i + j;

		DPRINTF("On try %d, sending query to port %d of machine %s\n",
				retries, NAMESERVER_PORT, nsip[ns]);

#ifndef __UCLIBC_HAS_IPV6__
		sa.sin_family = AF_INET;
		sa.sin_port = htons(NAMESERVER_PORT);
		sa.sin_addr.s_addr = inet_addr(nsip[ns]);
#else /* __UCLIBC_HAS_IPV6__ */
		if (v6) {
			sa6.sin6_family = AF_INET6;
			sa6.sin6_port = htons(NAMESERVER_PORT);
			/* sa6.sin6_addr is already here */
		} else {
			sa.sin_family = AF_INET;
			sa.sin_port = htons(NAMESERVER_PORT);
			sa.sin_addr.s_addr = inet_addr(nsip[ns]);
		}
#endif /* __UCLIBC_HAS_IPV6__ */

#ifndef __UCLIBC_HAS_IPV6__
		if (connect(fd, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
#else /* __UCLIBC_HAS_IPV6__ */
		if (connect(fd, (struct sockaddr *) (v6 ? &sa6 : &sa), 
			    v6 ? sizeof(sa6) : sizeof(sa)) == -1) {
#endif /* __UCLIBC_HAS_IPV6__ */
			if (errno == ENETUNREACH) {
				/* routing error, presume not transient */
				goto tryall;
			} else
				/* retry */
				continue;
		}

		DPRINTF("Transmitting packet of length %d, id=%d, qr=%d\n",
				len, h.id, h.qr);

		send(fd, packet, len, 0);

		dns_caught_signal = 0;
		oldalarm = alarm(REPLY_TIMEOUT);
		oldhandler = signal(SIGALRM, dns_catch_signal);

		i = recv(fd, packet, PACKETSZ, 0);

		alarm(0);
		signal(SIGALRM, oldhandler);
		alarm(oldalarm);

		DPRINTF("Timeout=%d, len=%d\n", dns_caught_signal, i);

		if (dns_caught_signal)
			/* timed out, so retry send and receive,
			   to next nameserver on queue */
			goto again;

		if (i < HFIXEDSZ)
			/* too short ! */
			goto again;

		decode_header(packet, &h);

		DPRINTF("id = %d, qr = %d\n", h.id, h.qr);

		if ((h.id != id) || (!h.qr))
			/* unsolicited */
			goto again;

		DPRINTF("Got response %s\n", "(i think)!");
		DPRINTF("qrcount=%d,ancount=%d,nscount=%d,arcount=%d\n",
				h.qdcount, h.ancount, h.nscount, h.arcount);
		DPRINTF("opcode=%d,aa=%d,tc=%d,rd=%d,ra=%d,rcode=%d\n",
				h.opcode, h.aa, h.tc, h.rd, h.ra, h.rcode);

		if ((h.rcode) || (h.ancount < 1)) {
			/* negative result, not present */
			goto again;
		}

		pos = HFIXEDSZ;

		for (j = 0; j < h.qdcount; j++) {
			DPRINTF("Skipping question %d at %d\n", j, pos);
			i = length_question(packet, pos);
			DPRINTF("Length of question %d is %d\n", j, i);
			if (i < 0)
				goto again;
			pos += i;
		}
		DPRINTF("Decoding answer at pos %d\n", pos);

		for (j=0;j<h.ancount;j++)
		{
		    i = decode_answer(packet, pos, a);

		    if (i<0) {
			DPRINTF("failed decode %d\n", i);
			goto again;
		    }
		    /* For all but T_SIG, accept first answer */
		    if (a->atype != T_SIG)
			break;

		    DPRINTF("skipping T_SIG %d\n", i);
		    free(a->dotted);
		    pos += i;
		}

		DPRINTF("Answer name = |%s|\n", a->dotted);
		DPRINTF("Answer type = |%d|\n", a->atype);

		close(fd);

		if (outpacket)
			*outpacket = packet;
		else
			free(packet);
		free(lookup);
		return (0);				/* success! */

	  tryall:
		/* if there are other nameservers, give them a go,
		   otherwise return with error */
		variant = 0;
		if (retries >= nscount*(searchdomains+1))
		    goto fail;

	  again:
		/* if there are searchdomains, try them or fallback as passed */
		if (variant < searchdomains) {
		    /* next search */
		    variant++;
		} else {
		    /* next server, first search */
		    ns = (ns + 1) % nscount;
		    variant = 0;
		}
	}

fail:
	if (fd != -1)
	    close(fd);
	if (lookup)
	    free(lookup);
	if (packet)
	    free(packet);
	return -1;
}
#endif

#ifdef L_resolveaddress

int resolve_address(const char *address, int nscount, 
	char **nsip, struct in_addr *in)
{
	unsigned char *packet;
	struct resolv_answer a;
	char temp[256];
	int i;
	int nest = 0;

	if (!address || !in)
		return -1;

	strncpy(temp, address, sizeof(temp));

	for (;;) {

		i = dns_lookup(temp, T_A, nscount, nsip, &packet, &a);

		if (i < 0)
			return -1;

		free(a.dotted);

		if (a.atype == T_CNAME) {		/* CNAME */
			DPRINTF("Got a CNAME in resolve_address()\n");
			i = decode_dotted(packet, a.rdoffset, temp, sizeof(temp));
			free(packet);

			if (i < 0)
				return -1;
			if (++nest > MAX_RECURSE)
				return -1;
			continue;
		} else if (a.atype == T_A) {	/* ADDRESS */
			free(packet);
			break;
		} else {
			free(packet);
			return -1;
		}
	}

	if (in)
	    memcpy(in, a.rdata, INADDRSZ); /* IPv4 T_A */

	return 0;
}
#endif

#ifdef L_resolvemailbox

int resolve_mailbox(const char *address, int nscount, 
	char **nsip, struct in_addr *in)
{
	struct resolv_answer a;
	unsigned char *packet;
	char temp[256];
	int nest = 0;
	int i;

	if (!address || !in)
		return -1;

	/* look up mail exchange */
	i = dns_lookup(address, T_MX, nscount, nsip, &packet, &a);

	strncpy(temp, address, sizeof(temp));

	if (i >= 0) {
		i = decode_dotted(packet, a.rdoffset+2, temp, sizeof(temp));
		free(packet);
	}

	for (;;) {

		i = dns_lookup(temp, T_A, nscount, nsip, &packet, &a);

		if (i < 0)
			return -1;

		free(a.dotted);

		if (a.atype == T_CNAME) {		/* CNAME */
			DPRINTF("Got a CNAME in resolve_mailbox()\n");
			i = decode_dotted(packet, a.rdoffset, temp, sizeof(temp));
			free(packet);
			if (i < 0)
				return i;
			if (++nest > MAX_RECURSE)
				return -1;
			continue;
		} else if (a.atype == T_A) {	/* ADDRESS */
			free(packet);
			break;
		} else {
			free(packet);
			return -1;
		}
	}

	if (in)
		memcpy(in, a.rdata, INADDRSZ); /* IPv4 */

	return 0;
}
#endif


#ifdef L_opennameservers

int nameservers;
char * nameserver[MAX_SERVERS];
int searchdomains;
char * searchdomain[MAX_SEARCH];

/*
 *	we currently read formats not quite the same as that on normal
 *	unix systems, we can have a list of nameservers after the keyword.
 */

int open_nameservers()
{
	FILE *fp;
	int i;
#define RESOLV_ARGS 5
	char szBuffer[128], *p, *argv[RESOLV_ARGS];
	int argc;

	if (nameservers > 0) 
	    return 0;

	if ((fp = fopen("/etc/resolv.conf", "r")) ||
			(fp = fopen("/etc/config/resolv.conf", "r"))) {

		while (fgets(szBuffer, sizeof(szBuffer), fp) != NULL) {

			for (p = szBuffer; *p && isspace(*p); p++)
				/* skip white space */;
			if (*p == '\0' || *p == '\n' || *p == '#') /* skip comments etc */
				continue;
			argc = 0;
			while (*p && argc < RESOLV_ARGS) {
				argv[argc++] = p;
				while (*p && !isspace(*p) && *p != '\n')
					p++;
				while (*p && (isspace(*p) || *p == '\n')) /* remove spaces */
					*p++ = '\0';
			}

			if (strcmp(argv[0], "nameserver") == 0) {
				for (i = 1; i < argc && nameservers < MAX_SERVERS; i++) {
					nameserver[nameservers++] = strdup(argv[i]);
					DPRINTF("adding nameserver %s\n", argv[i]);
				}
			}

			/* domain and search are mutually exclusive, the last one wins */
			if (strcmp(argv[0],"domain")==0 || strcmp(argv[0],"search")==0) {
				while (searchdomains > 0) {
					free(searchdomain[--searchdomains]);
					searchdomain[searchdomains] = NULL;
				}
				for (i=1; i < argc && searchdomains < MAX_SEARCH; i++) {
					searchdomain[searchdomains++] = strdup(argv[i]);
					DPRINTF("adding search %s\n", argv[i]);
				}
			}
		}
		fclose(fp);
	} else {
	    DPRINTF("failed to open %s\n", "resolv.conf");
	}
	DPRINTF("nameservers = %d\n", nameservers);
	return 0;
}
#endif


#ifdef L_closenameservers

void close_nameservers(void)
{
	while (nameservers > 0) {
		free(nameserver[--nameservers]);
		nameserver[nameservers] = NULL;
	}
	while (searchdomains > 0) {
		free(searchdomain[--searchdomains]);
		searchdomain[searchdomains] = NULL;
	}
}
#endif


#ifdef L_resolvename

const char *resolve_name(const char *name, int mailbox)
{
	struct in_addr in;
	int i;

	/* shortcut: is it a valid IP address to begin with? */
	if (inet_aton(name, &in))
		return name;

	open_nameservers();

	DPRINTF("looking up '%s', mailbox=%d, nameservers=%d\n",
			name, mailbox, nameservers);

	if (mailbox)
		i = resolve_mailbox(name, nameservers, nameserver, &in);
	else
		i = resolve_address(name, nameservers, nameserver, &in);

	if (i < 0)
		return 0;

	DPRINTF("success = '%s'\n", inet_ntoa(in));

	return inet_ntoa(in);
}
#endif


#ifdef L_gethostbyname

struct hostent *gethostbyname(const char *name)
{
	static struct hostent h;
	static char namebuf[256];
	static struct in_addr in;
	static struct in_addr *addr_list[2];
	struct hostent *hp;
	unsigned char *packet;
	struct resolv_answer a;
	int i;
	int nest = 0;

	open_nameservers();

	if (!name)
		return 0;

	if ((hp = get_hosts_byname(name, AF_INET))) /* do /etc/hosts first */
		return(hp);

	memset(&h, 0, sizeof(h));

	addr_list[0] = &in;
	addr_list[1] = 0;
	
	strncpy(namebuf, name, sizeof(namebuf));

	/* First check if this is already an address */
	if (inet_aton(name, &in)) {
	    h.h_name = namebuf;
	    h.h_addrtype = AF_INET;
	    h.h_length = sizeof(in);
	    h.h_addr_list = (char **) addr_list;
	    return &h;
	}

	for (;;) {

		i = dns_lookup(namebuf, 1, nameservers, nameserver, &packet, &a);

		if (i < 0)
			return 0;

		strncpy(namebuf, a.dotted, sizeof(namebuf));
		free(a.dotted);


		if (a.atype == T_CNAME) {		/* CNAME */
			DPRINTF("Got a CNAME in gethostbyname()\n");
			i = decode_dotted(packet, a.rdoffset, namebuf, sizeof(namebuf));
			free(packet);

			if (i < 0)
				return 0;
			if (++nest > MAX_RECURSE)
				return 0;
			continue;
		} else if (a.atype == T_A) {	/* ADDRESS */
			memcpy(&in, a.rdata, sizeof(in));
			h.h_name = namebuf;
			h.h_addrtype = AF_INET;
			h.h_length = sizeof(in);
			h.h_addr_list = (char **) addr_list;
			free(packet);
			break;
		} else {
			free(packet);
			return 0;
		}
	}

	return &h;
}
#endif

#ifdef L_gethostbyname2

#ifdef __UCLIBC_HAS_IPV6__
/* TBD: Not the right place for defining these, I guess */
const struct in6_addr in6addr_any =
	{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } };
const struct in6_addr in6addr_loopback =
	{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } };                                    
#endif /* __UCLIBC_HAS_IPV6__ */

struct hostent *gethostbyname2(const char *name, int family)
{
#ifndef __UCLIBC_HAS_IPV6__
	return family == AF_INET ? gethostbyname(name) : (struct hostent*)0;
#else /* __UCLIBC_HAS_IPV6__ */
	static struct hostent h;
	static char namebuf[256];
	static struct in6_addr in;
	static struct in6_addr *addr_list[2];
	struct hostent *hp;
	unsigned char *packet;
	struct resolv_answer a;
	int i;
	int nest = 0;

	if (family == AF_INET)
		return gethostbyname(name);
		
	if (family != AF_INET6)
		return NULL;
		
	open_nameservers();

	if (!name)
		return 0;

	if ((hp = get_hosts_byname(name, family))) /* do /etc/hosts first */
		return(hp);

	memset(&h, 0, sizeof(h));

	addr_list[0] = &in;
	addr_list[1] = 0;
	
	strncpy(namebuf, name, sizeof(namebuf));

	/* First check if this is already an address */
	if (inet_pton(AF_INET6, name, &in)) {
	    h.h_name = namebuf;
	    h.h_addrtype = AF_INET6;
	    h.h_length = sizeof(in);
	    h.h_addr_list = (char **) addr_list;
	    return &h;
	}

	for (;;) {

		i = dns_lookup(namebuf, T_AAAA, nameservers, nameserver, &packet, &a);

		if (i < 0)
			return 0;

		strncpy(namebuf, a.dotted, sizeof(namebuf));
		free(a.dotted);


		if (a.atype == T_CNAME) {		/* CNAME */
			DPRINTF("Got a CNAME in gethostbyname()\n");
			i = decode_dotted(packet, a.rdoffset, namebuf, sizeof(namebuf));
			free(packet);

			if (i < 0)
				return 0;
			if (++nest > MAX_RECURSE)
				return 0;
			continue;
		} else if (a.atype == T_AAAA) {	/* ADDRESS */
			memcpy(&in, a.rdata, sizeof(in));
			h.h_name = namebuf;
			h.h_addrtype = AF_INET6;
			h.h_length = sizeof(in);
			h.h_addr_list = (char **) addr_list;
			free(packet);
			break;
		} else {
			free(packet);
			return 0;
		}
	}

	return &h;
#endif /* __UCLIBC_HAS_IPV6__ */
}
#endif

#ifdef L_getnetbyname

struct netent * getnetbyname(const char * name)
{
	return NULL;
}
#endif


#ifdef L_res_init

int res_init(void)
{
	return(0);
}
#endif


#ifdef L_res_query

#ifndef MIN
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#endif

int res_query(const char *dname, int class, int type,
              unsigned char *answer, int anslen)
{
	unsigned char * packet = 0;
	struct resolv_answer a;
	int i;

	open_nameservers();
	
	if (!dname || class != 1 /* CLASS_IN */)
		return(-1);
		
	memset((char *) &a, '\0', sizeof(a));

	i = dns_lookup(dname, type, nameservers, nameserver, &packet, &a);
	
	if (i < 0)
		return(-1);
			
	free(a.dotted);
		
	if (a.atype == type) { /* CNAME*/
		if (anslen && answer)
			memcpy(answer, a.rdata, MIN(anslen, a.rdlength));
		if (packet)
			free(packet);
		return(MIN(anslen, a.rdlength));
	}
	if (packet)
		free(packet);
	return 0;
}
#endif


#ifdef L_gethostbyaddr
struct hostent *gethostbyaddr (const void *addr, socklen_t len, int type)
{
	static struct hostent h;
	static char namebuf[256];
	static struct in_addr in;
	static struct in_addr *addr_list[2];
#ifdef __UCLIBC_HAS_IPV6__
    char *qp;
	static struct in6_addr	in6;
	static struct in6_addr	*addr_list6[2];
#endif /* __UCLIBC_HAS_IPV6__ */
	struct hostent *hp;
	unsigned char *packet;
	struct resolv_answer a;
	int i;
	int nest = 0;

	if (!addr)
		return 0;
        
    switch (type) {
	case AF_INET:
		if (len != sizeof(struct in_addr))
			return 0;
		break;
#ifdef __UCLIBC_HAS_IPV6__
	case AF_INET6:
		if (len != sizeof(struct in6_addr))
			return 0;
		break;
#endif /* __UCLIBC_HAS_IPV6__ */
	default:
		return 0;
	}

	if ((hp = get_hosts_byaddr(addr, len, type))) /* do /etc/hosts first */
		return(hp);

	open_nameservers();

	memset(&h, 0, sizeof(h));

	if(type == AF_INET) {
		unsigned char *tmp_addr = (unsigned char *)addr;

		memcpy(&in.s_addr, addr, len);

		addr_list[0] = &in;

		sprintf(namebuf, "%u.%u.%u.%u.in-addr.arpa",
			tmp_addr[3], tmp_addr[2], tmp_addr[1], tmp_addr[0]);
#ifdef __UCLIBC_HAS_IPV6__
	} else {
		memcpy(&in6.s6_addr, addr, len);

		addr_list6[0] = &in6;
        qp = namebuf;

		for (i = len - 1; i >= 0; i--) {
			qp += sprintf(qp, "%x.%x.", in6.s6_addr[i] & 0xf,
				(in6.s6_addr[i] >> 4) & 0xf);
    	}
    	strcpy(qp, "ip6.int");
#endif /* __UCLIBC_HAS_IPV6__ */
	}

	addr_list[1] = 0;

	for (;;) {

		i = dns_lookup(namebuf, T_PTR, nameservers, nameserver, &packet, &a);

		if (i < 0)
			return 0;

		strncpy(namebuf, a.dotted, sizeof(namebuf));
		free(a.dotted);

		if (a.atype == T_CNAME) {		/* CNAME */
			DPRINTF("Got a CNAME in gethostbyaddr()\n");
			i = decode_dotted(packet, a.rdoffset, namebuf, sizeof(namebuf));
			free(packet);

			if (i < 0)
				return 0;
			if (++nest > MAX_RECURSE)
				return 0;
			continue;
		} else if (a.atype == T_PTR) {	/* ADDRESS */
			i = decode_dotted(packet, a.rdoffset, namebuf, sizeof(namebuf));
			free(packet);

			h.h_name = namebuf;
			h.h_addrtype = type;

			if(type == AF_INET) {
				h.h_length = sizeof(in);
#ifdef __UCLIBC_HAS_IPV6__
			} else {
				h.h_length = sizeof(in6);
#endif /* __UCLIBC_HAS_IPV6__ */
    		}

			h.h_addr_list = (char **) addr_list;
			break;
		} else {
			free(packet);
			return 0;
		}
	}

	return &h;
}
#endif


#ifdef L_read_etc_hosts

void __open_etc_hosts(FILE **fp)
{
	if ((*fp = fopen("/etc/hosts", "r")) == NULL) {
		*fp = fopen("/etc/config/hosts", "r");
	}
	return;
}

struct hostent * read_etc_hosts(FILE * fp, const char * name, int type, enum etc_hosts_action action)
{
	static struct hostent	h;
	static struct in_addr	in;
	static struct in_addr	*addr_list[2];
#ifdef __UCLIBC_HAS_IPV6__
	static struct in6_addr	in6;
	static struct in6_addr	*addr_list6[2];
#endif /* __UCLIBC_HAS_IPV6__ */
	static char				line[80];
	char					*cp;
#define		 MAX_ALIAS		5
	char					*alias[MAX_ALIAS];
	int						aliases, i;

	if (action!=GETHOSTENT) {
		__open_etc_hosts(&fp);
		if (fp == NULL) {
			return((struct hostent *)NULL);
		}
	}

	while (fgets(line, sizeof(line), fp)) {
		if ((cp = strchr(line, '#')))
			*cp = '\0';
		aliases = 0;

		cp = line;
		while (*cp) {
			while (*cp && isspace(*cp))
				*cp++ = '\0';
			if (!*cp)
				continue;
			if (aliases < MAX_ALIAS)
				alias[aliases++] = cp;
			while (*cp && !isspace(*cp))
				cp++;
		}

		if (aliases < 2)
			continue; /* syntax error really */
		
		if (action==GETHOSTENT) {
			/* Return whatever the next entry happens to be. */
			break;
		} else if (action==GET_HOSTS_BYADDR) {
			if (strcmp(name, alias[0]) != 0)
				continue;
		} else {
			/* GET_HOSTS_BYNAME */
			for (i = 1; i < aliases; i++)
				if (strcasecmp(name, alias[i]) == 0)
					break;
			if (i >= aliases)
				continue;
		}

		if (type == AF_INET && inet_pton(AF_INET, alias[0], &in) > 0) {
			addr_list[0] = &in;
			addr_list[1] = 0;
			h.h_name = alias[1];
			h.h_addrtype = AF_INET;
			h.h_length = sizeof(in);
			h.h_addr_list = (char**) addr_list;
#ifdef __UCLIBC_HAS_IPV6__
        } else if (type == AF_INET6 && inet_pton(AF_INET6, alias[0], &in6) > 0) {
			addr_list6[0] = &in6;
			addr_list6[1] = 0;
			h.h_name = alias[1];
			h.h_addrtype = AF_INET6;
			h.h_length = sizeof(in6);
			h.h_addr_list = (char**) addr_list6;
#endif /* __UCLIBC_HAS_IPV6__ */
		} else {
			break; /* bad ip address */
        }
        
		if (action!=GETHOSTENT) {
			fclose(fp);
		}
		return(&h);
	}
	if (action!=GETHOSTENT) {
		fclose(fp);
	}
	return((struct hostent *) NULL);
}
#endif


#ifdef L_endhostent
extern int __stay_open;
extern FILE * __gethostent_fp;
void endhostent (void)
{
    __stay_open = 0;
    if (__gethostent_fp) {
	fclose(__gethostent_fp);
    }
}
#endif

#ifdef L_sethostent
extern int __stay_open;
void sethostent (int stay_open)
{
    __stay_open = stay_open;
}
#endif

#ifdef L_gethostent
int __stay_open;
FILE * __gethostent_fp;

struct hostent *gethostent (void)
{
    struct hostent *host;

    if (__gethostent_fp == NULL) {
	__open_etc_hosts(&__gethostent_fp);
	if (__gethostent_fp == NULL) {
	    return((struct hostent *)NULL);
	}
    }

    host = read_etc_hosts(__gethostent_fp, NULL, AF_INET, GETHOSTENT);
    if (__stay_open==0) {
	fclose(__gethostent_fp);
    }
    return(host);
}
#endif

#ifdef L_get_hosts_byname

struct hostent * get_hosts_byname(const char * name, int type)
{
	return(read_etc_hosts(NULL, name, type, GET_HOSTS_BYNAME));
}
#endif


#ifdef L_get_hosts_byaddr

struct hostent * get_hosts_byaddr(const char * addr, int len, int type)
{
#ifndef __UCLIBC_HAS_IPV6__
	char	ipaddr[INET_ADDRSTRLEN];
#else
	char	ipaddr[INET6_ADDRSTRLEN];
#endif /* __UCLIBC_HAS_IPV6__ */

    switch (type) {
	case AF_INET:
		if (len != sizeof(struct in_addr))
			return 0;
		break;
#ifdef __UCLIBC_HAS_IPV6__
	case AF_INET6:
		if (len != sizeof(struct in6_addr))
			return 0;
		break;
#endif /* __UCLIBC_HAS_IPV6__ */
	default:
		return 0;
	}

	inet_ntop(type, addr, ipaddr, sizeof(ipaddr));

	return(read_etc_hosts(NULL, ipaddr, type, GET_HOSTS_BYADDR));
}
#endif

#ifdef L_getnameinfo

#ifndef min
# define min(x,y) (((x) > (y)) ? (y) : (x))
#endif /* min */

int getnameinfo (const struct sockaddr *sa, socklen_t addrlen, char *host,
	     socklen_t hostlen, char *serv, socklen_t servlen,
	     unsigned int flags)
{
	int serrno = errno;
	int ok = 0;
	struct hostent *h = NULL;
    char domain[256];
    
	if (flags & ~(NI_NUMERICHOST|NI_NUMERICSERV|NI_NOFQDN|NI_NAMEREQD|NI_DGRAM))
		return EAI_BADFLAGS;

	if (sa == NULL || addrlen < sizeof (sa_family_t))
		return EAI_FAMILY;

	switch (sa->sa_family) {
	case AF_LOCAL:
		break;
	case AF_INET:
		if (addrlen < sizeof (struct sockaddr_in))
			return EAI_FAMILY;
		break;
#ifdef __UCLIBC_HAS_IPV6__
	case AF_INET6:
		if (addrlen < sizeof (struct sockaddr_in6))
			return EAI_FAMILY;
		break;
#endif /* __UCLIBC_HAS_IPV6__ */
	default:
		return EAI_FAMILY;
	}

	if (host != NULL && hostlen > 0)
		switch (sa->sa_family) {
		case AF_INET:
#ifdef __UCLIBC_HAS_IPV6__
		case AF_INET6:
#endif /* __UCLIBC_HAS_IPV6__ */
			if (!(flags & NI_NUMERICHOST)) {
#ifdef __UCLIBC_HAS_IPV6__
				if (sa->sa_family == AF_INET6)
					h = gethostbyaddr ((const void *) &(((const struct sockaddr_in6 *) sa)->sin6_addr),
						sizeof(struct in6_addr), AF_INET6);
				else
#endif /* __UCLIBC_HAS_IPV6__ */
                    h = gethostbyaddr ((const void *) &(((const struct sockaddr_in *)sa)->sin_addr),
					  sizeof(struct in_addr), AF_INET);

				if (h) {
					char *c;
					if ((flags & NI_NOFQDN)
					    && (getdomainname (domain, sizeof(domain)) == 0)
					    && (c = strstr (h->h_name, domain))
					    && (c != h->h_name) && (*(--c) == '.')) {
						strncpy (host, h->h_name,
							min(hostlen, (size_t) (c - h->h_name)));
						host[min(hostlen - 1, (size_t) (c - h->h_name))] = '\0';
						ok = 1;
					} else {
						strncpy (host, h->h_name, hostlen);
						ok = 1;
					}
				 }
			}

			if (!ok) {
				if (flags & NI_NAMEREQD) {
					errno = serrno;
					return EAI_NONAME;
				} else {
					const char *c;
#ifdef __UCLIBC_HAS_IPV6__
					if (sa->sa_family == AF_INET6) {
						const struct sockaddr_in6 *sin6p;

						sin6p = (const struct sockaddr_in6 *) sa;

						c = inet_ntop (AF_INET6,
							(const void *) &sin6p->sin6_addr, host, hostlen);
#if 0
/* Does scope id need to be supported? */
						uint32_t scopeid;
						scopeid = sin6p->sin6_scope_id;
						if (scopeid != 0) {
							/* Buffer is >= IFNAMSIZ+1.  */
							char scopebuf[IFNAMSIZ + 1];
							char *scopeptr;
							int ni_numericscope = 0;
							size_t real_hostlen = __strnlen (host, hostlen);
							size_t scopelen = 0;

							scopebuf[0] = SCOPE_DELIMITER;
							scopebuf[1] = '\0';
							scopeptr = &scopebuf[1];

							if (IN6_IS_ADDR_LINKLOCAL (&sin6p->sin6_addr)
							    || IN6_IS_ADDR_MC_LINKLOCAL (&sin6p->sin6_addr)) {
								if (if_indextoname (scopeid, scopeptr) == NULL)
									++ni_numericscope;
								else
									scopelen = strlen (scopebuf);
							} else {
								++ni_numericscope;
                            }

							if (ni_numericscope)
								scopelen = 1 + snprintf (scopeptr,
									(scopebuf
									+ sizeof scopebuf
									- scopeptr),
									"%u", scopeid);

							if (real_hostlen + scopelen + 1 > hostlen)
								return EAI_SYSTEM;
							memcpy (host + real_hostlen, scopebuf, scopelen + 1);
						}
#endif
					} else
#endif /* __UCLIBC_HAS_IPV6__ */
						c = inet_ntop (AF_INET,
							(const void *) &(((const struct sockaddr_in *) sa)->sin_addr),
							host, hostlen);

					if (c == NULL) {
						errno = serrno;
						return EAI_SYSTEM;
					}
				}
				ok = 1;
			}
			break;

		case AF_LOCAL:
			if (!(flags & NI_NUMERICHOST)) {
				struct utsname utsname;

				if (!uname (&utsname)) {
					strncpy (host, utsname.nodename, hostlen);
					break;
				};
			};

			if (flags & NI_NAMEREQD) {
				errno = serrno;
				return EAI_NONAME;
			}

			strncpy (host, "localhost", hostlen);
			break;

		default:
			return EAI_FAMILY;
	}

	if (serv && (servlen > 0)) {
		switch (sa->sa_family) {
		case AF_INET:
#ifdef __UCLIBC_HAS_IPV6__
		case AF_INET6:
#endif /* __UCLIBC_HAS_IPV6__ */
			if (!(flags & NI_NUMERICSERV)) {
				struct servent *s;
				s = getservbyport (((const struct sockaddr_in *) sa)->sin_port,
				      ((flags & NI_DGRAM) ? "udp" : "tcp"));
				if (s) {
					strncpy (serv, s->s_name, servlen);
					break;
				}
			}
			snprintf (serv, servlen, "%d",
				ntohs (((const struct sockaddr_in *) sa)->sin_port));
			break;

		case AF_LOCAL:
			strncpy (serv, ((const struct sockaddr_un *) sa)->sun_path, servlen);
			break;
		}
	}
	if (host && (hostlen > 0))
		host[hostlen-1] = 0;
	if (serv && (servlen > 0))
		serv[servlen-1] = 0;
	errno = serrno;
	return 0;
}
#endif
