#include "reflect.h"

typedef struct Person{
	const char * name;
	int age;
	struct Person * next;
}Person;
typedef const char * cstr;
REFLECT_STRUCT(Person, 
	REFLECT_CSTR(Person, name), 
	REFLECT_FIELD(Person, int, age),
	REFLECT_PTR(Person, Person, next,1)
)


int main(void ){
	Person p;
	p.name = "test";
	p.next =malloc(sizeof(Person));
	p.next->age = 69;
	p.next->name = "test2";
	p.next->next = 0;
	p.age = 32;
	Reflection r = REFLECT(Person, &p);	
	printf("next:%zu\n", PersonINFO.composition[2].offset);
	debug_fprint(stderr,r);

} 
