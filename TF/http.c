#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"

size_t set_addr_from_get_buf(char* addr, char* buffer) {

  char* host_ptr = NULL;
  char* sub_dom_ptr = NULL;
  if( (host_ptr = strstr(buffer, "Host: ")) != NULL && (sub_dom_ptr = strstr(buffer, "/")) != NULL ) {
    host_ptr += 6; // Jump "Host: "
    size_t host_l = 0;
    size_t sub_dom_l = 0;
    uint32_t i = 0;
    while (host_ptr[i] != '\n' && host_ptr[i] != '\r') {
      host_l++;
      i++;
    }
    i = 0;
    while (sub_dom_ptr[i] != ' ' && sub_dom_ptr[i] != ' ') {
      sub_dom_l++;
      i++;
    }
    memset(addr, 0, host_l + sub_dom_l + 1); // + 1 for the '\0'
    memcpy(addr, host_ptr, host_l);
    memcpy(addr + host_l, sub_dom_ptr, sub_dom_l);
    return host_l + sub_dom_l;
  }else{
    return 0;
  }
}
