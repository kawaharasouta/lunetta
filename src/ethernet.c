#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <rte_mbuf.h>



size_t tx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void){
	struct queue_pkt queue_pkt = rx_queue_pop()
	uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
}

size_t rx_ether(struct rte_mbuf *mbuf, uint32_t size){

}
