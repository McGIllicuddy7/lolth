/*#include "async.h"*/
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "context.h"
void test(void * arg){
	printf("testing 123\n");
}
int main(){
	lolth_init();
	spawn(test,0);
	printf("exit success\n");
	return 0;
}
