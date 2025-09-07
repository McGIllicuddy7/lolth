#pragma once
#include <stdio.h>
typedef long TaskHandle;
void yield();
TaskHandle spawn(void (*to_call)(void*),void * args);
void lolth_init();
