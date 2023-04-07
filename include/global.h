#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define PATH_LEN 256

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <math.h>
#include <algorithm> 
#include <sstream>
#include <string>

using namespace std;

#define FOR_RANGE(start, end, code_block) \
for (int i = (start); i < (end); i++) { \
    code_block \
}

//helper_functions.cpp functions
int string_to_int(string str);

// client.cpp functions
int client(string port_number);

// server.cpp functions
int server(string port_num);

#endif