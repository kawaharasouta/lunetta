#ifndef __ETHERNET_H_
#define __ETHERNET_H_


#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_RARP 0x8035

#define ETHER_ADDR_LEN 6


typedef struct {
	uint8_t addr[ETHER_ADDR_LEN];
} __attribute__ ((packed)) ethernet_addr;

struct ethernet_hdr {
	ethernet_addr dest;
	ethernet_addr src;
	uint16_t type;
} __attribute__ ((packed));


void print_mac_addr(ethernet_addr *addr);
int equal_mac_addr(ethernet_addr *addr1, ethernet_addr *addr2);
int is_ether_broadcast(ethernet_addr *addr);
void rx_ether(struct_port_config);

#endif
