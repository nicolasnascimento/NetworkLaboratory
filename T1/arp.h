#ifndef ARP_H
#define ARP_H

#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

#define REQUEST   ARPOP_REQUEST
#define REPLY     ARPOP_REPLY

/// Constructios an Arp packge with the passed args
void arp_pkg_set(struct ether_arp* pkg, uint16_t op, uint8_t* s_haddr, uint8_t* s_paddr, uint8_t* t_haddr, uint8_t* t_paddr);

#endif // ARP_H
