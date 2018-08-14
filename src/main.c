#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<rte_eal.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ether.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"
#include"include/ethernet.h"
#include"include/arp.h"


int main(int argc, char **argv){
	if (lunetta_init() == -1) {
		exit(1);
	}

	struct port_config port;
	port.port_num = 0;
	port_init(&port);
	port_setup(&port);
	arp_init(&port);

	print_mac_addr(&port.mac_addr);

	struct ether_port *ether_port;
	
	uint16_t nb_ports;
	uint16_t nport = port.port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	struct rte_mbuf *tbufs[BURST_SIZE];
	uint16_t nb_rx;
	uint16_t nb_tx;

	printf("launch from master\n");
	rte_eal_remote_launch(launch_lcore_rxtx, (void *)&port, 1);

	while(1) {
		sleep(1);

		if (port.rx_queue.num > 0)
			rx_ether(&port);

		struct rte_mbuf *mbuf;
		mbuf = rte_pktmbuf_alloc(mbuf_pool);

		//tx_ether(mbuf, 0, &port, 0x0800, NULL, NULL);
		uint32_t q = 0x0a000003;
		send_req(&q, &port);
	}

	rte_eal_wait_lcore(1);

//	pthread_mutex_destroy(&tx_queue.mutex);
//	pthread_mutex_destroy(&rx_queue.mutex);

	return 0;
}
