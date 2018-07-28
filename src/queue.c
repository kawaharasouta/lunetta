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
#if 1
  if (!tx_queue.head){
    tx_queue.head = pkt;
    tx_queue.tail = pkt;
    return;
  }
  tx_queue.tail->next = pkt;
  tx_queue.tail = pkt;
#else
  if (rx_queue.tail) {
    rx_queue.tail->next = pkt;
  }
  rx_queue.tail = pkt;
  if (!rx_queue.head){
    rx_queue.head = pkt;
  }
#endif

  //pthread_mutex_unlock(&tx_queue.mutex);
  return;
}




#ifdef QUEUE_DEBUG

int main(void) {
	struct queue_info queue;

	if (!queue_init(&queue))
		printf("queue_init error no tumori ha-to\n");
}

#endif
