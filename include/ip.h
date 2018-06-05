#ifndef __IP_H_
#define __IP_H_

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
	u_int8_t type_of_service;
	u_int16_t total_len;
	u_int16_t id;
	u_int16_t frag;
	u_int8_t ttl;
	u_int8_t proto;
	u_int16_t check;
	u_int32_t src_addr;
	u_int32_t dest_addr;
}

#endif
