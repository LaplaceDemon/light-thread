#include "queue.h"

void queue_push(Queue * p_queue, void * p_value) {
    QueueNode * pNewQueueNode = (QueueNode *)malloc(sizeof(QueueNode));
    pNewQueueNode->p_value = p_value;

    if(p_queue -> head == NULL && p_queue -> tail == NULL) {
        p_queue -> head = pNewQueueNode;
        p_queue -> tail = pNewQueueNode;
        return ;
    }

    pNewQueueNode -> p_next = NULL;
    QueueNode * pTailNode = p_queue -> tail;
    pTailNode -> p_next = pNewQueueNode;
    pNewQueueNode -> p_prev = pTailNode;
    p_queue -> tail = pNewQueueNode;
}

void * queue_pop(Queue * p_queue) {
	QueueNode * pHeadNode = p_queue->head;
    if(pHeadNode == NULL) {
        return NULL;
    }

    void * p_value = pHeadNode->p_value;

    if(p_queue->head == p_queue->tail) {
    	p_queue -> head = NULL;
    	p_queue -> tail = NULL;
    } else {
    	QueueNode * secondNode = pHeadNode->p_next;
    	p_queue->head = secondNode;
    	secondNode->p_prev = NULL;
    }

    free(pHeadNode);
    return p_value;
}

Queue * queue_new() {
    Queue * pQueue = (Queue *)malloc(sizeof(Queue));
    return pQueue;
}

void queue_free(Queue * pQueue) {
    free(pQueue);
}
