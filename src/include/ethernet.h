#ifndef __ETHERNET_H_
#define __ETHERNET_H_

struct ether_hdr {
	u_int8_t ether_dest_addr[6];
	u_int8_t ether_src_addr[6];
	u_int16_t type;
}

#endif
