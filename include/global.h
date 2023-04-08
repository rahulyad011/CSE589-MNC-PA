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

#define MESSAGE_LEN 65535

using namespace std;

#define FOR_RANGE(start, end, code_block) \
for (int i = (start); i < (end); i++) { \
    code_block \
}

// print_functions.cpp
void print_log_success(string command);
void print_log_end(string command);

//helper_functions.cpp
int string_to_int(string str);
string set_ip();
vector<string> split_string(string input_string, string delimiter);
void print_fd_set(fd_set& set);
void *get_in_addr(struct sockaddr *sa);


// client.cpp
int client(string port_number);

// server.cpp
int server(string port_num);

// Reference = https://github.com/tingting0711/CSE589_network_programming/blob/master/cse489589_assignment1/twang49/src/twang49_assignment1.cpp
struct SocketObject
{
    int cfd;
    int port_num;
    int num_msg_sent;
    int num_msg_rcv;
    string status; 
    string ip;
    string port;
    string hostname;
    vector<string> blockeduser;
    vector<string> msgbuffer;
    bool operator<(const SocketObject &abc) const 
    {
        return string_to_int(port) < string_to_int(abc.port);
    }
};

// network_functions.cpp
SocketObject* newSocketObject(int cfd, string hostname, string ip, string port);
SocketObject* is_exist_Socket(string ip, string port);
SocketObject* is_exist_Socket(string ip);
SocketObject* is_exist_Socket(int cfd);


//helper functions for sockets
void printSocketList(const vector<SocketObject>& socketlist);
string trimString(string& str);

#endif