#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#define REFLECT_STRINGIFY1(S) (#S)
#define REFLECT_STRINGIFY(S) (REFLECT_STRINGIFY1(S))
typedef enum{
	REFLECT_STRUCT,
	REFLECT_I32,
	REFLECT_I64,
	REFLECT_U32,
	REFLECT_U64,
	REFLECT_I16,
	REFLECT_U16,
	REFLECT_I8,
	REFLECT_U8,
	REFLECT_F32,
	REFLECT_F64,
	REFLECT_ARRAY,
	REFLECT_PTR,
	REFLECT_SLICE,
	REFLECT_C_STRING,
}ReflectionFieldType;
[[maybe_unused]] static const char * reflection_names[] = {"struct", "i32", "i64", "u32", "u64", "i16", "u16", "i8", "u8", "f32", "f64", "array", "ptr", "slice", "cstr"};
struct ReflectionData;
typedef struct ReflectionField{
	ReflectionFieldType field_type;
	const struct ReflectionData* composed_of;
	size_t count;
	size_t offset;
	size_t size;
	const char *  field_name;
}ReflectionField;
typedef struct ReflectionData{
	ReflectionFieldType vt;
	ReflectionField* fields;
	size_t field_count;
	size_t size;
	const char * type_name;
}ReflectionData;

typedef struct{
	void * ptr;
	const ReflectionData * data;
}Reflection;
#define reflect_data(STRUCT_NAME,NAME) _Generic((((STRUCT_NAME*)NULL)->NAME),\
	 int8_t:(ReflectionField){.field_type = REFLECT_I8,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = 1},\
	int16_t:(ReflectionField){.field_type = REFLECT_I16,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME),.offset = offsetof(STRUCT_NAME, NAME), .size = 2},\
	int32_t:(ReflectionField){.field_type = REFLECT_I32,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME),.offset = offsetof(STRUCT_NAME, NAME), .size =4},\
	int64_t:(ReflectionField){.field_type = REFLECT_I64,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME),.offset = offsetof(STRUCT_NAME, NAME), .size = 8},\
	uint8_t:(ReflectionField){.field_type = REFLECT_U8,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = 1},\
	uint16_t:(ReflectionField){.field_type = REFLECT_U16,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME),.size =2},\
	uint32_t:(ReflectionField){.field_type = REFLECT_U32,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME),.size = 4}, \
	uint64_t:(ReflectionField){.field_type = REFLECT_U64,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = 8}, \
	float:(ReflectionField){.field_type = REFLECT_F32,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = 4},\
	double:(ReflectionField){.field_type = REFLECT_F64,.composed_of = 0,.count =1,.field_name = REFLECT_STRINGIFY(NAME),.offset = offsetof(STRUCT_NAME, NAME), .size = 8},\
	default: (ReflectionField){}\
	)
#define reflect_struct(STRUCT_NAME,NAME,T) (ReflectionField){.field_type = REFLECT_STRUCT, .composed_of = &T##ReflectionStruct,.count = 1, .field_name = REFLECT_STRINGIFY(NAME), .size = sizeof(T), .offset = offsetof(STRUCT_NAME, NAME)};
#define reflect_pointer(STRUCT_NAME,NAME,T) (ReflectionField){.field_type = REFLECT_PTR, .composed_of = &T##ReflectionStruct,.count = 1, .field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME),.size = sizeof(void*)}

#define reflect_array(STRUCT_NAME,NAME,T) (ReflectionField){.field_type = REFLECT_ARRAY, .composed_of = &T##ReflectionStruct, .count = sizeof(((STRUCT_NAME*)0)->NAME)/sizeof(T), .field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = sizeof(((STRUCT_NAME*)0)->NAME)}

#define reflect_c_str(STRUCT_NAME,NAME) (ReflectionField){.field_type = REFLECT_C_STRING, .composed_of = 0, .count = 0, .field_name = REFLECT_STRINGIFY(NAME), .offset = offsetof(STRUCT_NAME, NAME), .size = sizeof(const char *)}

