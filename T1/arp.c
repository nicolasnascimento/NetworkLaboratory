#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <string.h>

#include "arp.h"

#define IP_ADDR_LEN 4 // IPV4

void arp_pkg_set(struct ether_arp* pkg, uint16_t op, uint8_t* s_haddr, uint8_t* s_paddr, uint8_t* t_haddr, uint8_t* t_paddr) {
  // Default Header
  pkg->ea_hdr.ar_hrd = 0x1;                       // Ethernet
  pkg->ea_hdr.ar_pro = 0x800;                     // Ip
  pkg->ea_hdr.ar_hln = ETHER_ADDR_LEN;            // Ethernet Address Length
  pkg->ea_hdr.ar_pln = IP_ADDR_LEN;               // Ip Address Length

  // Customizable Portion
  pkg->ea_hdr.ar_op = op;
  memcpy(pkg->arp_sha, s_haddr, ETHER_ADDR_LEN);  // Sender Hardware Address
  memcpy(pkg->arp_spa, s_paddr, IP_ADDR_LEN);     // Sender Protocol Address
  memcpy(pkg->arp_tha, t_haddr, ETHER_ADDR_LEN);  // Target Hardware Address
  memcpy(pkg->arp_tpa, t_paddr, IP_ADDR_LEN);     // Target Protocol Address
}
