#include "taskqueue/taskqueue.h"
#include <ucontext.h>

#define TRUE 1
#define FALSE 0

typedef void (*coroutine_func)(void *);

typedef struct light_thread_task LightThreadTask;

typedef struct schedule {
    size_t stack_size;
    TaskQueue * pExecuteQueue;
    ucontext_t * pcontext;
    LightThreadTask * p_curLightThreadTask;
    int yield;
    ucontext_t main_context;
} Schdule;

typedef struct light_thread_task {
        ucontext_t context;
        coroutine_func co_func;
        char * stack;
} LightThreadTask,Task;

void schedule_init();

void schedule_start();

int schedule_start_in_thread();

void light_thread_create(coroutine_func co_func, void * arg);

int light_thread_yield();

int light_thread_wait();

void light_thread_resume(LightThreadTask * pLightThreadTask);

char * make_stack();

void free_stack(char *);

Schdule * PSchdule;
