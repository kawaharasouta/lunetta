#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/arp.h"








int arp_resolve(const uint32_t *ip_addr, ethernet_addr *ether_addr, const void *data, uint32_t size) {
	ether_addr->addr[0] = 0xff;
	ether_addr->addr[1] = 0xff;
	ether_addr->addr[2] = 0xff;
	ether_addr->addr[3] = 0xff;
	ether_addr->addr[4] = 0xff;
	ether_addr->addr[5] = 0x0f;

	return 1;

}
