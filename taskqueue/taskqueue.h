/*
 * taskqueue.h
 *
 *  Created on: 2018年11月27日
 *      Author: jackiesjq
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <pthread.h>
#include "../queue/queue.h"

/*
typedef enum task_type{

} TaskType;

typedef struct task {

} Task;
*/

typedef struct task_queue {
	Queue * pqueue;
	pthread_mutex_t mutex;
	pthread_cond_t not_empty;
	pthread_cond_t not_full;
	size_t count;
} TaskQueue;

void put_task(TaskQueue * p_taskqueue, void * ptask);

void * take_task(TaskQueue * p_taskqueue);

TaskQueue * task_queue_make();

void task_queue_free(TaskQueue * pTaskQueue);

#endif /* TASKQUEUE_H_ */
