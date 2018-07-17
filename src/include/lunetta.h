#ifndef __LUNETTA_H_
#define __LUNETTA_H_

#include"ethernet.h"
//extern struct ethernet_addr;

struct port_config {
	uint16_t port_num;
	ethernet_addr mac_addr;
	uint32_t ip_addr;
	uint32_t ip_netmask;
	uint32_t ip_gateway;
};

void port_setup(struct port_config *port);
int lunetta_init();

#endif
