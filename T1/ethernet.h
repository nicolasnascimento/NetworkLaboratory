#include <sys/types.h>
#include <net/ethernet.h>
#include <net/bpf.h>
#include <net/if.h>
#include <sys/types.h>

/// Defines an arbitrary Ethernet Frame
typedef struct  {
	struct ether_header hdr;
	uint8_t payload[ETHER_MAX_LEN - ETHER_HDR_LEN];
} eth_frm;

/// The name of the associated interface
extern char _ifa_name[IFNAMSIZ];

// Interface associated Values for Ip, Broadcast Address & Sub-Net Mask
extern struct in_addr _y_ip;
extern struct in_addr _brd_addr;
extern struct in_addr _sub_addr;

/// The default size for the bpf file descriptor
extern size_t _buf_size;
/// The associated Berkeley Packet Filter file descriptor
extern int bpf;

/// Initializes and configures the BPF Device
int eth_init();

/// waits to receive an ethrenet frame from the network
/// this should be placed in a while loop so that all ethernet frmaes are captured
size_t wait_eth_frame(eth_frm**);

/// sends the ethernet frame using throught the network
int write_eth_frame(eth_frm*);

/// prints the ethernet frame to standard output
void print_eth_frame(eth_frm*);
