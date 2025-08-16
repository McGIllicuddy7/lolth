#include <stdio.h>
#include "reflect.h"
typedef struct Test{
	int x;
	int y;
	char * name;
	struct Test * next;
}Test;
void debug_print(Reflection v){
	printf("{");
	for(size_t i =0; i<v.data->field_count; i++){
		RValue f = reflect_get(v, i);
		const char * fname = f.field->field_name;
		if(reflect_is_int(f)){
			printf("%s: %ld", fname,reflect_get_int(f));
		}else if(reflect_is_uint(f)){
			printf("%s: %lu", fname,reflect_get_uint(f));
		}else if(reflect_is_c_str(f)){
			printf("%s: \"%s\"", fname,reflect_get_c_str(f));
		}else if(reflect_is_float(f)){
			printf("%s: %f",fname, reflect_get_float(f));
		}else if(reflect_is_struct(f)){
			printf("%s: ",fname);
			debug_print(reflect_get_struct(f));
		}else if(reflect_is_ptr(f)){
			printf("%s: ",fname);
			Reflection r = reflect_get_ptr(f);
			if(r.ptr){
				debug_print(r);
			}else{
				printf("null");
			}
		}else{
			printf("ERROR:else");
		}
		if(i != v.data->field_count-1){
			printf(", ");
		}
	}
	printf("}\n");
}

make_reflection(Test, reflect_data(Test,x), reflect_data(Test,y), reflect_c_str(Test, name), reflect_pointer(Test, next, Test));
int main(){
	Test test2 = {0};
	test2.x = 32;
	test2.y = 16;
	test2.name = "test2";
	test2.next =0;
	Test test = {0};
	test.x = 0;
	test.y = 0;
	test.name = ":3";
	test.next =&test2;
	Reflection s = reflect(Test,&test);
	debug_print(s);
}
