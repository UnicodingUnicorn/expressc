# expressc

[Express.js](expressjs.com) in C. Almost. It's still a work in progress, so please don't expect too much as there isn't quite everything in yet. As of now, basic sending of text and files is supported, along with the routing functions everyone knows and loves. Do note that winsocks hasn't been integrated and as such this will only work on non-Windows platforms.

#Install

As of yet, expressc isn't a full fledged library, and hence doesn't require a proper install. Instead, download the repository, copy ```express.c``` and ```express.h``` to your project folder, and add the following line:
```C
#include "express.h"
```
to the top of each file you would be using it in.

#Usage

##Initialising

In ```main()```, before adding handler functions, call ```express_server_initialise()```, this prepares for the adding of functions.

##Routing

###Basics

```C
#include "expressc.h"

void example_get_function(struct Request *req){
  //Do stuff
}

void example_post_function(struct Request *req){
  //Do stuff
}

int main(){
  express_server_initialise(); //Initialise
  
  add_get_function("/example_get", example_get_function); //Add a function to listener for GET
  add_post_function("/example_post", example_post_function); //Add a function to listener for POST
  
  express_server_start(8080); //Start the server
  
  return 0;
}
```
```struct Request``` is a custom structure consisting of the address, headers and body of the HTTP request received. Type is also included by I doubt it's really neccesary. Functions have to accept it as their first argument.

##Multiple functions for the same address
```C
#include "expressc.h"

void example_get_function_1(struct Request *req){
  printf("Function 1\n");
}

void example_get_function_2(struct Request *req){
  printf("Function 2\n");
}

int main(){
  express_server_initialise(); //Initialise
  
  add_get_function("/example_get", example_get_function_1); //Add first function to listener for GET
  add_get_function("/example_get", example_get_function_2); //Add second function to listener for GET
  
  express_server_start(8080); //Start the server
  
  return 0;
}
```
Functions are called in the order they are added. For example, from the example above, if a GET request is sent to "/example_get", "Function 1" and then "Function 2" will be printed.

Also, since req is a pointer, any changes made to data inside will persist throughout the functions.

##Seperating into files

file1.c
```C
#include "expressc.h"
#include "file1.h"

void example_get_function_1(struct Request *req){
  printf("Function 1\n");
}
void init_file1(){
  add_get_function("/example_get", example_get_function_1);
}
```
file1.h
```C
void example_get_function_1(struct Request *req);
void init_file1();
```
main.c
```C
#include "expressc.h"
#include "file1.h"
int main(){
  express_server_initialise();
  
  init_file1();
  
  express_server_start(8080);
  
  return 0;
}
```

##Sending

There are two methods of sending: ```express_send(struct Request req, char *message)``` and ```express_send_file(struct Request req, char *fileName)```

###express_send
Sends simple strings. 
```C
void example_function(struct Request *req){
  express_send(req, "Foo");
}
```

###express_send_file
Sends an entire file, for example when you want to serve a webpage.
```C
void example_function(struct Request *req){
  express_send_file(req, "foo.html");
}
```
Currently, the file root is in the same directory as expressc.c

##Starting

To start the server, call ```express_server_start(port_number)``` after you have finished adding the handler functions. ```port_number``` must be an integer. The function will automatically print a message once it is listening on the specified port.
