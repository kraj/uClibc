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
 */

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
#include <cfgfile.h>
#include <resolv.h>
#include <arpa/nameser.h>

#define MAX_RECURSE 5
#define REPLY_TIMEOUT 10
#define MAX_RETRIES 15
#define MAX_SERVERS 3
#define MAX_SEARCH 4

#undef DEBUG
#ifdef DEBUG
#define DPRINTF(X,args...) printf(X,args...)
#else
#define DPRINTF(X,args...)
#endif							/* DEBUG */

#ifdef L_encodeh
int encode_header(struct resolv_header *h, unsigned char *dest, int maxlen)
{
	if (maxlen < HFIXEDSZ)
		return -1;

	dest[0] = (h->id & 0xff00) >> 8;
	dest[1] = (h->id & 0x00ff) >> 0;
	dest[2] = (h->qr ? 0x80 : 0) |
		((h->opcode & 0x0f) << 3) |
		(h->aa ? 0x04 : 0) | (h->tc ? 0x02 : 0) | (h->rd ? 0x01 : 0);
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

	while ((l = data[offset++])) {
		
		if (measure && total++)
			break;

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

int dns_lookup(const char *name, int type, int nscount, const char **nsip,
			   unsigned char **outpacket, struct resolv_answer *a)
{
	static int id = 1;
	int i, j, len, fd, pos;
	static int ns = 0;
	struct sockaddr_in sa;
	int oldalarm;
	__sighandler_t oldhandler;
	struct resolv_header h;
	struct resolv_question q;
	int retries = 0;
	unsigned char * packet = malloc(PACKETSZ);
	unsigned char * lookup = malloc(MAXDNAME);
	int variant = 0;
	extern int searchdomains;
	extern const char * searchdomain[MAX_SEARCH];

	if (!packet || !lookup || !nscount)
	    goto fail;

	DPRINTF("Looking up type %d answer for '%s'\n", type, name);

	ns %= nscount;

	fd = -1;

	while (retries++ < MAX_RETRIES) {

		if (fd != -1)
			close(fd);

		fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (fd == -1)
			goto fail;

		memset(packet, 0, PACKETSZ);

		memset(&h, 0, sizeof(h));
		h.id = ++id;
		h.qdcount = 1;
		h.rd = 1;

		DPRINTF("encoding header\n");

		i = encode_header(&h, packet, PACKETSZ);
		if (i < 0)
			goto fail;

		strncpy(lookup,name,MAXDNAME);
		if (variant < searchdomains)
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

		sa.sin_family = AF_INET;
		sa.sin_port = htons(NAMESERVER_PORT);
		sa.sin_addr.s_addr = inet_addr(nsip[ns]);

		if (connect(fd, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
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

		DPRINTF("Got response (i think)!\n");
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
	const char **nsip, struct in_addr *in)
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

int resolve_mailbox(const char *address,
					int nscount, const char **nsip, struct in_addr *in)
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

extern int nameservers;
extern const char * nameserver[MAX_SERVERS];
extern int searchdomains;
extern const char * searchdomain[MAX_SEARCH];

#ifdef L_opennameservers

int nameservers;
const char * nameserver[MAX_SERVERS];
int searchdomains;
const char * searchdomain[MAX_SEARCH];

int open_nameservers()
{
	FILE *fp;
	char **arg;
	int i;

	if (nameservers > 0) 
	    return 0;

	if ((fp = fopen("/etc/resolv.conf", "r"))) {
		if (0 != (arg = cfgfind(fp, "nameserver"))) {
			for (i=1; arg[i] && nameservers <= MAX_SERVERS; i++) {
				nameserver[nameservers++] = strdup(arg[i]);
				DPRINTF("adding nameserver %s\n",arg[i]);
			}
		}
		if (0 != (arg = cfgfind(fp, "search"))) {
			for (i=1; arg[i] && searchdomains <= MAX_SEARCH; i++) {
				searchdomain[searchdomains++] = strdup(arg[i]);
				DPRINTF("adding search %s\n",arg[i]);
			}
		}
		fclose(fp);
	} else {
	    DPRINTF("failed to open resolv.conf\n");
	}
	DPRINTF("nameservers = %d\n", nameservers);
	return 0;
}
#endif

#ifdef L_closenameservers
void close_nameservers(void)
{

	while (nameservers > 0)
		free((void*)nameserver[--nameservers]);
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
	unsigned char *packet;
	struct resolv_answer a;
	int i;
	int nest = 0;

	open_nameservers();

	if (!name)
		return 0;

	memset(&h, 0, sizeof(h));

	addr_list[0] = &in;
	addr_list[1] = 0;

	strcpy(namebuf, name);

	for (;;) {

		i = dns_lookup(namebuf, 1, nameservers, nameserver, &packet, &a);

		if (i < 0)
			return 0;

		strncpy(namebuf, a.dotted, sizeof(namebuf));
		free(a.dotted);


		if (a.atype == T_CNAME) {		/* CNAME */
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

#ifdef L_gethostbyaddr

struct hostent *gethostbyaddr(const char *addr, int len, int type)
{
	static struct hostent h;
	static char namebuf[256];
	static struct in_addr in;
	static struct in_addr *addr_list[2];
	unsigned char *packet;
	struct resolv_answer a;
	int i;
	int nest = 0;

	if (!addr || (len != sizeof(in)) || (type != AF_INET))
		return 0;

	memcpy(&in.s_addr, addr, len);

	open_nameservers();

	memset(&h, 0, sizeof(h));

	addr_list[0] = &in;
	addr_list[1] = 0;

	sprintf(namebuf, "%d.%d.%d.%d.in-addr.arpa",
			(in.s_addr >> 24) & 0xff,
			(in.s_addr >> 16) & 0xff,
			(in.s_addr >> 8) & 0xff, (in.s_addr >> 0) & 0xff);

	for (;;) {

		i = dns_lookup(namebuf, T_PTR, nameservers, nameserver, &packet, &a);

		if (i < 0)
			return 0;

		strncpy(namebuf, a.dotted, sizeof(namebuf));
		free(a.dotted);

		if (a.atype == T_CNAME) {		/* CNAME */
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
			h.h_addrtype = AF_INET;
			h.h_length = sizeof(in);
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
