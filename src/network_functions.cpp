#include "../include/global.h"
#include "../include/logger.h"

vector<SocketObject> socketlist;

typedef unsigned int Uint;

// Reference: https://github.com/tingting0711/CSE589_network_programming/blob/master/cse489589_assignment1/twang49/src/twang49_assignment1.cpp

SocketObject* newSocketObject(int cfd, string hostname, string ip, string port) 
{
    SocketObject* hd = new SocketObject;
    hd->cfd = cfd;
    hd->port_num = string_to_int(port);
    hd->num_msg_sent = 0;
    hd->num_msg_rcv = 0;
    hd->ip = ip;
    hd->port = port;
    hd->status = "logged-in";
    hd->hostname = hostname;
    return hd;
}

SocketObject* is_exist_Socket(string ip, string port) 
{
    //we can delete
    Uint i = 0;
    while(i < socketlist.size())
    {
        SocketObject* hd = &socketlist[i];
        int falg = hd->ip == ip && hd->port == port;
        if(falg)return hd;
        i ++;
    }   
  return NULL;
}

SocketObject* is_exist_Socket(string ip) 
{
    Uint i = 0;
    while(i < socketlist.size())
    {
        SocketObject* hd = &socketlist[i];
        int falg = hd->ip == ip;
        if(falg)return hd;
        i ++;
    }   
  return NULL;
}

// new ===
SocketObject* is_exist_Socket(int cfd) 
{
    Uint i = 0;
    while(i < socketlist.size())
    {
        SocketObject* hd = &socketlist[i];
        int falg = hd->cfd == cfd;
        if(falg)return hd;
        i ++;
    }   
  return NULL;
}