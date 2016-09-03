#ifndef EXPRESSC
#define EXPRESSC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define BUFFER_SIZE 10196
#define CHUNK_SIZE 512
#define N 1000

struct Callback{
	char address[100];
	void (*func_pointer)(struct Request *);
};

struct Header{
	char name[50];
	char data[50];
};

struct Request{
	int sockfd;
	char type[8];
	char address[50];
	struct Header headers[50];
	char body[500];
};

void expressc_server_initialise();
static void error(char *);
void expressc_add_get_handler(char *, void (*)(struct Request *));
void expressc_add_post_handler(char *, void (*)(struct Request *));
int expressc_send(struct Request, char *);
int expressc_send_file(struct Request, char *);
static void process_request(int);
void expressc_server_start(int);

#endif
