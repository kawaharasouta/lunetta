#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/lunetta_mbuf.h"

//struct mbuf {
//	uint16_t pkt_len;
//	uint16_t front_len;
//	uint16_t back_len;
//	uint8_t front[64];
//	uint8_t packet[64];
//	uint8_t back[64];
//	uint8_t *head;
//};


void init_mbuf(struct mbuf *mbuf) {
	mbuf->pkt_len = 0;//?
	mbuf->front_len = 64;
	mbuf->back_len = 64;

	//mbuf->front = (uint8_t *)malloc(sizeof(uint8_t) * ROOM_SIZE);
	//mbuf->packet = mbuf->front + (ROOM_SIZE/3);
	//mbuf->back = mbuf->packet + (ROOM_SIZE/3);

	mbuf->head = mbuf->packet;
	mbuf->tail = mbuf->packet;

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
	if (mbuf->back_len < size) {
		fprintf(stderr, "mbuf_extend_forward error\n");
		exit(1);
	}
	mbuf->back_len -= size;
	tail += size;

	return;
}


