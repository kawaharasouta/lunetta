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


	

	return 0;
}
