#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
typedef long TaskHandle;
void yield();
TaskHandle spawn(void (*to_call)(void*),void * args);
void lolth_init();
void lolth_finish();
void lolth_await(TaskHandle handle);
size_t lolth_read(FILE * file, char * buf, size_t count);
size_t lolth_write(FILE * file, char * buf, size_t count);
String lolth_read_to_string(Arena * arena,const char* fname);
String lolth_get_line(Arena * arena, FILE * f);
String lolth_get_until(Arena * arena, FILE * f, Str pat);
void lolth_write_to_file(const char * fname, Str to_write);
void lolth_write_str(FILE * file, Str to_write);
