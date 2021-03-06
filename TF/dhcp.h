#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>

/// The following header was based on the DHCP header file which can be found at:
/// http://www.networksorcery.com/enp/protocol/dhcp.htm#Boot%20filename
/// https://support.microsoft.com/en-us/kb/169289
/// https://technet.microsoft.com/en-us/library/cc977584.aspx

#define CLT_HRD_ADDR_L 16
/// Amount of bytes to be used to represent the host name
#define SRV_HST_L 64

/// Amount of bytes to be used to represent the boot file name
#define BOOT_FLN_L 128

/// The standard DHCP Port Number
#define DHCP_PRT_NUM_S 67 // Server Port
#define DHCP_PRT_NUM_C 68 // Client

/// The maximum length for the options field
#define OPT_MAX_L 312

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
	uint8_t clt_hrd_addr[CLT_HRD_ADDR_L];
	uint8_t srv_hst[SRV_HST_L];
	uint8_t boot_fln[BOOT_FLN_L];

	// Variable Length Field
	uint8_t opt[OPT_MAX_L];
} dhcp_hdr;


/// Magic Cookie Constant
extern const uint8_t MAGIC_COOKIE[4];
/// Possible DHCP Message Types
typedef enum{ DISCOVER, OFFER, REQUEST, DECLINE, ACK, NAK, RELEASE, INFORM, INVALID } dhcp_msg_t;
/// The magic cookie status
typedef enum{ OK, NOK } cookie_status_t;
/// Maximum Length for Host Name
#define MAX_HOST_NAME OPT_MAX_L
/// Default Ethernet Mac address length
#define ETHER_ADDR_L 6
/// Default IPV4 Address Length
#define IP_ADDR_L 4

/// This package defines some commom opt that can be found in the "Options" portion of the dhcp header
typedef struct {
	cookie_status_t cookie_status;
	dhcp_msg_t dhcp_msg;
	char hst_name[MAX_HOST_NAME];
	uint8_t clt_id[ETHER_ADDR_L];		// WARNING - Assuming that this field will be filled with Ethernet Mac Addresses
	uint8_t sub_msk[IP_ADDR_L];
	uint32_t rnw_time;
	uint32_t rbn_time;
	uint32_t ip_lease_time;
	uint8_t srv_id[IP_ADDR_L];
	uint8_t rtr_id[IP_ADDR_L];
	uint8_t rqt_id[IP_ADDR_L];
	uint8_t dns_id[IP_ADDR_L];
} dhcp_opt;

/// Some commom DHCP Opt Field Opcodes
#define DHCP_MSG_OP 53
#define DHCP_HST_NAME_OP 12
#define DHCP_CLT_ID_OP 61
#define DHCP_SUB_MSK_OP 1
#define DHCP_RNW_TIME_OP 58
#define DHCP_RBN_TIME_OP 59
#define DHCP_IP_LEASE_TIME_OP 51
#define DHCP_SRV_ID_OP 54
#define DHCP_RTR_ID_OP 3
#define DHCP_RQT_ID_OP 50
#define DHCP_DNS_OP 6
#define DHCP_END_OP 255

/// This is a blocking calling, it will begin looking for dhcp packages in the network.
/// As soon as it finds one, it will return the inet_addr value with the ip address and set the dhcp_hdr struct
in_addr_t wait_dhcp_hdr(dhcp_hdr*);

/// This will send the dhcp header through the network
void send_dhcp_hdr(dhcp_hdr*, in_addr_t);

/// This will get all data from the network and set the struct with the appropriate values
void set_dhcp_hdr_from_bytes(dhcp_hdr*, uint8_t*, size_t);

/// This will get all data form the struct and set the values pointed by the
void set_bytes_from_dhcp_hdr(dhcp_hdr*, uint8_t*, size_t);

/// This will set the dhcp opt struct using the dhcp header struct provided
void set_dhcp_opt_from_dhcp_hdr(dhcp_opt*, dhcp_hdr*);

/// This will append the valid options to the dhcp header options field
void set_dhcp_hdr_from_dhcp_opt(dhcp_opt*, dhcp_hdr*);

#endif // DHCP_H