#define make_reflection(T, FIELDS...) const ReflectionData T##ReflectionStruct = (ReflectionData){.vt = REFLECT_STRUCT,.fields = ((ReflectionField[]){FIELDS}), .field_count = sizeof((ReflectionField[]){FIELDS})/sizeof(ReflectionField), .type_name =REFLECT_STRINGIFY(T), .size = sizeof(T)};\
[[maybe_unused]]static Reflection T##_reflect(const T* v){\
	return (Reflection){.ptr = (void*)v, .data = &T##ReflectionStruct};\
}
#define reflect(T, v) T##_reflect(v)
const ReflectionData i8ReflectionStruct = {.vt = REFLECT_I8, .fields = 0, .field_count =0, .type_name = "i8", .size = 1};
const ReflectionData i16ReflectionStruct = {.vt = REFLECT_I16, .fields = 0, .field_count =0, .type_name = "i16", .size = 2};
const ReflectionData i32ReflectionStruct = {.vt = REFLECT_I32, .fields = 0, .field_count =0, .type_name = "i32", .size =4};
const ReflectionData i64ReflectionStruct = {.vt = REFLECT_I64, .fields = 0, .field_count =0, .type_name = "i64", .size = 8};
const ReflectionData u8ReflectionStruct = {.vt = REFLECT_U8, .fields = 0, .field_count =0, .type_name = "u8", .size = 1};
const ReflectionData u16ReflectionStruct = {.vt = REFLECT_U16, .fields = 0, .field_count =0, .type_name = "u16", .size = 2};
const ReflectionData u32ReflectionStruct = {.vt = REFLECT_U32, .fields = 0, .field_count =0, .type_name = "u32", .size = 4};
const ReflectionData u64ReflectionStruct = {.vt = REFLECT_U64, .fields = 0, .field_count =0, .type_name = "u64", .size = 8};
const ReflectionData f32ReflectionStruct = {.vt = REFLECT_F32, .fields = 0, .field_count =0, .type_name = "f32", .size = 4};
const ReflectionData f64ReflectionStruct = {.vt = REFLECT_F64, .fields = 0, .field_count =0, .type_name = "f64", .size = 8};
#define charReflectionStruct i8ReflectionStruct
#define intReflectionStruct i32ReflectionStruct
#define shortReflectionStruct i16ReflectionStruct
#define longReflectionStruct i64ReflectionStruct
#define floatReflectionStruct f32ReflectionStruct
#define doubleReflectionStruct f64ReflectionStruct

#define int8_tReflectionStruct i8ReflectionStruct
#define int32_tReflectionStruct i32ReflectionStruct
#define int16_tReflectionStruct i16ReflectionStruct
#define int64_tReflectionStruct i64ReflectionStruct
#define uint8_tReflectionStruct u8ReflectionStruct
#define uint32_tReflectionStruct u32ReflectionStruct
#define uint16_tReflectionStruct u16ReflectionStruct
#define uint64_tReflectionStruct u64ReflectionStruct

