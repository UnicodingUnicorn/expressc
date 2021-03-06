#include "expressc.h"

struct Callback get_callbacks[N];
unsigned int nget = 0;
struct Callback post_callbacks[N];
unsigned int npost = 0;
struct Callback put_callbacks[N];
unsigned int nput = 0;
struct Callback delete_callbacks[N];
unsigned int ndelete = 0;

//Initialised the thing. For stuff like 0-ing arrays
void expressc_server_initialise(){
	bzero(get_callbacks, sizeof(get_callbacks));
	nget = 0;
	bzero(post_callbacks, sizeof(post_callbacks));
	npost = 0;
	bzero(put_callbacks, sizeof(put_callbacks));
	nput = 0;
	bzero(delete_callbacks, sizeof(delete_callbacks));
	ndelete = 0;
}

//Prints error and exits
static void error(char *msg){
	perror(msg);
	exit(1);
}

//Adds a function pointer for GET
void expressc_add_get_handler(char address[], void (*func)(struct Request *)){
	struct Callback callback;
	strcpy(callback.address, address);
	callback.func_pointer = func;
	get_callbacks[nget] = callback;
	nget++;
}

//Adds a function pointer for POST
void expressc_add_post_handler(char address[], void (*func)(struct Request *)){
	struct Callback callback;
	strcpy(callback.address, address);
	callback.func_pointer = func;
	post_callbacks[npost] = callback;
	npost++;
}
//Adds a function pointer for PUT
void expressc_add_put_handler(char address[], void (*func)(struct Request *)){
	struct Callback callback;
	strcpy(callback.address, address);
	callback.func_pointer = func;
	put_callbacks[nput] = callback;
	nput++;
}

//Adds a function pointer for POST
void expressc_add_delete_handler(char address[], void (*func)(struct Request *)){
	struct Callback callback;
	strcpy(callback.address, address);
	callback.func_pointer = func;
	delete_callbacks[ndelete] = callback;
	ndelete++;
}

int expressc_send(struct Request request, char *message){
	char response[17 + strlen(message) + 1]; //17 is the char length of the header. Also null terminator
	strcpy(response, "HTTP/1.1 200 OK\n\n");
	strcat(response, message);
	return write(request.sockfd, response, strlen(response)); //Return the PID so user can handle (possible) error
}

int expressc_send_file(struct Request request, char *fileName){
	int n;
	FILE *fp;
	fp = fopen(fileName, "r");
	char response[100] = "HTTP/1.1 ";
	strcpy(response, "HTTP/1.1"); 
	if(fp == NULL){
		strcat(response, "404 File Not Found");
		n = write(request.sockfd, response, strlen(response));
	}else{
		strcat(response, "200 OK");
		
		char file_data[CHUNK_SIZE];
		size_t nbytes = 0;
		while((nbytes = fread(file_data, sizeof(char), CHUNK_SIZE, fp)) > 0){
			int sent;
			int offset = 0;
			while((sent = send(request.sockfd, file_data + offset, nbytes, 0)) > 0 || (sent == -1 && errno == EINTR)){
				if(sent > 0){
					offset += sent;
					nbytes -= sent;
				}
			}
		}
	}
	return n;
}

static void process_request(int sockfd){
	int i = 0, j = 0;; //For looping
	
	char buffer[BUFFER_SIZE]; //Request buffer received from connected client
	bzero(buffer, BUFFER_SIZE);
	
	int n = read(sockfd, buffer, BUFFER_SIZE); //Read buffer
	if (n < 0) 
		error("ERROR reading from socket");
	
	//Parse buffer
	struct Request request; //Data structure
	request.sockfd = sockfd;
	
	char *token;
	char lines[50][500];
	for(i = 0; i < 50; i++) strcpy(lines[i], "");
	
	i = 0;
	token = strtok(buffer, "\n");
	while(token != NULL){
		strcpy(lines[i], token);
		token = strtok(NULL, "\n");
		i++;
	}
	token = strtok(lines[0], " "); //Parse the first line of type and address
	strcpy(request.type, token);
	token = strtok(NULL, " ");
	strcpy(request.address, token);
	
	bzero(request.headers, sizeof(request.headers));
	i = 1;
	while(!(strcmp(lines[i], "") == 0)){
		char line[100];
		strcpy(line, lines[i]);
		token = strtok(line, ":");
		strcpy(request.headers[i].name, token);
		token = strtok(NULL, ":");
		strcpy(request.headers[i].data, token);
		i++;
	}
	strcpy(request.body, lines[i+1]);
	
	if(strcmp(request.type, "GET") == 0){
		struct Callback relevant[nget];
		bzero(relevant, sizeof(relevant));
		j = 0;
		for(i = 0; i < nget; i++){
			if(strcmp(get_callbacks[i].address, request.address) == 0){
				get_callbacks[i].func_pointer(&request);
			}
		}
	}else if(strcmp(request.type, "POST") == 0){
		struct Callback relevant[nget];
		bzero(relevant, sizeof(relevant));
		j = 0;
		for(i = 0; i < nget; i++){
			if(strcmp(post_callbacks[i].address, request.address) == 0){
				post_callbacks[i].func_pointer(&request);
			}
		}
	}
}

void expressc_server_start(int port_number){
	int sockfd, newsockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr; //Server and client addresses
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; //Set address family
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_number); //Set port number
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd, 5); //Backlog queue is 5
	clilen = sizeof(cli_addr);
	printf("Server is running at Port %d\n", port_number);
	//Start listening for requests
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		int pid = fork();
		if(pid < 0)
			error("ERROR on fork");
		if(pid == 0){
			close(sockfd);
			process_request(newsockfd);
			exit(0);
		}else{
			close(newsockfd);
		}
	}
	close(sockfd);
}
