#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<arpa/inet.h>//byte order

#include <rte_mbuf.h>
#include <rte_ether.h>

#include"include/pkt_io.h"
#include"include/ethernet.h"



void print_mac_addr(ethernet_addr *addr) {
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
	addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
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

void rx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void) {
	uint32_t pop_size;
	int rx_pop_num;

	struct rte_mbuf *mbuf = rx_queue_pop(&pop_size);
	printf("pop_size: %d\n", pop_size);
	if (pop_size > 1052)
		return;

	uint32_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	rte_hexdump(stdout, "", (const void *)p, pop_size);


	struct ethernet_hdr *packet = (struct ethernet_hdr *)p;

	print_ethernet_hdr(packet);


	switch (ntohs(packet->type)) {
		case ETHERTYPE_IP:
		{
			printf("ip\n");
			//rx_ip();
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

	rte_pktmbuf_free(mbuf);
	

	return;
}
