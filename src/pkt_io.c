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
#include<pthread.h>

#include"include/pkt_io.h"

#if 0
struct queue_info{
	int num;
	struct pkt_queue *head;
	struct pkt_queue *tail;
	pthread_mutex_t mutex;
};
struct pkt_queue {
	struct rte_mbuf *mbuf;
	uint32_t size;
	struct pkt_queue *next;
};
#endif

struct queue_info tx_queue;
struct queue_info rx_queue;

/* Memory management. allocate_pool is not necessary to be global. */
struct allocate_pool {
	uint16_t num;
	//struct pkt_queue *head;
	struct pkt_queue *head[1000];
} allocate_pool;
void allocate_pool_brk() {
	//allocate_pool.head = (struct pkt_queue *)malloc(sizeof(struct pkt_queue) * 2);

	//struct pkt_queue *pool = (struct pkt_queue *)malloc(sizeof(struct pkt_queue) * 2);
	//*allocate_pool.head = pool;

	//allocate_pool.head = (struct pkt_queue **)malloc(sizeof(struct pkt_queue *) * 1000);
	for (int i = 0; i < 1000; i++){
		allocate_pool.head[i] = (struct pkt_queue *)malloc(sizeof(struct pkt_queue *));
	}
	allocate_pool.num = 1000;
}
struct pkt_queue* allocate_pkt_queue() {
	if (allocate_pool.num <= 0) {
		allocate_pool_brk();
	}
#if 1
	//struct pkt_queue *ret = *allocate_pool.head;
	//allocate_pool.head += sizeof(struct allocate_pool *);
	//allocate_pool.head++;
	//return ret;
	allocate_pool.num--;
	return allocate_pool.head[/*(500 - 1) - */allocate_pool.num];
#else
	uint16_t index = allocate_pool.num;
	allocate_pool.num -= 1;
	return &allocate_pool.head[index];
#endif
}
/*************************************/
void queue_init() {
	tx_queue.head = NULL;
	tx_queue.tail = NULL;
	tx_queue.num = 0;
	pthread_mutex_init(&tx_queue.mutex, NULL);

	rx_queue.head = NULL;
	rx_queue.tail = NULL;
	rx_queue.num = 0;

	pthread_mutex_init(&rx_queue.mutex, NULL);

	allocate_pool_brk();
}

void tx_queue_push(struct rte_mbuf *mbuf, uint32_t size) {
	if (mbuf == NULL)
		return;

	//pthread_mutex_lock(&tx_queue.mutex);
	tx_queue.num += 1;
	struct pkt_queue *pkt = (struct pkt_queue *)malloc(sizeof(struct pkt_queue));
	//struct pkt_queue *pkt = allocate_pkt_queue();
	pkt->mbuf = mbuf;
	//!it is really wrong
	pkt->size = size;
	pkt->next = NULL;
#if 1
	if (!tx_queue.head){
		tx_queue.head = pkt;
		tx_queue.tail = pkt;
		return;
	}
	tx_queue.tail->next = pkt;
	tx_queue.tail = pkt;
#else
	if (rx_queue.tail) {
    rx_queue.tail->next = pkt;
  }
  rx_queue.tail = pkt;
  if (!rx_queue.head){
    rx_queue.head = pkt;
  }
#endif

	//pthread_mutex_unlock(&tx_queue.mutex);
	return;
}

struct rte_mbuf* tx_queue_pop() {
	if (tx_queue.head == NULL) {
		return NULL;
	}
	//pthread_mutex_lock(&tx_queue.mutex);

	tx_queue.num -= 1;
	struct rte_mbuf *ret;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
	ret = tx_queue.head->mbuf;
	struct pkt_queue *dust = tx_queue.head;
	tx_queue.head = tx_queue.head->next;
	free((struct pkt_queue *)dust);
	//if head is NULL, tail can be anything
	//pthread_mutex_unlock(&tx_queue.mutex);
	return ret;
}

