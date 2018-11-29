#include "light_thread.h"

#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <pthread.h>

void schedule_init(size_t stack_size) {
	TaskQueue * pTaskQueue = task_queue_make();
	PSchdule = (Schdule *) malloc(sizeof(Schdule));
	PSchdule->pExecuteQueue = pTaskQueue;
	PSchdule->pcontext = NULL;
	PSchdule->stack_size = stack_size;
}

void light_thread_create(coroutine_func co_func, void * arg) {
	LightThreadTask * p_light_thread_task = (LightThreadTask *) malloc(sizeof(LightThreadTask));

	// 上下文设置
	getcontext(&p_light_thread_task->context);
	p_light_thread_task->context.uc_stack.ss_sp = make_stack();
	p_light_thread_task->context.uc_stack.ss_size = PSchdule->stack_size;
	p_light_thread_task->context.uc_stack.ss_flags = 0;

	// 执行完后调回
	p_light_thread_task->context.uc_link = &(PSchdule->main_context);

	// 记录指针（可能可以注释掉）
	p_light_thread_task->co_func = co_func;

	// 设置下一条执行函数
	if (arg != NULL) {
		makecontext(&(p_light_thread_task->context), (void (*)(void)) (p_light_thread_task->co_func), 1, arg);
	} else {
		makecontext(&(p_light_thread_task->context), (void (*)(void)) (p_light_thread_task->co_func), 0, NULL);
	}

	// 压入运行队列
	light_thread_resume(p_light_thread_task);
}

int light_thread_yield() {
	PSchdule->yield = TRUE;
	// 临时退出
	int ret = swapcontext(&(PSchdule->p_curLightThreadTask->context), &(PSchdule->main_context));
	return ret;
}

int light_thread_wait() {
	PSchdule->yield = FALSE;
	// 临时退出
	int ret = swapcontext(&(PSchdule->p_curLightThreadTask->context), &(PSchdule->main_context));
	return ret;
}

void light_thread_resume(LightThreadTask * pLightThreadTask) {
	put_task(PSchdule->pExecuteQueue, (void *) pLightThreadTask);
}

void schedule_start() {
	while (1) {
		// 获取上下文
		getcontext(&(PSchdule->main_context));
		if (PSchdule->yield) {
			// 压入队列
			// queue_push(PSchdule->pExecuteQueue, (void *) PSchdule->p_curLightThreadTask);
			put_task(PSchdule->pExecuteQueue, (Task *) (PSchdule->p_curLightThreadTask));
		}

		// get task from queue
		LightThreadTask * pTask = (LightThreadTask *) take_task(PSchdule->pExecuteQueue);
		if (pTask != NULL) {
			// 切换进入pTask
			// 保存上下文
			// PSchdule->pcontext = &(pTask->context);
			PSchdule->p_curLightThreadTask = pTask;
			PSchdule->yield = FALSE;
			// 开始执行上下文
			setcontext(&pTask->context);
		} else {
			return;
		}
	}
}

void* __schedule_start(void * arg) {
	schedule_start();
	return NULL;
}

int schedule_start_in_thread() {
	pthread_t thread;
	int ret;
	ret = pthread_create(&thread, NULL, __schedule_start, NULL);
	if (ret < 0) {
		return ret;
	}

	ret = pthread_detach(thread);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

char * make_stack() {
	return (char *) malloc(PSchdule->stack_size);
}

void free_stack(char * p) {
	free(p);
}

