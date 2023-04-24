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

class Buffer {
private:
    vector<pkt> wait_buffer;
    vector<pkt> send_buffer;
public:
    Buffer() {
      
    }

    struct pkt get_front_pkt_wait_buffer(){
      return wait_buffer.front();
    }

    struct pkt get_front_pkt_send_buffer(){
      return send_buffer.front();
    }

    void add_pkt_wait_buffer(struct pkt packet){
      wait_buffer.push_back(packet);
      return;
    }

    void add_pkt_send_buffer(struct pkt packet){
      send_buffer.push_back(packet);
      return;
    }

    void remove_pkt_wait_buffer(){
      wait_buffer.erase(wait_buffer.begin());
      return;
    }

    void remove_pkt_send_buffer(){
      send_buffer.erase(send_buffer.begin());
      return;
    }

    struct pkt* fetch_pkt_with_seqnum_send_buffer(int seqnum){
      vector<pkt>::iterator it = send_buffer.begin();
      for(;it != send_buffer.end(); it++){
        if(it->seqnum == seqnum){
          struct pkt* packet = &(*it);
          return packet;
        }
      }

      cout<<"Returning Empty Packet"<<endl;
      struct pkt* empty_packet = new pkt();
      return empty_packet;
    }

    int remove_pkt_with_seqnum_send_buffer(int seqnum){
      vector<pkt>::iterator it = send_buffer.begin();
      for(;it != send_buffer.end(); it++){
        if(it->seqnum == seqnum){
          send_buffer.erase(it);
          return 0;
        }
      }
      return -1;
    }

    void clear_both_buffers(){
      wait_buffer.clear();
      send_buffer.clear();
    }

    int get_size_wait_buffer(){
      return wait_buffer.size();
    }

    int get_size_send_buffer(){
      return send_buffer.size();
    }

    struct pkt* transfer_pkt_wait_to_send(){
      add_pkt_send_buffer(get_front_pkt_wait_buffer());
      remove_pkt_wait_buffer();

      struct pkt front_pkt = get_front_pkt_wait_buffer();
      struct pkt* temp = &front_pkt;
      return temp;
    }
};

int seq_a, seq_b;
int ack;
float RTT = 10.0;

Buffer buffer_obj;

int calc_checksum(struct pkt *p) {
  int sum = 0;
  sum += p->seqnum + p->acknum;
  for (int i = 0; i < 20; i++) {
    sum += (int)p->payload[i];
  }
  return sum;
}

void copyArray(char* source_array, char* destination_array, int size) {
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
  struct pkt* new_packet = new pkt();
  new_packet->seqnum = seq_a;
  new_packet->acknum = ack;
  
  copyArray(message.data, new_packet->payload, 20);
  new_packet->checksum = calc_checksum(new_packet);
  buffer_obj.add_pkt_wait_buffer(*new_packet);
  seq_a++;

  if(buffer_obj.get_size_send_buffer() < 1 && buffer_obj.get_size_wait_buffer() > 0){
    tolayer3(0, *buffer_obj.transfer_pkt_wait_to_send());
    starttimer(0, RTT);
  }
  return;
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  if(packet.checksum == calc_checksum(&packet) && packet.acknum == 1 && buffer_obj.fetch_pkt_with_seqnum_send_buffer(packet.seqnum)){
    stoptimer(0);
    buffer_obj.remove_pkt_with_seqnum_send_buffer(packet.seqnum);
    if(buffer_obj.get_size_send_buffer() < 1 && buffer_obj.get_size_wait_buffer() > 0){
      tolayer3(0, *buffer_obj.transfer_pkt_wait_to_send());
      starttimer(0, RTT);
    }
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  struct pkt front_pkt = buffer_obj.get_front_pkt_wait_buffer();
  struct pkt* temp = &front_pkt;
  tolayer3(0, *temp);
  starttimer(0, RTT);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  seq_a = 0;
  ack = 1;
  buffer_obj.clear_both_buffers();
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  if(packet.checksum == calc_checksum(&packet)){
    struct pkt* new_ack_packet = new pkt();
    new_ack_packet->seqnum = packet.seqnum;
    new_ack_packet->acknum = 1;

    setArray(new_ack_packet->payload, 0, sizeof(new_ack_packet->payload));
    new_ack_packet->checksum = calc_checksum(new_ack_packet);
    
    tolayer3(1, *new_ack_packet);
    if(packet.seqnum == seq_b){
      tolayer5(1, packet.payload);
      seq_b = packet.seqnum + 1;
      return;
    }
  }
  else{
    return;
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  seq_b = 0;
}
