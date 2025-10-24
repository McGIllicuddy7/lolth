#include "reflect.h"
#include "stream.h"
typedef struct Person{
	char name[256];
	int age;
	struct Person * next;
}Person;
typedef const char * cstr;
REFLECT_STRUCT(Person, 
	REFLECT_FIELD(Person, char,name), 
	REFLECT_FIELD(Person, int, age),
	REFLECT_PTR(Person, Person, next,1)
)

void test1(void){
	Person p;
	memcpy(p.name,"testing 1 2 3", sizeof("testing 1 2 3"));
	p.age = 32;
	p.next = 0;
	Reflection r = REFLECT(Person, &p);
	Stream s = stream_create();
	reflect_serialize(&s, r);
	s.head = 0;
	reflect_deserialize(&s, r);
	debug_fprint(stdout, r);
}
int main(void ){
	test1();
} 
