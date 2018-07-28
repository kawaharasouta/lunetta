#ifndef __QUEUE_H_
#define __QUEUE_H_

#include<pthread.h>

#define QUEUE_SIZE 10000

struct allocate_pool {
	uint16_t num;
	struct queue_node *head;
	struct queue_node *tail;
	struct queue_node *next;
} allocate_pool;

struct queue_info{
	int num;
	struct queue_node *head;
	struct queue_node *tail;
	struct allocate_pool pool;
	//pthread_mutex_t mutex;
};
struct queue_node {
	void *data;
	uint32_t size;
	struct queue_node *next;
};

#endif
