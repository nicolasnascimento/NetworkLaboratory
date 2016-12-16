#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>


#include "util.h"
#include "ethernet.h"

// The target Ip Address
struct in_addr t_paddr;

void deinit() {
    d_printf("Deinit\n");
}

void deinit_it(int code) {
  exit(EXIT_SUCCESS);
}

void init() {
    d_printf("Init\n");

    // Register ctrl+c interruption
    struct sigaction act;
    act.sa_handler = deinit_it;
    sigaction(SIGINT, &act, NULL);

    // Register exit function
    atexit(deinit);

    // Initilize Ethernet Interface
    eth_init();
}

void arp_poision() {
  d_printf("Target Ip: %s\n", str_v);

  // An empty arp package
  struct ether_arp arphdr;

  // An empty space for the Ethernet Frame
  eth_frm* frame = malloc(sizeof(eth_frm));

  

}

void init_loop() {

  arp_poision();

  eth_frm* frame = malloc(sizeof(eth_frm));
  while (1) {
    size_t num_bytes = wait_eth_frame(&frame);
    d_printf("{Ethernet");
    if( ntohs(frame->hdr.ether_type) == ETHERTYPE_ARP ) {
      struct ether_arp* arphdr = (struct ether_arp*)frame->payload;

    }else{
      d_printf("-*");
    }
    d_printf("} - %u\n", num_bytes);
  }
}

int main(int argc, char * argv[]) {

    // Gets initial flags
    get_initial_flags(argc, argv);

    // Initializations
    init();

    // Begin Main Thread Loop
    init_loop();

}
