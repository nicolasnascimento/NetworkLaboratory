#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>

/// The following header was based on the DHCP header file which can be found at: 
/// http://www.networksorcery.com/enp/protocol/dhcp.htm#Boot%20filename

/// Amount of bytes ot be used to represent the host name
#define SRV_HST_L 64 
#define BOOT_FLN_L 128

/// This package will be used to capture incoming packages
typedef struct {
	uint8_t opcode;
	uint8_t hrd_t;
	uint8_t hrd_addr_l;
	uint8_t hop_c;
	uint32_t trs_id;
	uint16_t num_s;
	uint16_t flags;
	uint32_t clt_ip;
	uint32_t own_ip
	uint32_t srv_ip;
	uint32_t gtw_ip;
	uint16_t clt_hrd_addr;
	uint8_t srv_hst[SRV_HST_L];
	uint8_t boot_fln[BOOT_FLN_L];
	// Fields definitions
} dhcp_header;



#endif
