/*
 * Copyright (C) 2017 Joyent Inc.
 * Copyright (C) 2017 Matthew Macy <matt.macy@joyent.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef UVX_VXLAN_H_
#define UVX_VXLAN_H_
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include "datapath.h"

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct	arphdr {
	u_short	ar_hrd;		/* format of hardware address */
#define ARPHRD_ETHER 	1	/* ethernet hardware format */
#define ARPHRD_IEEE802	6	/* token-ring hardware format */
#define ARPHRD_ARCNET	7	/* arcnet hardware format */
#define ARPHRD_FRELAY 	15	/* frame relay hardware format */
#define ARPHRD_IEEE1394	24	/* firewire hardware format */
#define ARPHRD_INFINIBAND 32	/* infiniband hardware format */
	u_short	ar_pro;		/* format of protocol address */
	u_char	ar_hln;		/* length of hardware address */
	u_char	ar_pln;		/* length of protocol address */
	u_short	ar_op;		/* one of: */
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
#define	ARPOP_REVREQUEST 3	/* request protocol address given hardware */
#define	ARPOP_REVREPLY	4	/* response giving protocol address */
#define ARPOP_INVREQUEST 8 	/* request to identify peer */
#define ARPOP_INVREPLY	9	/* response identifying peer */

/* NB: delete an entry by issuing a gratuitous reply with 255.255.255.255 */

/*
 * The remaining fields are variable in size,
 * according to the sizes above.
 */
#ifdef COMMENT_ONLY
	u_char	ar_sha[];	/* sender hardware address */
	u_char	ar_spa[];	/* sender protocol address */
	u_char	ar_tha[];	/* target hardware address */
	u_char	ar_tpa[];	/* target protocol address */
#endif
};

#define ar_sha(ap)	(((caddr_t)((ap)+1)) +   0)
#define ar_spa(ap)	(((caddr_t)((ap)+1)) +   (ap)->ar_hln)
#define ar_tha(ap)	(((caddr_t)((ap)+1)) +   (ap)->ar_hln + (ap)->ar_pln)
#define ar_tpa(ap)	(((caddr_t)((ap)+1)) + 2*(ap)->ar_hln + (ap)->ar_pln)

#define AE_IPFW_INGRESS 0x1
#define AE_IPFW_EGRESS	0x2

struct arphdr_ether {
    union {
		uint64_t data;
		struct arphdr fields;
    } ae_hdr;
    uint8_t	ae_sha[ETHER_ADDR_LEN];
    uint32_t	ae_spa;
    uint8_t	ae_tha[ETHER_ADDR_LEN];
	union {
		uint32_t	uae_tpa;
		struct {
			uint8_t usae_tvxlanid[3];
			uint8_t usae_tflags;
		} s;
	} u;
} __packed;

#define ae_tpa u.uae_tpa
#define ae_tflags u.s.usae_tflags
#define ae_tvxlanid u.s.usae_tvxlanid



#ifdef __cplusplus
extern "C" {
#endif
	int data_dispatch(char *rxbuf, char *txbuf, path_state_t *, void *arg);
	int cmd_dispatch(char *rxbuf, char *txbuf, path_state_t *, void *arg);
	int cmd_initiate(char *rxbuf, char *txbuf, path_state_t *, void *arg);
	void netmap_enqueue(struct mbuf *m, int proto);
	int cmd_dispatch_config(char *rxbuf, char *txbuf, path_state_t *ps, void *arg);
#ifdef __cplusplus
}
#endif


#endif
