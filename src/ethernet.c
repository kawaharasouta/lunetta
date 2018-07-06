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

void tx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void) {
	//struct queue_pkt queue_pkt = rx_queue_pop()
	//uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
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
