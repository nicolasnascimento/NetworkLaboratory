
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/bpf.h>
#include <ifaddrs.h>

#include "ethernet.h"

char _ifa_name[IFNAMSIZ];
size_t _buf_size;
int bpf;
struct in_addr _y_ip;
struct in_addr _brd_addr;
struct in_addr _sub_addr;

int get_ifa_name() {
	// Tries to get an interface name to attach to
	struct ifaddrs* addrs, *it;
	getifaddrs(&addrs);
	it = addrs;
	memset(_ifa_name, 0, IFNAMSIZ);
	while(it) {
		if( it->ifa_addr && it->ifa_addr->sa_family == AF_INET && strstr(it->ifa_name, "lo") == NULL ) {
			struct sockaddr_in *sock_addr = (struct sockaddr_in *)it->ifa_addr;
      struct sockaddr_in *sock_brd_addr = (struct sockaddr_in *)it->ifa_broadaddr;
      struct sockaddr_in *sock_sub_addr = (struct sockaddr_in *)it->ifa_netmask;
      _y_ip = sock_addr->sin_addr;
      _brd_addr = sock_brd_addr->sin_addr;
      _sub_addr = sock_sub_addr->sin_addr;
      strcpy(_ifa_name, it->ifa_name);
			break;
		}
		it = it->ifa_next;
	}
	freeifaddrs(addrs);
	if( strcmp(_ifa_name, "") != 0 ) {
		return 0;
	}else{
		return -1;
	}
}


int eth_init() {
	// Gets the name for the interface
	// It will be used later
	if( get_ifa_name() < 0 ) {
		return -1;
	}

	// Tries to open the next avaiable device
	char buf[11];
	int fnd_if = 0;
	for( int i = 0; i < 99; i++ ) {
		sprintf(buf, "/dev/bpf%i", i);
		if( (bpf = open(buf, O_RDWR)) < 0 ) {
			continue;
		}else{
			fnd_if = 1;
			break;
		}
	}
	if( !fnd_if ) {
		return -1;
	}
	// Bound interface
	struct ifreq bound_if;
	strcpy(bound_if.ifr_name, _ifa_name);
	if(ioctl(bpf, BIOCSETIF, &bound_if) > 0) {
		return -1;
	}

	_buf_size = 1;
	if( ioctl(bpf, BIOCIMMEDIATE, &_buf_size) < 0 ) {
		return -1;
	}
	if( ioctl(bpf, BIOCGBLEN, &_buf_size) < 0 ) {
		return -1;
	}
	return 0;
}

size_t wait_eth_frame(eth_frm** frame) {
	struct bpf_hdr bpf_buf[_buf_size];
	struct bpf_hdr* bpf_packet;
	size_t read_bytes = 0;
	size_t max_l = sizeof(struct ether_header) + (ETHER_MAX_LEN - ETHER_HDR_LEN);

	memset(bpf_buf, 0, _buf_size);
	if((read_bytes = read(bpf, bpf_buf, _buf_size)) > 0) {
		char* ptr = (char*)bpf_buf;
		bpf_packet = (struct bpf_hdr*)ptr;
		size_t c_bytes = read_bytes < max_l ? read_bytes : max_l;
		memcpy((*frame), (eth_frm*)((char*) bpf_packet + bpf_packet->bh_hdrlen), c_bytes);
		ptr += BPF_WORDALIGN(bpf_packet->bh_hdrlen + bpf_packet->bh_caplen);
		return c_bytes;
	}
	return 0;
}

int write_eth_frame(eth_frm* frame) {
	return write(bpf, frame, sizeof(eth_frm));
}

void print_mac_addr(uint8_t* src) {
	const unsigned ETHER_ADDR_L = 6;
	for( unsigned i = 0; i < ETHER_ADDR_L; i++ ) {
		printf("%x", src[i]);
		if( i + 1 < 6 )  {
			printf(".");
		}else{
			printf("\n");
		}
	}
}

void print_eth_frame(eth_frm* frame) {
	printf("Destination Mac Address: ");
	print_mac_addr(frame->hdr.ether_dhost);
	printf("Source Mac Address: ");
	print_mac_addr(frame->hdr.ether_shost);
	printf("EtherType: 0x%hx\n", ntohs(frame->hdr.ether_type));
}
