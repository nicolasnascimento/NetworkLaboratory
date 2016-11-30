#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ippool.h"


#define MAX_IP 1024

in_addr_t ippool[MAX_IP];
int is_valid = 0;

union ip_key {
  uint32_t trs_id;
  uint8_t trs_id_b[4];
};

void lazy_init(void)  {
  memset(ippool, 0, sizeof(in_addr_t)*MAX_IP);
  is_valid = 1;
}

uint8_t get_key_for_trs_id(uint32_t trs_id) {
  union ip_key key;
  key.trs_id = trs_id;
  return (key.trs_id_b[0]) + (key.trs_id_b[1]) + (key.trs_id_b[2]) + (key.trs_id_b[3]);
}

void set_ip_for_trs_id(in_addr_t ip, uint32_t trs_id) {
  if( !is_valid ) {
    lazy_init();
  }

  ippool[get_key_for_trs_id(trs_id)] = ip;
}

in_addr_t get_ip_for_trs_id(uint32_t trs_id) {
  if( !is_valid ) {
    lazy_init();
  }
  return ippool[get_key_for_trs_id(trs_id)];
}
