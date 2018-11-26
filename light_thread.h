#include "queue.h"
#include <ucontext.h>

#define TRUE 1
#define FALSE 0

typedef void (*coroutine_func)();

typedef struct light_thread_task LightThreadTask;

typedef struct schedule {
    size_t stack_size;
    Queue * pExecuteQueue;
    ucontext_t * pcontext;
    LightThreadTask * p_curLightThreadTask;
    int yield;
    ucontext_t main_context;
} Schdule;


typedef struct light_thread_task {
        ucontext_t context;
        coroutine_func co_func;
        char * stack;
} LightThreadTask;

void schedule_init();

void schedule_start();

void light_thread_create(coroutine_func co_func);

int light_thread_yield();

char * make_stack();

void free_stack(char * );

Schdule * PSchdule;
