#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>

/// The following header was based on the DHCP header file which can be found at: 
/// http://www.networksorcery.com/enp/protocol/dhcp.htm#Boot%20filename

/// Amount of bytes to be used to represent the host name
#define SRV_HST_L 64

/// Amount of bytes to be used to represent the boot file name
#define BOOT_FLN_L 128

/// This package will be used to capture incoming packages
typedef struct {
	uint8_t opcode;				// 0x1 = REQUEST, 0x2 = REPLY
	uint8_t hrd_t;				// 0x1 = Ethernet
	uint8_t hrd_addr_l;
	uint8_t hop_c;				
	uint32_t trs_id;
	uint16_t num_s;
	uint16_t flags;				// if MSB = 1, Broacast
	uint32_t clt_ip;
	uint32_t own_ip;
	uint32_t srv_ip;
	uint32_t gtw_ip;
	uint16_t clt_hrd_addr;			
	uint8_t srv_hst[SRV_HST_L];
	uint8_t boot_fln[BOOT_FLN_L];
	// Fields definitions
} dhcp_hdr;


/// This is a blocking calling, it will begin looking for dhcp packages in the network
void wait_dhcp_hdr(dhcp_hdr*);

/// This will get all data from the network and set the struct with the appropriate values
void set_dhcp_hdr_from_bytes(dhcp_hdr*, uint8_t*);

/// This will get all data form the struct and set the values pointed by the 
void set_bytes_from_dhcp_hdr(dhcp_hdr*, uint8_t*);


#endif // DHCP_H
