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
#define N 100000000

struct Callback;
struct Header;
struct Request;

void expressc_server_initialise();
static void error(char *);
void expressc_add_get_handler(char *, void (*)());
int expressc_send(struct Request, char *);
int expressc_send_file(struct Request, char *);
static void process_request(int);
void expressc_server_start(int);

#endif