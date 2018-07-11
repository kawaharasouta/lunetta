#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<arpa/inet.h>//byte order

#include <rte_mbuf.h>
#include <rte_ether.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"
#include"include/ethernet.h"
#include"include/ip.h"
#include"include/arp.h"



void print_mac_addr(ethernet_addr *addr) {
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
	addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
}

int equal_mac_addr(ethernet_addr *addr1, ethernet_addr *addr2) {
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		if (addr1->addr[i] != addr2->addr[i])
			return -1;
	}
	return 0;
}

int is_ether_broadcast(ethernet_addr *addr) {
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		if (addr->addr[i] != 0xff)
			return -1;
	}
}

void print_ethernet_hdr(struct ethernet_hdr *ether_hdr) {
	printf("ether dest addr: ");
	print_mac_addr(&ether_hdr->dest);
	printf("ether src addr: ");
	print_mac_addr(&ether_hdr->src);
	printf("type: %04x\n", ntohs(ether_hdr->type));

	return;
}

void tx_ether(/*struct rte_mbuf *mbuf, */uint32_t size, struct port_config *port, uint16_t type, const void *paddr, ethernet_addr *dest) {
	int ret;
	uint32_t len = 64;
	ethernet_addr haddr;

	struct rte_mbuf *mbuf;
	mbuf = rte_pktmbuf_alloc(mbuf_pool);
	//uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	mbuf = rte_pktmbuf_alloc(mbuf_pool);
  mbuf->pkt_len = len;
	mbuf->data_len = len;
	mbuf->port = port->port_num;
	mbuf->packet_type = 1;

	haddr.addr[0] = 0xff;
  haddr.addr[1] = 0xff;
  haddr.addr[2] = 0xff;
  haddr.addr[3] = 0xff;
  haddr.addr[4] = 0xff;
  haddr.addr[5] = 0xff;

	uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	struct ethernet_hdr *eth = p;

	memcpy(eth->dest.addr, haddr.addr, ETHER_ADDR_LEN);
	memcpy(eth->src.addr, port->mac_addr.addr, ETHER_ADDR_LEN);
	uint16_t _type = 0x0800;
	eth->type = htons(_type);
	p += sizeof(struct ethernet_hdr);
	memset(p, 0, 40);


	//ret = arp_resolve(paddr, &haddr, p, size);
	//if (ret != 1) {
	//	return ret;
	//}
	//dest = &haddr;
	//make header!!!!!!!!!
	//uint8_t *pp = (uint8_t *)rte_pktmbuf_prepend(mbuf, sizeof(struct ethernet_hdr));
	//if (!p) {
	//	fprintf(stderr, "mbuf prepend err\n");
	//	exit(1);
	//}
	//struct ethernet_hdr *eth = pp;
	//memcpy(eth->dest.addr, dest->addr, ETHER_ADDR_LEN);
	//memcpy(eth->src.addr, port->mac_addr.addr, ETHER_ADDR_LEN);
	//eth->type = htons(type);
	//!!!!!!!!!!
	//len = sizeof(struct ethernet_hdr) + size;
	//if (len < 64) {
	//	pp += len;
	//	memset(pp, 0, 64 - len);
	//	len = 64;
	//}
	//else if (size > 1512) {
	//	return;
	//}
	//mbuf->pkt_len = len;
	//mbuf->data_len = len;
	//mbuf->port = port->port_num;
	//mbuf->packet_type = 1;
	tx_queue_push(mbuf, len);
	return;
}

void rx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/struct port_config *port) {
	uint32_t pop_size;
	int rx_pop_num;

	struct rte_mbuf *mbuf = rx_queue_pop(&pop_size);
	printf("pop_size: %d\n", pop_size);
	if (pop_size > 1052)
		return;

	uint32_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	//rte_hexdump(stdout, "", (const void *)p, pop_size);


	struct ethernet_hdr *packet = (struct ethernet_hdr *)p;
	uint8_t *pp = (uint8_t *)p;

	printf("sizeof %u\n", sizeof(struct ethernet_hdr));
	pp += sizeof(struct ethernet_hdr);
	pop_size -= sizeof(struct ethernet_hdr);

	print_ethernet_hdr(packet);

	printf("port mac addr:");
	print_mac_addr(&port->mac_addr);

	
	if (equal_mac_addr(&port->mac_addr, &packet->src) == 0 | is_ether_broadcast(&packet->src) == 0 ) {
		switch (ntohs(packet->type)) {
			case ETHERTYPE_IP:
			{
				printf("ip\n");
				rx_ip((uint8_t *)pp, pop_size, port);
				//struct ip_hdr *ip = (struct ip_hdr *)p;
				//printf("version: %u\n", ip->version);
				break;
			}
			case ETHERTYPE_ARP:
			{
				printf("arp\n");
				//rx_arp();
				break;
			}
			default:
			{
				printf("wow\n");
				break;
			}
		}
	}
	else {//different mac addr
		printf("uooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
	}

	rte_pktmbuf_free(mbuf);
	

	return;
}
