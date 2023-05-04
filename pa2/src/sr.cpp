#include "../include/simulator.h"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

#include <iostream>
#include <numeric>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

class pkt_Class {
public:
    pkt packet;
    float time;
};

class Buffer{
private:
    vector<pkt> send_buffer;
    vector<pkt> recv_buffer;
    vector<pkt_Class> ack_buffer;
public:
    Buffer() {
      
    }

    struct pkt* fetch_pkt_with_seqnum_recv_buffer(int seqnum){
      vector<pkt>::iterator it = recv_buffer.begin();
      for(;it != recv_buffer.end(); it++){
        if(it->seqnum == seqnum){
          return &(*it);
        }
      }
      cout<<"Can't find pkt in recv_buffer related to seqnum = "<< seqnum <<". Returning NULL"<<endl;
      return NULL;
    }

    class pkt_Class* fetch_pkt_Class_with_seqnum_ack_buffer(int seqnum){
      vector<pkt_Class>::iterator it = ack_buffer.begin();
      for(;it != ack_buffer.end(); it++){
        if(it->packet.seqnum == seqnum){
          return &(*it);
        }
      }
      cout<<"Can't find pkt_Class in ack_buffer related to seqnum = "<< seqnum <<". Returning NULL"<<endl;
      return NULL;
    }
    
    int remove_pkt_with_seqnum_recv_buffer(int seqnum){
      vector<pkt>::iterator it = recv_buffer.begin();
      for(;it != recv_buffer.end(); it++){
        if(it->seqnum == seqnum){
          recv_buffer.erase(it);
          return 0;
        }
      }
      return -1;
    }

    int remove_pkt_Class_with_seqnum_ack_buffer(int seqnum){
      vector<pkt_Class>::iterator it = ack_buffer.begin();
      for(;it != ack_buffer.end(); it++){
        if(it->packet.seqnum == seqnum){
          ack_buffer.erase(it);
          return 0;
        }
      }
      return -1;
    }

    void clear_send_buffer(){
      send_buffer.clear();
    }

    void clear_recv_buffer(){
      recv_buffer.clear();
    }

    void clear_ack_buffer(){
      ack_buffer.clear();
    }

    int get_size_send_buffer(){
      return send_buffer.size();
    }

    int get_size_recv_buffer(){
      return recv_buffer.size();
    } 

    int get_size_ack_buffer(){
      return ack_buffer.size();
    } 
    
    void insert_pkt_at_begin_in_send_buffer(struct pkt* packet){
      send_buffer.insert(send_buffer.begin(), *packet);
    }

    struct pkt get_back_pkt_send_buffer(){
      return send_buffer.back();
    }

    void push_back_pkt_in_recv_buffer(struct pkt packet){
      recv_buffer.push_back(packet);
    }

    void push_back_pkt_Class_in_ack_buffer(class pkt_Class* packet_Class){
      ack_buffer.push_back(*packet_Class);
    }

    void pop_back_from_send_buffer(){
      send_buffer.pop_back();
    }

    vector<pkt>::iterator get_recv_buffer_begin(){
      return recv_buffer.begin();
    }

    vector<pkt>::iterator get_recv_buffer_end(){
      return recv_buffer.end();
    }

    vector<pkt_Class>::iterator get_ack_buffer_begin(){
      return ack_buffer.begin();
    }

    vector<pkt_Class>::iterator get_ack_buffer_end(){
      return ack_buffer.end();
    }

    bool is_recv_buffer_empty(){
      return recv_buffer.empty();
    }

    bool is_ack_buffer_empty(){
      return ack_buffer.empty();
    }
};

int ack, seq, recv_seq, window;
float RTT = 20.0;

Buffer buffer_obj;

int calc_checksum(struct pkt *p) {
  int sum = 0;
  sum += p->seqnum + p->acknum;
  for (int i = 0; i < 20; i++) {
    sum += (int)p->payload[i];
  }
  return sum;
}

int calc_checksum_simple(struct pkt p) {
  int sum = 0;
  sum += p.seqnum + p.acknum;
  return sum;
}

void copyArray(char* destination_array, char* source_array, int size) {
  int i;
  for (i = 0; i < size && source_array[i] != '\0'; i++) {
    destination_array[i] = source_array[i];
  }
  for (; i < size; i++) {
    destination_array[i] = '\0';
  }
}

