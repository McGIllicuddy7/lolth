#define LOLTH_IMPLEMENTATION
#include "lolth.h"
typedef struct Person{
	char *name;
	int age;
	struct Person * next;
}Person;
typedef const char * cstr;
REFLECT_STRUCT(Person, 
	REFLECT_CSTR(Person, name),
	REFLECT_FIELD(Person, int, age),
	REFLECT_PTR(Person, Person, next,1)
)

void test1(void){
	Person* p = 0;
	for(int i =0; i<2000; i++){
		Person * n = p;
		p = malloc(sizeof(Person));
		p->next = n;
		p->age = i;
		p->name = (char*)malloc(32);
		snprintf(p->name,32, "person: %d", i);
	}
	Reflection r = REFLECT(Person, p);
	reflect_serialize_to_file(r, "test.bin");
}
void test2(void){
	Person p;
	Reflection r= REFLECT(Person, &p);
	reflect_deserialize_from_file(std_allocator,r, "test.bin");
	debug_fprint(stdout, r);
}
int main(void ){
	test1();
	test2();
} 
