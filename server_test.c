#include <stdio.h>
#include <stdlib.h>

#include "expressc.h"

void func1(struct Request *);
void func2(struct Request *);

int main(){
	printf("1\n");
	expressc_server_initialise();
	printf("2\n");
	expressc_add_get_handler("/", func1);
	printf("3\n");
	expressc_add_get_handler("/", func2);
	printf("Stuff\n");
	expressc_server_start(1837);
	return 0;
}

void func1(struct Request *request){
	printf("Function 1\n");
}
void func2(struct Request *request){
	printf("Function 2\n");
}
