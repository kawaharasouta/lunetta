#ifndef __ARP_H_
#define __ARP_H_

/* ARP type num */
#define ARP_HRD_ETHERNET 0x0001
#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY   2

/* ARP table conf */
#define ARP_TABLE_SIZE 4096
#define ARP_LOOKUP_TIMEOUT_SEC 1

/* ARP protocol opcodes. */
#define ARPOP_REQUEST 1   /* ARP request.  */
#define ARPOP_REPLY 2   /* ARP reply.  */
#define ARPOP_RREQUEST  3   /* RARP request.  */
#define ARPOP_RREPLY  4   /* RARP reply.  */
#define ARPOP_InREQUEST 8   /* InARP request.  */
#define ARPOP_InREPLY 9   /* InARP reply.  */
#define ARPOP_NAK 10    /* (ATM)ARP NAK.  */

//#include"ethernet.h"
//extern struct ethernet_addr;
#include"lunetta.h"
//extern struct port_config;

struct arp_hdr {
	uint16_t hrd_type;
	uint16_t proto_type;
	uint8_t hrd_len;
	uint8_t proto_len;
	uint16_t ar_op;
};

struct arp_ether {
	struct arp_hdr arphdr;
	ethernet_addr s_eth_addr;
	uint32_t s_ip_addr;
	ethernet_addr d_eth_addr;
	uint32_t d_ip_addr;
} __attribute__ ((packed));


void arp_table_dump();
void arp_init(struct port_config *port);
int arp_resolve(const uint32_t *ip_addr, ethernet_addr *ether_addr, const void *data, uint32_t size, struct port_config *port);
void tx_arp();
void rx_arp(uint8_t *packet, uint32_t size, struct port_config *port);

#endif
