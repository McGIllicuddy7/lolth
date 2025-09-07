#pragma once
#include <stdio.h>
#include <stdbool.h>
typedef long TaskHandle;
void yield();
TaskHandle spawn(void (*to_call)(void*),void * args);
void lolth_init();
void lolth_await(TaskHandle handle);
