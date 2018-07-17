#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdint.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"

//struct port_config {
//	uint16_t port_num;
//	ethernet_addr mac_addr;
//	uint32_t ip_addr;
//	uint32_t ip_netmask;
//	uint32_t ip_gateway;
//};

void port_setup(struct port_config *port) {
	port->ip_addr = 0x0a000005;
	return;
}


int lunetta_init() {
	if (dpdk_init() == -1) {
		fprintf(stderr, "error dpdk_init");
		return -1;
	}


	return 0;
}


