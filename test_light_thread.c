/*
 * test_light_thread.c
 *
 *  Created on: 2018年11月26日
 *      Author: jackiesjq
 */


#include "light_thread.h"
#include <stdio.h>

void test_task0() {
    printf("%d\n", 1);
    printf("%d\n", 2);
    printf("%d\n", 3);

    light_thread_yield();

    printf("%d\n", 4);
    printf("%d\n", 5);
    printf("%d\n", 6);
}

void test_task1() {
    printf("%d\n", 11);
    printf("%d\n", 12);
    printf("%d\n", 13);

    light_thread_yield();

    printf("%d\n", 14);
    printf("%d\n", 15);
    printf("%d\n", 16);
}


int main() {
    schedule_init(8192);

    light_thread_create(test_task0);
    light_thread_create(test_task1);

    schedule_start();
    return 0;
}
