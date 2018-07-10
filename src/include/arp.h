#ifndef __ARP_H_
#define __ARP_H_

#include"ethernet.h"

struct arp_hdr {
	uint16_t hrd_addr;
	uint16_t proto_addr;
	uint8_t hrd_len;
	uint8_t protp_len;
	uint16_t ar_op;
};

struct arp_ether {
	struct arphdr;
	ethernet_addr s_eth_addr;
	uint32_t s_ip_addr;
	ethernet_addr d_eth_addr;
	uint32_t d_ip_addr;
} __attribute__ ((packed));

#endif
