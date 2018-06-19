#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <rte_mbuf.h>



void tx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void){
	//struct queue_pkt queue_pkt = rx_queue_pop()
	//uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
}


void rx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void){
	uint32_t size;
	struct pkt_queue pkt_queue = rx_queue_pop(&size);

	uint32_t *p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
	rte_hexdump(stdout, "", (const void *)p, size);

	return 0;
}