void rx_queue_push(struct rte_mbuf *mbuf, uint32_t size) {
	if (mbuf == NULL)
		return;

	//pthread_mutex_lock(&rx_queue.mutex);
	rx_queue.num += 1;
	struct pkt_queue *pkt = (struct pkt_queue *)malloc(sizeof(struct pkt_queue));
	//struct pkt_queue *pkt = allocate_pkt_queue();
	pkt->mbuf = mbuf;
	pkt->size = size;
	pkt->next = NULL;
#if 1
	if (!rx_queue.head){
		rx_queue.head = pkt;
		rx_queue.tail = pkt;
		return;
	}

	rx_queue.tail->next = pkt;
	rx_queue.tail = pkt;
#else
	if (rx_queue.tail) {
    rx_queue.tail->next = pkt;
  }
  rx_queue.tail = pkt;
  if (!rx_queue.head){
    rx_queue.head = pkt;
  }
#endif
	//pthread_mutex_unlock(&rx_queue.mutex);
	return;
}

struct rte_mbuf* rx_queue_pop(uint32_t *size) {
	if (rx_queue.head == NULL) {
		return NULL;
	}

	//pthread_mutex_lock(&rx_queue.mutex);
	rx_queue.num -= 1;
	struct rte_mbuf *ret;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
	ret = rx_queue.head->mbuf;
	*size = rx_queue.head->size;
	struct pkt_queue *dust = rx_queue.head;
	rx_queue.head = rx_queue.head->next;
	free((struct pkt_queue *)dust);
	//if head is NULL, tail can be anything
	//pthread_mutex_unlock(&rx_queue.mutex);
	return ret;
}

#if 0
struct port {
  uint8_t port_num;
}
#endif

struct rte_mempool *mbuf_pool;

static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};


/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static int
port_init(uint16_t port)
{ 
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;
	struct ether_addr addr;
	
	if (port >= rte_eth_dev_count()) {
		return -1;
	}
	
	/* Configure the Ethernet port. */ 
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0) {
		return retval;
	}
	
	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0) {
		return retval;
	}
	
	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd, rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd, rte_eth_dev_socket_id(port), NULL);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0) {
		return retval;
	}
  
	/* Display the port MAC address. */
	rte_eth_macaddr_get(port, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
		port,
		addr.addr_bytes[0], addr.addr_bytes[1],
		addr.addr_bytes[2], addr.addr_bytes[3],
		addr.addr_bytes[4], addr.addr_bytes[5]);
	
	/* Enable RX in promiscuous mode for the Ethernet port. */
	rte_eth_promiscuous_enable(port);
  
	return 0;
}

int
dpdk_init(void){
	int ret;
	unsigned nb_ports;
	uint16_t portid;  
	char *pg_name[] = {"kkk"};

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
	queue_init();
  
	return 0;
}


/* Wrapped a function to control port but not practically meaningful. It is expected that name will be assigned port number. */
struct port 
*port_open (const uint8_t num) {
	struct port *port;
	if ((port = malloc(sizeof(struct port))) == NULL) {
		perror("malloc");
		return NULL;
	}
	port->port_num = num;
	
	if (port_init(port->port_num) < 0){
		free(port);
		return NULL;
	}
  
	return port;
}

void 
port_close (struct port *port) {
	printf("close port %u\n", port->port_num);
	rte_eth_dev_stop(port->port_num);
	rte_eth_dev_close(port->port_num);
	free(port);
}

void
rx_pkt (struct port *port) {
	//uint16_t nb_ports;
	uint16_t nport = port->port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nb_rx = 0;
	//uint8_t  *p;

	/* Recv burst of RX packets */
	nb_rx = rte_eth_rx_burst(nport, 0, bufs, BURST_SIZE);
	for (int i = 0; i < nb_rx ; i++) {
		//uint8_t *p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
		uint32_t size = rte_pktmbuf_pkt_len(bufs[i]);
		//p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
		//rte_hexdump(stdout, "", (const void *)p, size);
		rx_queue_push(bufs[i], size);
	}
}

