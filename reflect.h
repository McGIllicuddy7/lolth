#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
typedef enum {
	TypeInt, 
	TypeUInt,
	TypeFloat,
	TypeString, 	
	TypeSlice,
	TypeStruct,
	TypeFunction
}TypeInfo;
struct Type;
typedef struct{
	struct Type * info;
	size_t offset;
	const char * length_field_name;
	size_t indirection_count;
	size_t count;
	const char * name;
}Field;
typedef struct Type{
	TypeInfo info;
	size_t size;
	size_t align;
	size_t field_count;
	Field * composition;	
	const char * name;
} Type;

#define REFLECT_VALUE __attribute__((unused)) static
#define INT_TYPE(SZ,NAME) REFLECT_VALUE Type NAME##INFO=  (Type){.info = TypeInt,.size = SZ, .align = SZ, .composition = 0, .name = #NAME};
#define FLOAT_TYPE(SZ,NAME) REFLECT_VALUE Type NAME##INFO=  (Type){.info = TypeFloat,.size = SZ, .align = SZ, .composition = 0,  .name = #NAME};
#define UINT_TYPE(SZ,NAME) REFLECT_VALUE Type UNSIGNED_##NAME##INFO=  (Type){.info = TypeUInt,.size = SZ, .align = SZ, .composition = 0, .name = "unsigned_"#NAME};

INT_TYPE(1, char)
INT_TYPE(2, short)
INT_TYPE(4, int)
INT_TYPE(8, long)
UINT_TYPE(1, char)
UINT_TYPE(2, short)
UINT_TYPE(4, int)
UINT_TYPE(8, long)
FLOAT_TYPE(4, float)
FLOAT_TYPE(8, double)
REFLECT_VALUE Type cstrINFO = (Type){.info = TypeString, .size = sizeof(const char *), .align = sizeof(const char *), .composition = 0, .name = "cstr"};
#define sizeof_field(BASE_TYPE, NAME) sizeof(((BASE_TYPE*)0)->NAME)

#define REFLECT_FIELD(BASE_TYPE,TYPE,NAME) (Field){.name = #NAME,.info= &TYPE##INFO, .offset = offsetof(BASE_TYPE, NAME),.indirection_count =0, .count = sizeof_field(BASE_TYPE, NAME)/sizeof(TYPE)}

#define REFLECT_PTR(BASE_TYPE,TYPE,NAME,COUNT) (Field){.name = #NAME,.info = &TYPE##INFO, .offset = offsetof(BASE_TYPE, NAME),.indirection_count =COUNT, .count = 1}

#define REFLECT_CSTR(BASE_TYPE, NAME) (Field){.name = #NAME, .info= &cstr##INFO, .offset = offsetof(BASE_TYPE, NAME), .indirection_count =0, .count =1}

#define REFLECT_SLICE(BASE_TYPE, NAME, TYPE) (Field){.name = #NAME, .info = &TYPE##SliceINFO,.type = TypeSlice, .offset = offsetof(BASE_TYPE, NAME), .indirection_count = 0, .count =1)

#define REFLECT_FUNC_PTR(BASE_TYPE,NAME) (Field){.name = #NAME, .info = 0, .type = TypeFunction, .offset = offsetof(BASE_TYPE, NAME), .count = 1}

#define REFLECT_STRUCT(NAME, ...) REFLECT_VALUE Type NAME##INFO = (Type){.info = TypeStruct, .size = sizeof(NAME), .field_count = (sizeof((Field[]){__VA_ARGS__})/sizeof(Field)),.composition = (Field[]){__VA_ARGS__},.name = #NAME};


#define MAKE_SLICE(T) typedef struct{T* items; size_t count;} T##Slice; REFLECT_VALUE Type T##SliceInfo = (Type){.info = TypeSlice, .size = sizeof(T), .field_count = 0, .composition = 0};
typedef struct {
	Type * info;
	size_t indirection_count;
	void * ptr;
}Reflection;
#define REFLECT(T, V) (Reflection){.info =&T##INFO, .indirection_count = 0, .ptr = (void*)V}

REFLECT_VALUE int reflect_get_int(Reflection a, int64_t * out){
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
REFLECT_VALUE int reflect_get_uint(Reflection a, uint64_t * out){
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
REFLECT_VALUE int reflect_get_float(Reflection a, double* out){
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
REFLECT_VALUE int reflect_get_string(Reflection a, const char ** out){
	if(a.info->info != TypeString){
		return 1;	
	}
	if(a.indirection_count!= 0){
		return 1;
	}
	*out = *(const char**)a.ptr;
	return 0;
}
REFLECT_VALUE int reflect_get_field(Reflection a,int idx,Reflection * out){
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
	return 0;
}

REFLECT_VALUE int reflect_set_int(Reflection a, int64_t out){
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
REFLECT_VALUE int reflect_set_uint(Reflection a, uint64_t out){
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
REFLECT_VALUE int reflect_set_float(Reflection a, double out){
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
REFLECT_VALUE int reflect_set_cstr(Reflection a,const char * ptr){
	if(a.info->info != TypeString){
		return 1;
	}
	*(const char**)a.ptr = ptr;
	return 0;
}
REFLECT_VALUE int reflect_set_field(Reflection a, Reflection b, int idx){
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
REFLECT_VALUE int reflect_deref(Reflection a, Reflection * out){
	if(a.indirection_count==0){
		return 1;
	}
	Reflection b = a;
	b.indirection_count -=1;
	b.ptr = *(void**)b.ptr;
	*out = b;
	return 0;
}
REFLECT_VALUE void debug_fprint(FILE * file,Reflection ref){
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
REFLECT_VALUE ByteSlice reflect_serialize(){
}