void setArray(char* destination_array, int value, int size){
  for (int i = 0; i < size; i++) {
    destination_array[i] = value;
  }
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  struct pkt* packet = new pkt();
  packet->seqnum = seq;
  packet->acknum = ack;
  setArray(packet->payload, 0, 20);
  copyArray(packet->payload, message.data, 20);
  packet->checksum = calc_checksum(packet);

  buffer_obj.insert_pkt_at_begin_in_send_buffer(packet);
  seq++;

  if(buffer_obj.get_size_ack_buffer() < window && buffer_obj.get_size_send_buffer() > 0){
    class pkt_Class* packet_Class = new pkt_Class();
    packet_Class->packet = buffer_obj.get_back_pkt_send_buffer();
    packet_Class->time = get_sim_time() + RTT;

    tolayer3(0, packet_Class->packet);

    if(buffer_obj.get_size_ack_buffer() == 0){
      starttimer(0, RTT);
    }
    buffer_obj.push_back_pkt_Class_in_ack_buffer(packet_Class);
    buffer_obj.pop_back_from_send_buffer();
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  if(buffer_obj.fetch_pkt_Class_with_seqnum_ack_buffer(packet.seqnum) != NULL && packet.checksum == calc_checksum_simple(packet)){
      stoptimer(0);
      buffer_obj.remove_pkt_Class_with_seqnum_ack_buffer(packet.seqnum);
      
      if(!buffer_obj.is_ack_buffer_empty()){
        float time_value = buffer_obj.get_ack_buffer_begin()->time;

        vector<pkt_Class>::iterator it = buffer_obj.get_ack_buffer_begin();
        for(; it != buffer_obj.get_ack_buffer_end(); it++){
          if(it->time < time_value){
            time_value = it->time;
          }
        }

        starttimer(0, time_value - get_sim_time());
      }

      if(buffer_obj.get_size_ack_buffer() < window && buffer_obj.get_size_send_buffer() > 0){
        class pkt_Class* packet_Class = new pkt_Class();
        packet_Class->packet = buffer_obj.get_back_pkt_send_buffer();
        packet_Class->time = get_sim_time() + RTT;

        tolayer3(0, packet_Class->packet);

        if(buffer_obj.get_size_ack_buffer() == 0){
          starttimer(0, RTT);
        }
        buffer_obj.push_back_pkt_Class_in_ack_buffer(packet_Class);
        buffer_obj.pop_back_from_send_buffer();
      }
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  float time_value = buffer_obj.get_ack_buffer_begin()->time;
  int seq_value = buffer_obj.get_ack_buffer_begin()->packet.seqnum;

  vector<pkt_Class>::iterator it = buffer_obj.get_ack_buffer_begin();
  for(; it != buffer_obj.get_ack_buffer_end(); it++){
    if(it->time < time_value){
      time_value = it->time;
      seq_value = it->packet.seqnum;
    }
  }

  class pkt_Class* packet_Class = buffer_obj.fetch_pkt_Class_with_seqnum_ack_buffer(seq_value);
  packet_Class->time = get_sim_time() + RTT;

  struct pkt packet = packet_Class->packet;

  tolayer3(0, packet);
  
  if(!buffer_obj.is_ack_buffer_empty()){
    float time_value = buffer_obj.get_ack_buffer_begin()->time;

    vector<pkt_Class>::iterator it = buffer_obj.get_ack_buffer_begin();
    for(; it != buffer_obj.get_ack_buffer_end(); it++){
      if(it->time < time_value){
        time_value = it->time;
      }
    }

    starttimer(0, time_value - get_sim_time());
  }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  seq = 0;
  ack = 1;
  window = getwinsize();
  buffer_obj.clear_send_buffer();
  buffer_obj.clear_ack_buffer();
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  if(calc_checksum(&packet) == packet.checksum){
    struct pkt* ack_packet = new pkt();
    ack_packet->seqnum = packet.seqnum;
    ack_packet->acknum = 1;
    setArray(ack_packet->payload, 0, 20);
    ack_packet->checksum = packet.seqnum + 1;

    tolayer3(1, *ack_packet);

    if(buffer_obj.fetch_pkt_with_seqnum_recv_buffer(packet.seqnum) != NULL && packet.seqnum < recv_seq){  
      return;
    }

    buffer_obj.push_back_pkt_in_recv_buffer(packet);

    while(true){
      if(buffer_obj.is_recv_buffer_empty()) break;

      int seq_value = buffer_obj.get_recv_buffer_begin()->seqnum;

      vector<pkt>::iterator it = buffer_obj.get_recv_buffer_begin();
      for(; it != buffer_obj.get_recv_buffer_end(); it++){
        if(it->seqnum < seq_value){
          seq_value = it->seqnum;
        }
      }

      struct pkt* min_packet = buffer_obj.fetch_pkt_with_seqnum_recv_buffer(seq_value);
      if(seq_value == recv_seq){
        tolayer5(1, min_packet->payload);
        buffer_obj.remove_pkt_with_seqnum_recv_buffer(seq_value);
        recv_seq++;
      }
      else{
        break;
      }
    }
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  recv_seq = 0;
  buffer_obj.clear_recv_buffer();
}
