#include "stream.h"
#include <stdlib.h>
#include <string.h>
Stream stream_create(void){
	Stream out;
	out.allocation = 0;
	out.capacity = 0;
	out.end = 0;
	out.head = 0;
	return out;
}

size_t stream_write(Stream * stream, void * ptr, size_t count){
	if(stream->capacity == 0){
		stream->allocation = malloc(1024);
		stream->capacity = 1024;		
		stream->head = 0;
		stream->end =0;
	}
	if(stream->head+count>= stream->capacity){
	grow_buffer:
		stream->capacity *= 2;	
		if(stream->head+count>= stream->capacity){
			goto grow_buffer;
		}
		stream->allocation = realloc(stream->allocation, stream->capacity);
		if(stream->allocation == 0){
			return 0;
		}
	}
	char * p = ptr;
	if(!p){
		return 0;
	}
	if(stream->head+count>= stream->end){
		stream->end = stream->head+count;
	}
	memcpy(stream->allocation+stream->head, ptr, count);
	stream->head += count;
	return count;
}
size_t stream_read(Stream * stream, void * ptr, size_t count){
	size_t to_read = 0;
	if(stream->head+count>= stream->end){
		to_read = stream->end-stream->head;
	}else{
		to_read = count;
	}
	memcpy(ptr, stream->allocation+stream->head,to_read);
	stream->head+= to_read;
	return to_read;
}

size_t stream_seek(Stream * stream, int offset, SeekMode mode){
	int64_t ptr = stream->head;
	if(mode == SEEK_FROM_CURRENT){
		ptr = stream->head+offset;
	}else if(mode == SEEK_FROM_END){
		ptr = stream->end+offset;
	}else if(mode == SEEK_FROM_START){
		ptr = offset;
	}
	if(ptr>= 0 && ptr< (int64_t)(stream->end)){
		stream->head = ptr;
	}
	return stream->head;
}
size_t stream_size(Stream * stream){
	return stream->end;
}
size_t stream_remaining(Stream * stream){
	if(stream->head>stream->end){
		return 0;
	}
	return stream->end-stream->head;
}
void stream_destroy(Stream * stream){
	if(stream->allocation){
		free(stream->allocation);
	}
	stream->capacity = 0;
	stream->end = 0;
	stream->head = 0;
	stream->allocation= 0;
}
void stream_write_to_file(Stream * stream, FILE * file){
	fwrite(stream->allocation, 1, stream->end, file);
}
void stream_write_to_file_as_hex(Stream * stream, FILE * file){
	for(size_t i =0; i<stream->end; i++){
		fprintf(file, "%x ",(int)stream->allocation[i]);	
	}
}
size_t stream_write_u64(Stream * stream, uint64_t u){
	return stream_write(stream, &u, sizeof(u));
}

uint64_t stream_read_u64(Stream * stream){
	uint64_t u =  0;
	stream_read(stream, &u, sizeof(u));
	return u;
}
Stream stream_from_file(const char * path){
	Stream out;
	FILE * f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	int l = ftell(f);
	char * ptr = malloc(l);
	fseek(f, 0,SEEK_SET);
	fread(ptr, 1, l, f);
	fclose(f);
	out.allocation = ptr;
	out.capacity = l;
	out.end = l;
	out.head = 0;
	return out;
}

