#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<arpa/inet.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"
#include"include/ethernet.h"
#include"include/ip.h"


void print_ip_addr(uint32_t *addr) {
	uint8_t *ad;
	ad = (uint8_t *)addr;


	printf("%u.%u.%u.%u\n", *ad, *(ad + 1), *(ad + 2), *(ad + 3));

	return;
}

void print_ip_hdr(struct ip_hdr *ip_hdr) {
	printf("version: %u\n", ip_hdr->version);
	printf("header length: %u (%d byte)\n", ip_hdr->hdr_len, ip_hdr->hdr_len * 4);
	printf("type of service: %u\n",ip_hdr->type_of_service);
	printf("total length: %u\n", ntohs(ip_hdr->total_len));
	printf("identification: 0x%x\n", ntohs(ip_hdr->id));
	printf("Time to Live: %u\n", ip_hdr->ttl);
	printf("proto: %x\n", ip_hdr->proto);

	//printf("Source IP Address: %s\n", inet_ntoa(ip_hdr->src_addr));
	//printf("Destination IP Address: %s\n", inet_ntoa(ip_hdr->dest_addr));
	print_ip_addr(&ip_hdr->src_addr);
	print_ip_addr(&ip_hdr->dest_addr);
	//printf("src ip: %x\n", ip_hdr->src_addr);
	//printf("dest ip: %x\n", ip_hdr->dest_addr);

	return;
}




void rx_ip(uint8_t *packet, uint32_t size, struct port_config *port) {
	struct ip_hdr *p = (struct ip_hdr *)packet;

	print_ip_hdr(p);

//	printf("version: %u\n", p->version);
//	printf("header length: %u (%d byte)\n", p->hdr_len, p->hdr_len * 4);
//	printf("type of service: %u\n",p->type_of_service);
//	printf("total length: %u\n", ntohs(p->total_len));
//	printf("identification: 0x%x\n", ntohs(p->id));
//	printf("Time to Live: %u\n", p->ttl);
//	printf("proto: %x\n", p->proto);



	return;
}