size_t
tx_pkt (struct port *port) {
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nport = port->port_num;
	int i, j, k;
	uint32_t pop_num = tx_queue.num;
	uint16_t nb_tx;
	//uint8_t *p;

	if (pop_num > 0) {
		for (i = 0; i < pop_num; i++) {
			bufs[i] = tx_queue_pop();
		}
		nb_tx = rte_eth_tx_burst(nport, 0, bufs, 1);
		for (j = 0; j < pop_num; j++) {
			rte_pktmbuf_free(bufs[j]);
		}
	}
 
//	for (i = 0; i < BURST_SIZE; i++){
//		//bufs[i] = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
//		bufs[i] = tx_queue_pop();
//		if (bufs[i] == NULL){
//			break;
//		}
//		//p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
//		uint32_t size = rte_pktmbuf_pkt_len(bufs[i]);
//		//rte_hexdump(stdout, "", (const void *)p, size);
//
//	}
	/* Send burst of TX packets */
	//bufs[0] = rte_pktmbuf_alloc(mbuf_pool);
	//bufs[0]->pkt_len = length;
	//bufs[0]->data_len = length;
	//bufs[0]->port = nport;
	//bufs[0]->packet_type = 1;
  //
	//p = rte_pktmbuf_mtod(bufs[0], uint8_t*);
	//memcpy(p, buffer, length);
  
#if 0
	if (num_tx < i){
		for (uint16_t j = num_tx; j < i; j++){
			rte_pktmbuf_free(bufs[j]);
		}
	}
#endif
	//if (num_tx > 0) {
	//	return num_tx;
	//}
	//return -1;
	return 0;
}


//static/* __attribute__((noreturn))*/ void
void lcore_txmain(struct port *port) {
	while (1) {
		tx_pkt(port);
	}
}
void lcore_rxmain(struct port *port) {
	while (1) {
		rx_pkt(port);
	}
}

/*static */int launch_lcore_tx(/*__attribute__ ((unused)) */void *arg) {
	unsigned lcore_id = rte_lcore_id();
	printf("lcore%u launched\n",lcore_id);

	lcore_txmain((struct port *)arg);
	return 0;
}
/*static */int launch_lcore_rx(/*__attribute__ ((unused)) */void *arg) {
	unsigned lcore_id = rte_lcore_id();
	printf("lcore%u launched\n",lcore_id);

	lcore_rxmain((struct port *)arg);
	return 0;
}

/** 1lcore per 1port **/
void lcore_rxtxmain(struct port *port) {
	printf("lcore_rxtxmain");
	printf("port->port_num: %u\n", port->port_num);
	while (1) {
		sleep(1);
		rx_pkt(port);
		//sleep(1);
		tx_pkt(port);
	}
}
int launch_lcore_rxtx(void *arg) {
	unsigned lcore_id = rte_lcore_id();
	printf("lcore%u launched\n", lcore_id);

	lcore_rxtxmain((struct port *)arg);
	return 0;
}


/* test pthread */
void *rxtx_thread(void *arg) {
	while (1) {
		sleep(1);
		rx_pkt((struct port *)arg);
		//sleep(1);
		tx_pkt((struct port *)arg);
	}
}


