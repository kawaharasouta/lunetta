#ifndef __TCP_H_
#define __TCP_H_

struct tcp_hdr
{
	u_int16_t src_addr;
	u_int16_t dest_addr;
	u_int32_t seq_num;
	u_int32_t ack_num;


//need modification
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	u_int16_t res1:4;
	u_int16_t doff:4;
	u_int16_t fin:1;
	u_int16_t syn:1;
	u_int16_t rst:1;
	u_int16_t psh:1;
	u_int16_t ack:1;
	u_int16_t urg:1;
	u_int16_t res2:2;
#  elif __BYTE_ORDER == __BIG_ENDIAN
	u_int16_t doff:4;
	u_int16_t res1:4;
	u_int16_t res2:2;
	u_int16_t urg:1;
	u_int16_t ack:1;
	u_int16_t psh:1;
	u_int16_t rst:1;
	u_int16_t syn:1;
	u_int16_t fin:1;
#  else
#   error "Adjust your <bits/endian.h> defines"
#  endif
	u_int16_t window;
	u_int16_t check;
	u_int16_t urg_ptr;
};

#endif
