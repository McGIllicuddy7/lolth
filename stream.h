#pragma once
#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
typedef struct {
	 void* ptr;
	 void *(*alloc)(void *, size_t count);
	 void (*dealloc)(void*);
}Allocator;

typedef struct{	
	char * allocation;
	size_t head;
	size_t end;
	size_t capacity;
}Stream;
typedef enum{
	SEEK_FROM_START, 
	SEEK_FROM_END, 
	SEEK_FROM_CURRENT,
}SeekMode;
Stream stream_create(void);
size_t stream_write(Stream * stream, void * ptr, size_t count);
size_t stream_read(Stream * stream, void * ptr, size_t count);
size_t stream_seek(Stream * stream, int offset, SeekMode mode);
size_t stream_size(Stream * stream);
size_t stream_remaining(Stream * stream);	
size_t stream_write_u64(Stream * stream, uint64_t u);
uint64_t stream_read_u64(Stream * stream);
void stream_destroy(Stream * stream);
void stream_write_to_file(Stream * stream, FILE * file);
Stream stream_from_file(const char * path);
void stream_write_to_file_as_hex(Stream * stream, FILE * file);
