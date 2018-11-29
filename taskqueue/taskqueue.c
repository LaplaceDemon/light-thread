/*
 * taskqueue.c
 *
 *  Created on: 2018年11月27日
 *      Author: jackiesjq
 */


#include "taskqueue.h"
#include <stdlib.h>

TaskQueue * task_queue_make() {
	TaskQueue *pTaskQueue = (TaskQueue*)malloc(sizeof(TaskQueue));
	pTaskQueue -> pqueue = queue_new();
	pthread_mutex_init(&(pTaskQueue->mutex), NULL);
	pthread_cond_init(&(pTaskQueue->not_empty), NULL);
	pthread_cond_init(&(pTaskQueue->not_full), NULL);
	return pTaskQueue;
}

void put_task(TaskQueue * p_taskqueue, void * ptask) {
	pthread_mutex_lock(&(p_taskqueue->mutex));
	queue_push(p_taskqueue->pqueue, ptask);
	p_taskqueue->count++;
	pthread_cond_signal(&(p_taskqueue->not_empty));
	pthread_mutex_unlock(&(p_taskqueue->mutex));
}

void * take_task(TaskQueue * p_taskqueue) {
	void * ptask;
	pthread_mutex_lock(&(p_taskqueue->mutex));
	while(p_taskqueue->count == 0) {
		pthread_cond_wait(&(p_taskqueue->not_empty) ,&(p_taskqueue->mutex));
	}
	ptask = queue_pop(p_taskqueue->pqueue);
	p_taskqueue->count--;
	pthread_mutex_unlock(&(p_taskqueue->mutex));
	return ptask;
}

void task_queue_free(TaskQueue * pTaskQueue) {
	pthread_cond_destroy(&(pTaskQueue->not_empty));
	pthread_cond_destroy(&(pTaskQueue->not_full));
	pthread_mutex_destroy(&(pTaskQueue->mutex));
	queue_free(pTaskQueue->pqueue);
	pTaskQueue->count = 0;
	free(pTaskQueue);
}
