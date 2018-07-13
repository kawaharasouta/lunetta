#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/arp.h"
#include"include/ip.h"
#include"include/ethernet.h"







int arp_resolve(const uint32_t *ip_addr, ethernet_addr *ether_addr, const void *data, uint32_t size) {
	ether_addr->addr[0] = 0xff;
	ether_addr->addr[1] = 0xff;
	ether_addr->addr[2] = 0xff;
	ether_addr->addr[3] = 0xff;
	ether_addr->addr[4] = 0xff;
	ether_addr->addr[5] = 0x0f;

	return 1;
}

void send_req() {

}

void send rep() {

}

void tx_arp() {

}

void rx_arp(uint8_t *packet, uint32_t size, struct port_config *port) {
	struct arp_ether *hdr;
	
	if (size < sizeof(struct arp_ether)) 
		return;

	hdr = (struct arp_ether *)packet;
	if (ntohs(hdr->arphdr.hrd_type) != ARP_HRD_ETHERNET) return;
	if (ntohs(hdr->arphdr.proto_type) != ETHERTYPE_IP) return;
	if (hdr->arphdr.hrd_len != ETHER_ADDR_LEN) return;
	if (hdr->arphdr.proto_len != IP_ADDR_LEN) return;

	//arp req or rep
	switch(hdr->arphdr.ar_op) {
		case ARPOP_REQUEST:
		{
			printf("arp req\n");
			break;
		}
		case ARPOP_REPLY:
		{
			printf("arp rep\n");
			break;
		}
		default:
		{
			printf("arp uooooooooooooooooo\n");
			break;
		}
	}


	return;
}
