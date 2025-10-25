#include "reflect.h"
#include <assert.h>

int reflect_get_int(Reflection a, int64_t * out){
	if(a.info->info != TypeInt){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 1){
		*out = *(const char*)(a.ptr);
	}else if(a.info->size == 2){
		*out = *(const short*)(a.ptr);
	}else if(a.info->size == 4){
		*out = *(const int*)(a.ptr);
	}else if(a.info->size == 8){
		*out = *(const int64_t*)(a.ptr);
	}else{
		return 1;
	}
	return 0;
}
int reflect_get_uint(Reflection a, uint64_t * out){
	if(a.info->info != TypeUInt){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 1){
		*out = *(const unsigned char*)(a.ptr);
	}else if(a.info->size == 2){
		*out = *(const unsigned short*)(a.ptr);
	}else if(a.info->size == 4){
		*out = *(const unsigned int*)(a.ptr);
	}else if(a.info->size == 8){
		*out = *(const uint64_t*)(a.ptr);
	}else{
		return 1;
	}
	return 0;
}
int reflect_get_float(Reflection a, double* out){
	if(a.info->info != TypeFloat){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 4){
		*out = *(const float*)(a.ptr);
	}else if(a.info->size == 8){
		*out = *(const double*)(a.ptr);
	}else{
		return 1;
	}
	return 0;
}
int reflect_get_string(Reflection a, const char ** out){
	if(a.info->info != TypeString){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	*out = *(const char**)a.ptr;
	return 0;
}
int reflect_get_field(Reflection a,int idx,Reflection * out){
	if(a.info->info != TypeStruct){
		return 1;
	}
	if((size_t)idx>= a.info->field_count){
		return 1;
	}
	if(idx<0){
		return 1;
	}
	char * p = (char*)a.ptr;
	Field info = a.info->composition[idx];
	out->info = info.info;
	out->indirection_count = info.indirection_count;
	out->ptr = p+info.offset;
	out->count = info.count;
	return 0;
}

int reflect_set_int(Reflection a, int64_t out){
	if(a.info->info != TypeInt){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 1){
		 *( char*)(a.ptr) = out;
	}else if(a.info->size == 2){
		 *( short*)(a.ptr) = out;
	}else if(a.info->size == 4){
		*( int*)(a.ptr) = out;
	}else if(a.info->size == 8){
		*( int64_t*)(a.ptr) = out;

	}else{
		return 1;
	}
	return 0;
}
int reflect_set_uint(Reflection a, uint64_t out){
	if(a.info->info != TypeUInt){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 1){
		 *( unsigned char*)(a.ptr) = out;
	}else if(a.info->size == 2){
		 *( unsigned short*)(a.ptr) = out;
	}else if(a.info->size == 4){
		*( unsigned int*)(a.ptr) = out;
	}else if(a.info->size == 8){
		*( uint64_t*)(a.ptr) = out;

	}else{
		return 1;
	}
	return 0;
}
int reflect_set_float(Reflection a, double out){
	if(a.info->info != TypeFloat){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	if(a.info->size == 4){
		*( float*)(a.ptr) = out;
	}else if(a.info->size == 8){
		*( double*)(a.ptr) = out;

	}else{
		return 1;
	}
	return 0;
}
int reflect_set_cstr(Reflection a,const char * ptr){
	if(a.info->info != TypeString){
		return 1;
	}
	*(const char**)a.ptr = ptr;
	return 0;
}
int reflect_set_field(Reflection a, Reflection b, int idx){
	if(a.info->info != TypeStruct){
		return 1;
	}
	if(a.info->field_count>=(size_t)idx){
		return 1;
	}
	if(idx<0){
		return 1;
	}
	char * p = (char*)a.ptr;
	Field info = a.info->composition[idx];
	char * f = p+info.offset;
	if(info.indirection_count != 0){
		memcpy(f, b.ptr, sizeof(void*));
	}else{
		memcpy(f, b.ptr, info.info->size * info.count);
	}
	return 0;
}
int reflect_deref(Reflection a, Reflection * out){
	if(a.indirection_count==0){
		return 1;
	}
	Reflection b = a;
	b.indirection_count -=1;
	b.ptr = *(void**)b.ptr;
	*out = b;
	return 0;
}
void debug_fprint(FILE * file,Reflection ref){
	 Reflection rf = ref;
	 if(rf.indirection_count){	
	 	do{
		 	if(*(void**)rf.ptr == 0){
				fprintf(file, "NULL");
				return; 
	 		}
	 		if(rf.indirection_count>0){
				reflect_deref(rf, &rf);
	 		}
	 	}while(rf.indirection_count>1);	
	 }
	 if(rf.info->info== TypeInt){
		int64_t s = 0;
	 	reflect_get_int(rf, &s);
	 	fprintf(file, "%ld", (long)s);

	 }else if(rf.info->info == TypeUInt){
		uint64_t s = 0;
	 	reflect_get_uint(rf, &s);
	 	fprintf(file, "%lu", (unsigned long)s);

	 }else if(rf.info->info == TypeFloat){
	 	double s = 0.0;
	 	reflect_get_float(rf, &s);
	 	fprintf(file, "%f", s);
	 }else if(rf.info->info == TypeString){
	 	const char * s = 0;
	 	reflect_get_string(rf, &s);
	 	fprintf(file,"\"%s\"", s);
	 }else if(rf.info->info== TypeSlice){
	 	return;
	 }else if(rf.info->info == TypeStruct){
		fprintf(file,"%s:{", rf.info->name);
	 	for(size_t i =0; i<rf.info->field_count; i++){
	 		fprintf(file,"%s = ", rf.info->composition[i].name);
	 		Reflection r;
	 		reflect_get_field(rf, i, &r);
	 		debug_fprint(file, r);
	 		if(i != rf.info->field_count-1){
	 			fprintf(file, ",");
	 		}
		
	 	}
	 	fprintf(file, "}");
	 }else{
	 	exit(-1);
	 	return;
	 }
}
void reflect_serialize(Stream *stream, Reflection r){
	 Reflection rf = r;
	 if(rf.indirection_count){	
	 	do{
		 	if(*(void**)rf.ptr == 0){	
				stream_write_u64(stream, 0);
				return; 
	 		}
	 		if(rf.indirection_count>0){
				reflect_deref(rf, &rf);
	 		}
	 	}while(rf.indirection_count>1);	
		stream_write_u64(stream, 1);
	 }
	 if(rf.info->info== TypeInt){
		int64_t s = 0;
	 	reflect_get_int(rf, &s);
	 	stream_write(stream, &s, sizeof(s));

	 }else if(rf.info->info == TypeUInt){
		uint64_t s = 0;
	 	reflect_get_uint(rf, &s);
	 	stream_write(stream, &s, sizeof(s));

	 }else if(rf.info->info == TypeFloat){
	 	double s = 0.0;
	 	reflect_get_float(rf, &s);
		stream_write(stream, &s, sizeof(s));	
	 }else if(rf.info->info == TypeString){
	 	const char * s = 0;
	 	reflect_get_string(rf, &s);
 		uint64_t l = 0;
		if(s != 0){
			l = strlen(s);
		}
		stream_write(stream, &l, sizeof(l));
		if(l != 0){
			stream_write(stream, (void*)s, l);
		}
	
		
	 }else if(rf.info->info== TypeSlice){
	 	return;
	 }else if(rf.info->info == TypeStruct){
		stream_write_u64(stream,strlen(rf.info->name));
		stream_write(stream, (void*)rf.info->name, strlen(rf.info->name));
	 	for(size_t i =0; i<rf.info->field_count; i++){	
	 		Reflection r;
	 		reflect_get_field(rf, i, &r);	
			reflect_serialize(stream, r);
	 	}	
	 }else{
	 	exit(-1);
	 	return;
	 }

}

void reflect_deserialize(Stream *stream, Reflection r){
	 Reflection rf = r;
	 if(rf.indirection_count){	
		uint64_t t = stream_read_u64(stream);
		void * ptr = 0;
		if(t){
			Reflection rt= rf;
			rt.indirection_count -=1;
			rt.ptr = malloc(rt.info->size);
			reflect_deserialize(stream, rt);
			*(void**)rf.ptr = rt.ptr;
			for(size_t i =0; i<rf.indirection_count-1; i++){
				
				void ** ptr = malloc(sizeof(void*));
				*ptr = *(void**)rt.ptr;
				*(void**)rt.ptr = ptr;
				rt.indirection_count-=1;
			}
			return;
		}else{
			*(void**)rf.ptr =0;
			return;
		}
	 }
	 if(rf.info->info== TypeInt){
		if(reflect_is_array(rf)){
			assert(0);
		}else{
			int64_t s = 0;
			stream_read(stream, &s, sizeof(s));
			reflect_set_int(rf, s);
		}

	 }else if(rf.info->info == TypeUInt){
		uint64_t s = 0;
		stream_read(stream, &s, sizeof(s));
		reflect_set_uint(rf, s);	
	 }else if(rf.info->info == TypeFloat){
		if(reflect_is_array(rf)){
			assert(0);
		}
		else{
			double s = 0;
			stream_read(stream, &s, sizeof(s));
			reflect_set_float(rf, s);	
		}
	 }else if(rf.info->info == TypeString){
		if (reflect_is_array(rf)){
			assert(0);
		}else{
 			uint64_t l =stream_read_u64(stream);	
			char * ptr = 0;
			if(l != 0){
				ptr = malloc(l+1)	;
				ptr[l] =0;
				stream_read(stream, ptr, l);
			}
			reflect_set_cstr(rf, ptr);	
		}
		
	 }else if(rf.info->info== TypeSlice){
	 	return;
	 }else if(rf.info->info == TypeStruct){
		int l = strlen(rf.info->name);
		long p = stream_read_u64(stream);
		if(l != p){
			return;
		}
		char * name = malloc(l);
		stream_read(stream, name, l);
		for(int i =0; i<l ; i++){
			if(name[i] != rf.info->name[i]){
				return;
			}
		}
		free(name);
	 	for(size_t i =0; i<rf.info->field_count; i++){	
	 		Reflection r;
	 		reflect_get_field(rf, i, &r);	
			reflect_deserialize(stream, r);
	 	}	
	 }else{
	 	exit(-1);
	 	return;
	 }

}

int reflect_is_array(Reflection ref){
	return ref.count>1;
}
Reflection reflect_array_access(Reflection ref, int offset){
	Reflection out = ref;
	if(ref.info->info == TypeSlice){	
		Reflection ptr;
		const Type * t = ref.info->composition[0].info;
		reflect_get_field(ref, 0, &ptr);
		char * p = *(void**)ptr.ptr;
		char * out =p+ t->size*offset;
		Reflection o;
		o.indirection_count = ref.indirection_count;
		o.count = 1;
		o.info = t;
		o.ptr = out;
		return o;
	}else{	
		const Type * t= ref.info;
		char * p = ref.ptr;
		char * out = p+t->size*offset;
		Reflection o;
		o.count = 1;
		o.indirection_count = ref.indirection_count;
		o.info = t;
		o.ptr = out;
		return o;
	}
}
uint64_t reflect_array_size(Reflection ref){
	if(ref.info->info == TypeSlice){
		Reflection sz;
		reflect_get_field(ref, 1, &sz);
		uint64_t p;
		reflect_get_uint(sz, &p);
		return p;
	}else{
		return ref.count;
	}
}
