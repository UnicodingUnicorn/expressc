#include <stdio.h>
#include <stdlib.h>

#include "expressc.h"

void *func1(struct Request *);
void *func2(struct Request *);

int main(){
	expressc_server_initialise();
	expressc_add_get_handler("/", &func1);
	expressc_add_get_handler("/", &func2);
	expressc_server_start(1837);
	return 0;
}

void *func1(struct Request *request){
	printf("Function 1\n");
}
void *func2(struct Request *request){
	printf("Function 2\n");
}
