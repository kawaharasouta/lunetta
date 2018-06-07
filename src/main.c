#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<rte_eal.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ether.h>

#include"include/pkt_io.h"



int main(int argc, char **argv){
	dpdk_init();
	struct port *port;

	port = port_open(0);

	for (;;){
		rx_pkt(port);

		struct rte_mbuf *mbuf = rx_queue_pop();
		if (mbuf != NULL){
			uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
			size_t size = rte_pktmbuf_pkt_len(mbuf);
			rte_hexdump(stdout, "", (const void *)p, size);
			

			tx_queue_push(mbuf);
			tx_pkt(port);
		}
	}
	return 0;
}
