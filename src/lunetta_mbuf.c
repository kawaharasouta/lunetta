#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/lunetta_mbuf.h"

//struct mbuf {
//	uint16_t pkt_len;
//	uint16_t front_len;
//	uint16_t tail_len;
//	uint8_t front[64];
//	uint8_t packet[64];
//	uint8_t tail[64];
//	uint8_t *head;
//};


void init_mbuf(struct mbuf *mbuf) {
	mbuf->pkt_len = 0;//?
	mbuf->front_len = 64;
	mbuf->tail_len = 64;

	//mbuf->front = (uint8_t *)malloc(sizeof(uint8_t) * ROOM_SIZE);
	//mbuf->packet = mbuf->front + (ROOM_SIZE/3);
	//mbuf->tail = mbuf->packet + (ROOM_SIZE/3);

	mbuf->head = mbuf->packet;

	return;
}

uint8_t *get_packet(struct mbuf *mbuf) {
	return /*(uint8_t *)*/mbuf->head;
}

void mbuf_extend_forwards(struct mbuf *mbuf, uint16_t size) {
	if (mbuf->front_len < size) {
		fprintf(stderr, "mbuf_extend_forward error\n");
		exit(1);
	}
	mbuf->front_len -= size;
	mbuf->head -= size;

	return;
}

void mbuf_extend_backwards(struct mbuf *mbuf, uint16_t size) {
	if (mbuf->tail_len < size) {
		fprintf(stderr, "mbuf_extend_forward error\n");
		exit(1);
	}
	mbuf->tail_len -= size;
	//head += size;

	return;
}



//int main() {
//	struct mbuf mbuf;
//	init_mbuf(&mbuf);
//	printf("%p\n", get_packet(&mbuf));
//
//
//	return 0;
//}