typedef struct{
	ReflectionFieldType vt;
	ReflectionField* field;
	void * ptr;
}RValue;
[[maybe_unused]] static RValue reflect_get(Reflection ref, size_t idx){
	ReflectionField f = ref.data->fields[idx];
	void * ptr = (char*)ref.ptr+f.offset;
	RValue out;
	out.ptr = ptr;
	out.vt = ref.data->fields[idx].field_type;
	out.field = &ref.data->fields[idx];
	return out;
}
[[maybe_unused]] static bool reflect_is_int(RValue v){
	if(v.vt == REFLECT_I8){
		return true;
	}else if(v.vt == REFLECT_I16){
		return true;
	}else if(v.vt == REFLECT_I32){
		return true;
	}else if(v.vt == REFLECT_I64){
		return true;
	}else{
		return false;
	}
}
[[maybe_unused]] static bool reflect_is_uint(RValue v){
	if(v.vt == REFLECT_U8){
		return true;
	}else if(v.vt == REFLECT_U16){
		return true;
	}else if(v.vt == REFLECT_U32){
		return true;
	}else if(v.vt == REFLECT_U64){
		return true;
	}else{
		return false;
	}
}
[[maybe_unused]] static bool reflect_is_ptr(RValue v){
	if(v.vt == REFLECT_PTR){
		return true;
	} else{
		return false;
	}
}
[[maybe_unused]] static bool reflect_is_struct(RValue v){
	return v.vt == REFLECT_STRUCT;
}
[[maybe_unused]] static bool reflect_is_c_str(RValue v){
	return v.vt == REFLECT_C_STRING;
}
[[maybe_unused]] static Reflection reflect_get_struct(RValue v){
	if(v.vt == REFLECT_STRUCT){
		Reflection out;
		out.ptr = v.ptr;
		out.data = v.field->composed_of;
		return out;
	}
	return (Reflection){};
}
[[maybe_unused]] static long reflect_get_int(RValue v){
	if(v.vt == REFLECT_I8){
		return (long)*(char *)(v.ptr);
	}else if(v.vt == REFLECT_I16){
		return (long)*(short*)(v.ptr);
	}else if(v.vt == REFLECT_I32){
		return (long)*(int*)(v.ptr);
	}else if(v.vt == REFLECT_I64){
		return *(long*)(v.ptr);
	}else{
		return 0;
	}
}
[[maybe_unused]] static unsigned long reflect_get_uint(RValue v){
	if(v.vt == REFLECT_U8){
		return (unsigned long)*(unsigned char *)(v.ptr);
	}else if(v.vt == REFLECT_U16){
		return (unsigned long)*(unsigned short*)(v.ptr);
	}else if(v.vt == REFLECT_U32){
		return (unsigned long)*(unsigned int*)(v.ptr);
	}else if(v.vt == REFLECT_U64){
		return *(unsigned long*)(v.ptr);
	}else{
		return 0;
	}
}
[[maybe_unused]] static double reflect_get_float(RValue v){
	if(v.vt == REFLECT_F32){
		return (double)*(float*)(v.ptr);
	}else if(v.vt == REFLECT_F64){
		return *(double*)(v.ptr);
	} else{
		return 0.0;
	}
}
[[maybe_unused]] static bool reflect_is_float(RValue v){
	if(v.vt == REFLECT_F32){
		return true;
	}else if(v.vt == REFLECT_F64){
		return true;
	} else{
		return false;
	}
}
[[maybe_unused]] static Reflection reflect_get_ptr(RValue v){
	if(v.vt == REFLECT_PTR){
		void * tmp =*(void**)(v.ptr);
		Reflection out;
		out.ptr = tmp;
		out.data = v.field->composed_of;
		return out;
	}else{
		return (Reflection){0};
	}
}

[[maybe_unused]] static const char * reflect_get_c_str(RValue v){
	if(v.vt == REFLECT_C_STRING){
		return *(const char**)v.ptr;
	}
	return 0;
}
[[maybe_unused]] static void reflect_set_int(RValue v, long a){
	if(v.vt == REFLECT_I8){
		*(char*)v.ptr = a;
	}else if(v.vt == REFLECT_I16){
		*(short*)v.ptr = a;
	}else if(v.vt == REFLECT_I32){
		*(int*)v.ptr = a;
	}else if(v.vt == REFLECT_I64){
		*(long*)v.ptr = a;
	}
}
[[maybe_unused]] static void reflect_set_uint(RValue v, unsigned long a){
	if(v.vt == REFLECT_U8){
		*(unsigned char*)v.ptr = a;
	}else if(v.vt == REFLECT_U16){
		*(unsigned short*)v.ptr = a;
	}else if(v.vt == REFLECT_U32){
		*(unsigned int*)v.ptr = a;
	}else if(v.vt == REFLECT_U64){
		*(unsigned long*)v.ptr = a;
	}
}
[[maybe_unused]] static void reflect_set_float(RValue v, double a){
	if(v.vt == REFLECT_F32){
		*(float*)v.ptr = a;
	}else if(v.vt == REFLECT_F64){
		*(double*)v.ptr = a;
	}
}
[[maybe_unused]] static void reflect_set_c_str(RValue v, const char * ptr){ 
	if(v.vt == REFLECT_C_STRING){
		*(const char**)v.ptr = ptr;
	}
}
[[maybe_unused]] static void reflect_set_ptr(RValue v, void * ptr){
	if(v.vt == REFLECT_PTR){
		*(void**)v.ptr = ptr;
	}
}
[[maybe_unused]] static void reflect_set_struct(RValue v, void * ptr, size_t size){
	if(v.vt == REFLECT_STRUCT){
		memcpy(v.ptr, ptr, size);
	}
}
[[maybe_unused]]

