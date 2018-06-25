#ifndef __IP_H_
#define __IP_H_

#include<stdint.h>

struct ip_hdr{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int hdr_len:4;
	unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	unsigned int version:4;
	unsigned int hdr_len:4;
#else
# error "Please fix <bits/endian.h>"
#endif
	uint8_t type_of_service;
	uint16_t total_len;
	uint16_t id;
	uint16_t frag;
	uint8_t ttl;
	uint8_t proto;
	uint16_t check;
	uint32_t src_addr;
	uint32_t dest_addr;
}

#endif
