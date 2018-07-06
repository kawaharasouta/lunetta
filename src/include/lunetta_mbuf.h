#ifndef __MBUF_H_
#define __MBUF_H_

#define ROOM_SIZE 64*3

struct mbuf {
	uint16_t pkt_len;
	uint16_t front_len;
	uint16_t tail_len;
	uint8_t front[64];
	uint8_t packet[64];
	uint8_t tail[64];
	uint8_t *head;
};

void init_mbuf(struct mbuf *mbuf);
uint8_t* get_packet(struct mbuf *mbuf);
void mbuf_extend_forwards(struct mbuf *mbuf, uint16_t size);
void mbuf_extend_backwards(struct mbuf *mbuf, uint16_t size);

#endif



