#ifndef __LUNETTA_H_
#define __LUNETTA_H_

#include"ethernet.h"
#include"queue.h"

#include<pthread.h>

//extern struct ethernet_addr;

struct port_config {
	uint16_t port_num;
	ethernet_addr mac_addr;
	uint32_t ip_addr;
	uint32_t ip_netmask;
	uint32_t ip_gateway;

	struct queue_info tx_queue;
	pthread_mutex_t tx_mutex;
	struct queue_info rx_queue;
	pthread_mutex_t rx_mutex;
};

void port_setup(struct port_config *port);
int lunetta_init();

#endif
