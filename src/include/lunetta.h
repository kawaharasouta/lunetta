#ifndef __LUNETTA_H_
#define __LUNETTA_H

#include"ethernet.h"

struct port_config {
	uint16_t port_num;
	ethernet_addr mac_addr;
	uint32_t ip_addr;
	uint32_t ip_netmask;
	uint32_t ip_gateway;
};

int lunetta_init();

#endif
