#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<arpa/inet.h>//byte order

#include <rte_mbuf.h>
#include <rte_ether.h>
#include<rte_memcpy.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"
#include"include/ethernet.h"
#include"include/ip.h"
#include"include/arp.h"
#include"include/queue.h"

#define ETHER_PORT_MAX_NUM 1

struct ether_port ports[ETHER_PORT_MAX_NUM];

ethernet_addr ether_broadcast = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

int ethernet_init(struct port_config *port, uint16_t num) {
	if (num > ETHER_PORT_MAX_NUM)
		num = ETHER_PORT_MAX_NUM;
	for (int i = 0; i < ETHER_PORT_MAX_NUM; i++) {
		ports[i].port_num = port->port_num;
		ports[i].mac_addr = port->mac_addr;
	}
}
struct ether_port* get_port_pointer() {
	return ports;
}

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
	if (!addr)
		return -1;
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		if (addr->addr[i] != 0xff)
			return -1;
	}
	return 0;
}

void print_ethernet_hdr(struct ethernet_hdr *ether_hdr) {
	printf("ether dest addr: ");
	print_mac_addr(&ether_hdr->dest);
	printf("ether src addr: ");
	print_mac_addr(&ether_hdr->src);
	printf("type: %04x\n", ntohs(ether_hdr->type));

	return;
}

void tx_ether(struct rte_mbuf *mbuf, uint32_t size, struct port_config *port, uint16_t type, const void *paddr, ethernet_addr *dest) {
	int ret;
	uint32_t len;/* = 64;*/
	ethernet_addr haddr;
	struct ethernet_hdr *eth;

	mbuf->port = port->port_num;
	mbuf->packet_type = 1;

	if (is_ether_broadcast(dest) == 0) {
		uint8_t *pp = (uint8_t *)rte_pktmbuf_prepend(mbuf, sizeof(uint8_t) * 14);
		if (!pp) {
			printf("mbuf prepend error\n");
			return;
		}
		eth = (struct ethernet_hdr *)pp;
		rte_memcpy(eth->dest.addr, dest, ETHER_ADDR_LEN);
		rte_memcpy(eth->src.addr, port->mac_addr.addr, ETHER_ADDR_LEN);
		eth->type = htons(type);

		len = sizeof(struct ethernet_hdr) + size;
		if (len < 64) {
			pp += len;
			memset(pp, 1, 64 - len);
			len = 64;
		}
		else if (size > 1512) {
			return;
		}
		mbuf->pkt_len = len;
		mbuf->data_len = len;
		queue_push(&port->tx_queue, mbuf, len);
		return;
	}

	uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	ret = arp_resolve(paddr, &haddr, p, size, port);
	if (ret != 1) {
		return;
	}

	uint8_t *q = p;
	uint16_t _type = 0x0800;
	uint8_t *pp = (uint8_t *)rte_pktmbuf_prepend(mbuf, sizeof(uint8_t) * 14);
	if (!pp) {
		printf("mbuf prepend error\n");
		return;
	}

	eth = (struct ethernet_hdr *)pp;
	rte_memcpy(eth->dest.addr, haddr.addr, ETHER_ADDR_LEN);
	rte_memcpy(eth->src.addr, port->mac_addr.addr, ETHER_ADDR_LEN);
	eth->type = htons(type);

	len = sizeof(struct ethernet_hdr) + size;
	if (len < 64) {
		pp += len;
		memset(pp, 1, 64 - len);
		len = 64;
	}
	else if (size > 1512) {
		return;
	}
	mbuf->pkt_len = len;
	mbuf->data_len = len;
	queue_push(&port->tx_queue, mbuf, len);
	return;
}

void rx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/struct port_config *port) {
	uint32_t pop_size;
	int rx_pop_num;
	struct rte_mbuf *mbuf;
	struct queue_node *ret;
	ret = queue_pop(&port->rx_queue);
	mbuf = (struct rte_mbuf *)ret->data;
	pop_size = ret->size;
	printf("pop_size: %d\n", pop_size);
	if (pop_size > 1512)
		return;

	uint32_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
	struct ethernet_hdr *packet = (struct ethernet_hdr *)p;
	uint8_t *pp = (uint8_t *)p;

	printf("sizeof %u\n", sizeof(struct ethernet_hdr));
	pp += sizeof(struct ethernet_hdr);
	pop_size -= sizeof(struct ethernet_hdr);

	print_ethernet_hdr(packet);

	printf("port mac addr:");
	print_mac_addr(&port->mac_addr);

	
	if (equal_mac_addr(&port->mac_addr, &packet->dest) == 0 || is_ether_broadcast(&packet->dest) == 0 ) {
		switch (ntohs(packet->type)) {
			case ETHERTYPE_IP:
			{
				printf("ip\n");
				rx_ip((uint8_t *)pp, pop_size, port);
				break;
			}
			case ETHERTYPE_ARP:
			{
				printf("arp\n");
				rx_arp((uint8_t *)pp, pop_size, port);
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
		printf("different mac addr\n");
	}

	rte_pktmbuf_free(mbuf);
	

	return;
}
