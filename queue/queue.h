#include <stdlib.h>

typedef struct queue_node {
	struct queue_node * p_next;
	struct queue_node * p_prev;
	void * p_value;
} QueueNode;

typedef struct queue {
	QueueNode * head;
	QueueNode * tail;
} Queue;

void queue_push(Queue * p_queue, void * p_value);

void * queue_pop(Queue * p_queue);

Queue * queue_new();

void queue_free(Queue * pQueue);
