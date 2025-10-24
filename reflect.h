#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"
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
	const struct Type * info;
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
	const Field * composition;	
	const char * name;
} Type;


#define REFLECT_VALUE __attribute__((unused)) static
#define INT_TYPE(SZ,NAME) REFLECT_VALUE const Type NAME##INFO=  (const Type){.info = TypeInt,.size = SZ, .align = SZ, .composition = 0, .name = #NAME};
#define FLOAT_TYPE(SZ,NAME) REFLECT_VALUE const Type NAME##INFO=  (const Type){.info = TypeFloat,.size = SZ, .align = SZ, .composition = 0,  .name = #NAME};
#define UINT_TYPE(SZ,NAME) REFLECT_VALUE const Type UNSIGNED_##NAME##INFO=  (const Type){.info = TypeUInt,.size = SZ, .align = SZ, .composition = 0, .name = "unsigned_"#NAME};

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
REFLECT_VALUE const Type cstrINFO = (const Type){.info = TypeString, .size = sizeof(const char *), .align = sizeof(const char *), .composition = 0, .name = "cstr"};
#define sizeof_field(BASE_TYPE, NAME) sizeof(((BASE_TYPE*)0)->NAME)

#define REFLECT_FIELD(BASE_TYPE,TYPE,NAME) (const Field){.name = #NAME,.info= &TYPE##INFO, .offset = offsetof(BASE_TYPE, NAME),.indirection_count =0, .count = sizeof_field(BASE_TYPE, NAME)/sizeof(TYPE)}

#define REFLECT_PTR(BASE_TYPE,TYPE,NAME,COUNT) (const Field){.name = #NAME,.info = &TYPE##INFO, .offset = offsetof(BASE_TYPE, NAME),.indirection_count =COUNT, .count = 1}

#define REFLECT_CSTR(BASE_TYPE, NAME) (const Field){.name = #NAME, .info= &cstr##INFO, .offset = offsetof(BASE_TYPE, NAME), .indirection_count =0, .count =1}

#define REFLECT_SLICE(BASE_TYPE, NAME, TYPE) (const Field){.name = #NAME, .info = &TYPE##SliceINFO,.type = TypeSlice, .offset = offsetof(BASE_TYPE, NAME), .indirection_count = 0, .count =1)

#define REFLECT_FUNC_PTR(BASE_TYPE,NAME) (const Field){.name = #NAME, .info = 0, .type = TypeFunction, .offset = offsetof(BASE_TYPE, NAME), .count = 1}

#define REFLECT_STRUCT(NAME, ...) REFLECT_VALUE Type NAME##INFO = (const Type){.info = TypeStruct, .size = sizeof(NAME), .field_count = (sizeof((const Field[]){__VA_ARGS__})/sizeof(Field)),.composition = (const Field[]){__VA_ARGS__},.name = #NAME};


#define MAKE_SLICE(T) typedef struct{T* items; size_t count;} T##Slice; REFLECT_VALUE Type T##SliceInfo = (Type){.info = TypeSlice, .size = sizeof(T), .field_count = 0, .composition = 0};
typedef struct {
	const Type * info;
	size_t indirection_count;
	void * ptr;
}Reflection;
#define REFLECT(T, V) (Reflection){.info =&T##INFO, .indirection_count = 0, .ptr = (void*)V}

int reflect_get_int(Reflection a, int64_t * out);
int reflect_get_uint(Reflection a, uint64_t * out);
int reflect_get_float(Reflection a, double* out);
int reflect_get_string(Reflection a, const char ** out);
int reflect_get_field(Reflection a,int idx,Reflection * out);
int reflect_set_int(Reflection a, int64_t out);
int reflect_set_uint(Reflection a, uint64_t out);
int reflect_set_float(Reflection a, double out);
int reflect_set_cstr(Reflection a,const char * ptr);
int reflect_set_field(Reflection a, Reflection b, int idx);
int reflect_deref(Reflection a, Reflection * out);
void reflect_serialize(Stream *s, Reflection r);
void reflect_deserialize(Stream *stream, Reflection r);
void debug_fprint(FILE * file,Reflection ref);
