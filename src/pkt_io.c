#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include <rte_eal.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ether.h>
//#include<pthread.h>

#include"include/lunetta.h"
#include"include/pkt_io.h"
#include"include/ethernet.h"
#include"include/queue.h"

struct rte_mempool *mbuf_pool;
static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
/*static */int
port_init(/*uint16_t port*/struct port_config *port)
{ 
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;
	struct ether_addr addr;
	uint16_t nport = port->port_num;
	
	if (nport >= rte_eth_dev_count()) {
		return -1;
	}
	
	/* Configure the Ethernet port. */ 
	retval = rte_eth_dev_configure(nport, rx_rings, tx_rings, &port_conf);
	if (retval != 0) {
		return retval;
	}
	
	retval = rte_eth_dev_adjust_nb_rx_tx_desc(nport, &nb_rxd, &nb_txd);
	if (retval != 0) {
		return retval;
	}
	
	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(nport, q, nb_rxd, rte_eth_dev_socket_id(nport), NULL, mbuf_pool);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(nport, q, nb_txd, rte_eth_dev_socket_id(nport), NULL);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(nport);
	if (retval < 0) {
		return retval;
	}
  
	/* Display the port MAC address. */
	rte_eth_macaddr_get(nport, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
		nport,
		addr.addr_bytes[0], addr.addr_bytes[1],
		addr.addr_bytes[2], addr.addr_bytes[3],
		addr.addr_bytes[4], addr.addr_bytes[5]);
	
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		port->mac_addr.addr[i] = addr.addr_bytes[i];
	}

	/* queue init */
	if (!queue_init(&port->tx_queue) || !queue_init(&port->rx_queue))
		return -1;

	/* pthread mutex init */
	pthread_mutex_init(&port->tx_mutex, NULL);
	pthread_mutex_init(&port->rx_mutex, NULL);
	
	/* Enable RX in promiscuous mode for the Ethernet port. */
	rte_eth_promiscuous_enable(nport);
  
	return 0;
}

int
dpdk_init(void){
	int ret;
	unsigned nb_ports;
	uint16_t portid;  
	char *pg_name[] = {"lunetta"};

	ret = rte_eal_init(1, pg_name);
	if (ret < 0) {
		return -1;
	}
  
	nb_ports = rte_eth_dev_count();
	if (nb_ports != 1) {
		return -1;
	}
	
	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
  
	if (mbuf_pool == NULL) {
		return -1;
	}
  
	return 0;
}


void
rx_pkt (struct port_config *port) {
	uint16_t nport = port->port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nb_rx = 0;

	/* Recv burst of RX packets */
	nb_rx = rte_eth_rx_burst(nport, 0, bufs, BURST_SIZE);
	pthread_mutex_lock(&port->rx_mutex);
	for (int i = 0; i < nb_rx ; i++) {
		uint32_t size = rte_pktmbuf_pkt_len(bufs[i]);
		queue_push(&port->rx_queue, bufs[i], size);
	}
	pthread_mutex_unlock(&port->rx_mutex);
}

void
tx_pkt (struct port_config *port) {
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nport = port->port_num;
	int i, j, k;
	uint32_t pop_num = port->tx_queue.num;
	uint16_t nb_tx;
	struct queue_node *ret;

	if (pop_num > 0) {
		pthread_mutex_lock(&port->tx_mutex);
		for (i = 0; i < pop_num; i++) {
			ret = queue_pop(&port->tx_queue);
			bufs[i] = (struct rte_mbuf *)ret->data;
		}
		pthread_mutex_unlock(&port->tx_mutex);
		nb_tx = rte_eth_tx_burst(nport, 0, bufs, 1);
		for (j = nb_tx; j < pop_num; j++) {
			rte_pktmbuf_free(bufs[j]);
		}
	}
	return;
}


/** 1lcore per 1port **/
void lcore_rxtxmain(struct port_config *port) {
	printf("lcore_rxtxmain");
	printf("port->port_num: %u\n", port->port_num);
	while (1) {
		//printf("*");
		rx_pkt(port);
		tx_pkt(port);
	}
}
int launch_lcore_rxtx(void *arg) {
	unsigned lcore_id = rte_lcore_id();
	printf("lcore%u launched\n", lcore_id);

	lcore_rxtxmain((struct port_config *)arg);
	return 0;
}

#ifdef DEBUG_PKT_IO
int main(void) {
	if (lunetta_init() == -1) {
		fprintf(stderr, "lunetta_init error.\n");
    exit(1);
  }

  struct port_config port;
  port.port_num = 0;
  port_init(&port);
  port_setup(&port);
  //arp_init(&port);


	rte_eal_remote_launch(launch_lcore_rxtx, (void *)&port, 1);

	int ret;
	int rx_pop_num;
	/* ethernet */
	while (1) {
		rx_pop_num = port.rx_queue.num;
		for (int i = 0; i < rx_pop_num; i++) {
			pthread_mutex_lock(&port.rx_mutex);
			struct queue_node *node = queue_pop(&port.rx_queue);
			pthread_mutex_unlock(&port.rx_mutex);
			if (!node) {
				printf("!node\n");
				//queue_init(&port.rx_queue);
				//rx_pop_num = 0;
				continue;
			}
			if (node->size < 1512) {
				uint8_t *p = rte_pktmbuf_mtod((struct rte_mbuf *)node->data, uint8_t*);
				rte_hexdump(stdout, "", (const void *)p, node->size);
			}
			rte_pktmbuf_free((struct rte_mbuf *)node->data);
		}
	}
	rte_eal_wait_lcore(1);
}
#endif
