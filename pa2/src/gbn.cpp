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
    vector<pkt> wait_buffer;
    vector<pkt_Class> send_buffer;
    vector<pkt_Class> ack_buffer;
public:
    Buffer() {
      
    }

    int remove_pkt_Class_with_seqnum_send_buffer(int seqnum){
      vector<pkt_Class>::iterator it = send_buffer.begin();
      for(;it != send_buffer.end(); it++){
        if(it->packet.seqnum == seqnum){
          send_buffer.erase(it);
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

    class pkt_Class* fetch_pkt_Class_with_seqnum_send_buffer(int seqnum){
      vector<pkt_Class>::iterator it = send_buffer.begin();
      for(;it != send_buffer.end(); it++){
        if(it->packet.seqnum == seqnum){
          return &(*it);
        }
      }
      cout<<"Can't find pkt_Class in send_buffer related to seqnum = "<< seqnum <<". Returning NULL"<<endl;
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

    void clear_all_buffers(){
      wait_buffer.clear();
      send_buffer.clear();
      ack_buffer.clear();
    }

    void clear_wait_buffer(){
      wait_buffer.clear();
    }

    void clear_send_buffer(){
      send_buffer.clear();
    }

    void clear_ack_buffer(){
      ack_buffer.clear();
    }

    void insert_pkt_at_begin_in_wait_buffer(struct pkt* packet){
      wait_buffer.insert(wait_buffer.begin(), *packet);
    }

    int get_size_wait_buffer(){
      return wait_buffer.size();
    }

    int get_size_send_buffer(){
      return send_buffer.size();
    } 

    int get_size_ack_buffer(){
      return ack_buffer.size();
    } 

    struct pkt get_back_pkt_wait_buffer(){
      return wait_buffer.back();
    }

    void push_pkt_Class_in_send_buffer(class pkt_Class* packet_Class){
      send_buffer.push_back(*packet_Class);
    }

    void push_pkt_Class_in_ack_buffer(class pkt_Class* packet_Class){
      ack_buffer.push_back(*packet_Class);
    }

    void pop_pkt_from_wait_buffer(){
      wait_buffer.pop_back();
    }

    vector<pkt_Class>::iterator get_send_buffer_begin(){
      return send_buffer.begin();
    }

    vector<pkt_Class>::iterator get_send_buffer_end(){
      return send_buffer.end();
    }

    vector<pkt_Class>::iterator get_ack_buffer_begin(){
      return ack_buffer.begin();
    }

    bool is_send_buffer_empty(){
      return send_buffer.empty();
    }
};

float delay = 1;
float RTT = 35.0;
int WINDOW_SIZE = 1;

int ack, seq;
int seq_a, seq_b;

Buffer buffer_obj;

int calc_checksum(struct pkt *p) {
  int sum = 0;
  sum += p->seqnum + p->acknum;
  return sum;
}

int calc_default_checksum(struct pkt *p) {
  int sum = 0;
  sum += p->seqnum + p->acknum;
  for (int i = 0; i < 20; i++) {
    sum += (int)p->payload[i];
  }
  return sum;
}

void copyArray(char* destination_array, char* source_array, int size) {
  for (int i = 0; i < size; i++) {
    destination_array[i] = source_array[i];
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
  copyArray(packet->payload, message.data, 20);
	packet->checksum = calc_default_checksum(packet);

  buffer_obj.insert_pkt_at_begin_in_wait_buffer(packet);
  seq++;

  if(buffer_obj.get_size_send_buffer() < WINDOW_SIZE && buffer_obj.get_size_wait_buffer() > 0){
    class pkt_Class* packet_Class = new pkt_Class();
    packet_Class->packet = buffer_obj.get_back_pkt_wait_buffer();
    packet_Class->time = get_sim_time() + RTT;

    buffer_obj.push_pkt_Class_in_send_buffer(packet_Class);
    tolayer3(0, packet_Class->packet);

    if(buffer_obj.get_size_send_buffer() == 1){
      starttimer(0, RTT);
    }

    buffer_obj.pop_pkt_from_wait_buffer();
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  class pkt_Class* packet_Class = buffer_obj.fetch_pkt_Class_with_seqnum_send_buffer(packet.seqnum);

  if(packet_Class != NULL && packet.checksum == calc_checksum(&packet)){
     if((get_sim_time() - packet_Class->time + RTT < delay) || (buffer_obj.fetch_pkt_Class_with_seqnum_ack_buffer(packet.seqnum) != NULL)){
      return;
     }

     buffer_obj.push_pkt_Class_in_ack_buffer(packet_Class);

     if(packet.seqnum == seq_a){
      stoptimer(0);
      while(buffer_obj.get_size_ack_buffer() != 0){
        if(buffer_obj.get_ack_buffer_begin()->packet.seqnum == seq_a){
          buffer_obj.remove_pkt_Class_with_seqnum_send_buffer(seq_a);
          buffer_obj.remove_pkt_Class_with_seqnum_ack_buffer(seq_a);
          seq_a++;
        }
        else{
          break;
        }
      }
      while(buffer_obj.get_size_send_buffer() < WINDOW_SIZE && buffer_obj.get_size_wait_buffer() > 0){
        class pkt_Class* packet_Class = new pkt_Class();
        packet_Class->packet = buffer_obj.get_back_pkt_wait_buffer();
        packet_Class->time = get_sim_time() + RTT;

        buffer_obj.push_pkt_Class_in_send_buffer(packet_Class);
        tolayer3(0, packet_Class->packet);

        if(buffer_obj.get_size_send_buffer() == 1){
          starttimer(0, RTT);
        }

        buffer_obj.pop_pkt_from_wait_buffer();
      }
      if(!buffer_obj.is_send_buffer_empty()){
        starttimer(0, buffer_obj.get_send_buffer_begin()->time - get_sim_time());
      }
      return;
     }
  }
  return;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  buffer_obj.clear_ack_buffer();

  vector<pkt_Class>::iterator it = buffer_obj.get_send_buffer_begin();
  float eps = 0;

  for(; it != buffer_obj.get_send_buffer_end(); it++){
    class pkt_Class* packet_Class = buffer_obj.fetch_pkt_Class_with_seqnum_send_buffer(it->packet.seqnum);
    packet_Class->time = get_sim_time() + RTT + eps*2;
    tolayer3(0, packet_Class->packet);
    eps++;
  }
  starttimer(0, RTT);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  WINDOW_SIZE = getwinsize();
  ack = 0;
  seq = 0;
  seq_a = 0;

  buffer_obj.clear_all_buffers();
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  if(packet.checksum != calc_default_checksum(&packet)){
    return;
  }

  struct pkt* ack_packet = new pkt();
  ack_packet->seqnum = packet.seqnum;
  ack_packet->acknum = 1;
  setArray(ack_packet->payload, 0, sizeof(ack_packet->payload));
  ack_packet->checksum = packet.seqnum + 1;

  tolayer3(1, *ack_packet);
  if(packet.seqnum == seq_b){
    tolayer5(1, packet.payload);
    seq_b += 1;
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  seq_b = 0;
}
