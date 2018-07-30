#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/queue.h"

int allocate_pool_brk(struct queue_info *queue) {
  queue->pool.head = (struct queue_node *)malloc(sizeof(struct queue_node) * QUEUE_SIZE);
	if (!queue->pool.head)	return 0;
  queue->pool.next = queue->pool.head;
  queue->pool.tail = queue->pool.head + QUEUE_SIZE - 1;
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
  return allocate_pool_brk(queue);
}

void queue_push(struct queue_info *queue, void *data, uint32_t size) {
	if (data == NULL)
    return;

  queue->num += 1;
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

  return;
}

struct queue_node* queue_pop(struct queue_info *queue) {
	if (queue->head == NULL) 
		return NULL;
	
	queue->num -= 1;
	struct queue_node *ret = queue->head; 
	queue->head = queue->head->next;
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
	queue_push(&queue, &a, sizeof(a));
	queue_push(&queue, &b, sizeof(b));
	struct queue_node *ret;

	ret = queue_pop(&queue);
	printf("ret: %d\n", *(int *)ret->data);
	ret = queue_pop(&queue);
	printf("ret: %d\n", *(int *)ret->data);

	ret = queue_pop(&queue);
	if (!ret)
		printf("nothing\n");

	for (int i = 0; i < 100; i++) {
		int *var;// = i;
		var = (int *)malloc(sizeof(int));
		*var = i;
//		printf("%p\n", var);
		queue_push(&queue, var, sizeof(int));
	}
	for (int i = 0; i < 100; i++) {
		struct queue_node *node = queue_pop(&queue);
		printf("%d\n", *(int *)node->data);
	}

	return 0;
}

#endif
