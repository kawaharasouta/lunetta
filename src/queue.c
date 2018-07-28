#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/queue.h"

int allocate_pool_brk(struct queue_info *queue) {
  queue->pool.head = (struct queue_node *)malloc(sizeof(struct queue_node) * QUEUE_SIZE);
	if (!queue->pool.head)	return 0;
  queue->pool.next = allocate_pool.head;
  queue->pool.tail = allocate_pool.head + QUEUE_SIZE - 1;
  queue->pool.num = QUEUE_SIZE;
	return 1;
}

struct queue_node* allocate_queue_node(struct queue_info *queue) {
  struct queue_node *ret = queue->pool.next;

  if (queue->pool.next == queue->pool.tail)
    queue->pool.next = queue->pool.head;
  else
    queue->pool.next++;

  return ret;
}

int queue_init(struct queue_info *queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->num = 0;
  //pthread_mutex_init(&queue->mutex, NULL);

  //rx_queue.head = NULL;
  //rx_queue.tail = NULL;
  //rx_queue.num = 0;
  //pthread_mutex_init(&rx_queue.mutex, NULL);

  return allocate_pool_brk(queue);
}

void queue_push(struct queue_info *queue, void *data, uint32_t size) {
	if (data == NULL)
    return;

  //pthread_mutex_lock(&tx_queue.mutex);
  queue->num += 1;
  //struct pkt_queue *pkt = (struct pkt_queue *)malloc(sizeof(struct pkt_queue));
  struct queue_node *node = allocate_queue_node(queue);
  node->data = data;
  //!it is really wrong
  node->size = size;
  node->next = NULL;

  if (!queue->head){
   queue->head = node;
   queue->tail = node;
    return;
  }
  queue->tail->next = node;
  queue->tail = node;

  //pthread_mutex_unlock(&tx_queue.mutex);
  return;
}

struct queue_node* queue_pop(struct queue_info *queue) {
	if (queue->head == NULL) 
		return NULL;
	
	//pthread_mutex_lock(&rx_queue.mutex);
	queue->num -= 1;
	//void *ret;// = (struct rte_mbuf *)malloc(sizeof(struct rte_mbuf *));
	struct queue_node *ret = queue->head; 
	//ret = rx_queue.head->data;
	//*size = rx_queue.head->size;
	//struct pkt_queue *dust = rx_queue.head;
	queue->head = queue->head->next;
	//free((struct pkt_queue *)dust);
	//if head is NULL, tail can be anything
	//pthread_mutex_unlock(&rx_queue.mutex);
	return ret;
}



#ifdef QUEUE_DEBUG

int main(void) {
	struct queue_info queue;

	printf("*********\n");
	if (!queue_init(&queue))
		printf("queue_init error no tumori ha-to\n");


	int a = 0;
	int b = 1;
	//
	printf("*******\n");
	queue_push(&queue, &a, sizeof(a));



	return 0;
}

#endif