#if 1
int main() {
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
		printf("***\n");
		int j;
		uint32_t pop_size;
		int rx_pop_num;
		//pthread_mutex_lock(&tx_queue.mutex);
		//pthread_mutex_lock(&rx_queue.mutex);
		rx_pop_num = rx_queue.num;
		if (rx_pop_num > 0) {
			printf("rx_pop_num > 0\n");
		}
		for (j = 0; j < rx_pop_num; j++){
			struct rte_mbuf *mbuf;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
			mbuf = rx_queue_pop(&pop_size);
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

#else
int main() {
	dpdk_init();

	struct port *port;
	port = port_open(0);

	uint16_t nb_ports;
	uint16_t nport = port->port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	struct rte_mbuf *tbufs[BURST_SIZE];
	uint16_t nb_rx;
	uint16_t nb_tx;
	//uint8_t *p;
#if 1
	pthread_t thread;

	pthread_create(&thread, NULL, rxtx_thread, (void *)port);

	while (1) {
		printf("***\n");
		sleep(1);
		int j;
		uint32_t pop_size;
		//pthread_mutex_lock(&rx_queue.mutex);
		int rx_pop_num = rx_queue.num;
		for (j = 0; j < rx_pop_num; j++){
			struct rte_mbuf *mbuf;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
			mbuf = rx_queue_pop(&pop_size);
			uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
			//uint32_t size = rte_pktmbuf_pkt_len(bufs[j]);
			rte_hexdump(stdout, "", (const void *)p, pop_size);
			
			//pthread_mutex_lock(&tx_queue.mutex);
			tx_queue_push(mbuf, pop_size);
			//pthread_mutex_unlock(&tx_queue.mutex);
		}
		//pthread_mutex_unlock(&rx_queue.mutex);

	}

#else
	for (;;){
#if 1
		rx_pkt(port);
#else
		nb_rx = 0;
		/* Recv burst of RX packets */
		nb_rx = rte_eth_rx_burst(nport, 0, bufs, BURST_SIZE);
		int i, j, k;
		uint32_t pop_size;
		for (i = 0; i < nb_rx ; i++) {
			uint8_t *pp = rte_pktmbuf_mtod(bufs[i], uint8_t*);
			uint32_t size = rte_pktmbuf_pkt_len(bufs[i]);

			//rte_hexdump(stdout, "", (const void *)pp, size);
			rx_queue_push(bufs[i], size);
		}
#endif
		//if (nb_rx > 0)
			//printf("-----------\n");
		//}
		int j;
		uint32_t pop_size;
		int rx_pop_num = rx_queue.num;
		for (j = 0; j < rx_pop_num; j++){
			struct rte_mbuf *mbuf;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
			mbuf = rx_queue_pop(&pop_size);
			uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
			uint32_t size = rte_pktmbuf_pkt_len(bufs[j]);
			//rte_hexdump(stdout, "", (const void *)p, pop_size);
	//		p = NULL;

			tx_queue_push(mbuf, pop_size);
//
//			//struct rte_mbuf **tx_mbuf = (struct rte_mbuf **)malloc(sizeof(struct rte_mbuf *) * );
//			struct rte_mbuf *tx_mbuf;
//			//tx_mbuf[0] = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
//			tx_mbuf = tx_queue_pop();
//			nb_tx = rte_eth_tx_burst(nport, 0, &tx_mbuf, 1);
//
//			rte_pktmbuf_free(mbuf);
//			rte_pktmbuf_free(tx_mbuf);
		}
		//if (nb_rx > 0)
			//printf("==========\n");
#if 1
		tx_pkt(port);
#else
		int tx_pop_num = tx_queue.num;
		if (tx_pop_num > 0) {
			for (k = 0; k < tx_pop_num; k++){
				tbufs[k] = tx_queue_pop();
			}
			nb_tx = rte_eth_tx_burst(nport, 0, tbufs, 1);
			for (int l = 0; l < tx_pop_num; l++){
				rte_pktmbuf_free(tbufs[l]);
			}
#endif
//			if ((tx_pop_num > nb_tx)){
//				for (int t = nb_tx; t < tx_pop_num; t++){
//					rte_pktmbuf_free(bufs[j]);
//				}
//			}
//		}


		//rx_pkt(port);
		//tx_pkt(port);
	}
#endif
	pthread_join(thread, NULL);
	return 0;
}
#endif
