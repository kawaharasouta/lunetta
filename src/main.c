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

#include"include/pkt_io.h"



int main(int argc, char **argv){
	dpdk_init();
	struct port *port;

	port = port_open(0);


	uint16_t nb_ports;
	uint16_t nport = port->port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	struct rte_mbuf *tbufs[BURST_SIZE];
	uint16_t nb_rx;
	uint16_t nb_tx;

	printf("launch from master\n");
	//rte_eal_remote_launch(launch_lcore_rx, (void *)port, 1);
	//rte_eal_remote_launch(launch_lcore_tx, (void *)port, 2);
	rte_eal_remote_launch(launch_lcore_rxtx, (void *)port, 1);

	while(1) {
		sleep(1);
		//printf("***\n");
		int j;
		uint32_t pop_size;
		int rx_pop_num;
		//pthread_mutex_lock(&tx_queue.mutex);
		//pthread_mutex_lock(&rx_queue.mutex);
		rx_pop_num = rx_queue.num;
		/*if (rx_pop_num > 0) {
			printf("rx_pop_num > 0\n");
		}*/
		for (j = 0; j < rx_pop_num; j++){
			struct rte_mbuf *mbuf;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
			mbuf = rx_queue_pop(&pop_size);
			if (pop_size != 60) {
				printf("pop_size: %u\n", pop_size);
				//continue;
			}
			uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
			//uint32_t size = rte_pktmbuf_pkt_len(bufs[j]);
			rte_hexdump(stdout, "", (const void *)p, pop_size);
	//		p = NULL;
			//pthread_mutex_lock(&tx_queue.mutex);
			tx_queue_push(mbuf, pop_size);
			//pthread_mutex_unlock(&tx_queue.mutex);
		}
		//pthread_mutex_unlock(&tx_queue.mutex);
		//pthread_mutex_unlock(&rx_queue.mutex);
	}


	rte_eal_wait_lcore(1);
	//rte_eal_wait_lcore(2);

	pthread_mutex_destroy(&tx_queue.mutex);
	pthread_mutex_destroy(&rx_queue.mutex);
	

	return 0;
}
