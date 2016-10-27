#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "dhcp.h"


void wait_dhcp_hdr(dhcp_hdr* pkg) {
	
	int sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int recv_data_l;
	unsigned sockaddr_l = sizeof(struct sockaddr_in);	
	int dhcp_hdr_l = sizeof(dhcp_hdr);
	uint8_t buffer[dhcp_hdr_l];

	// Initializes socket
	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Local Address Setup
	memset(&serv_addr, 0, sockaddr_l);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DHCP_PRT_NUM);
	
	// Binds Local Addres
	if(bind(sock, (struct sockaddr *) &serv_addr, sockaddr_l) < 0 ) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
		
	// Begin Receiving Packages
	while(1) {
		// Ensures The buffer is empty before doing any operation
		memset(buffer, 0, dhcp_hdr_l);
			
	
		if( (recv_data_l = recvfrom(sock, buffer, dhcp_hdr_l, 0, (struct sockaddr *) &clnt_addr, &sockaddr_l)) < 0) {
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
	
		//set_dhcp_hdr_from_bytes(pkg, buffer, recv_data_l);
		
	
		//printf("Received DHCP Package\n");
	
		
	}
}

void set_dhcp_hdr_from_bytes(dhcp_hdr* pkg, uint8_t* ptr, size_t ptr_l) {
	
	// Copies data from the pointer
	memcpy(pkg, ptr, ptr_l);
	
	// Apropriate Network-to-Host Conversions
	pkg->trs_id = ntohl(pkg->trs_id);
	pkg->num_s = ntohs(pkg->num_s);
	pkg->flags = ntohs(pkg->flags);
	pkg->clt_ip = ntohl(pkg->clt_ip);
	pkg->own_ip = ntohl(pkg->own_ip);
	pkg->srv_ip = ntohl(pkg->srv_ip);
	pkg->gtw_ip = ntohl(pkg->gtw_ip);
	pkg->clt_hrd_addr = ntohs(pkg->clt_hrd_addr);

	// Warning - assuming Magic Cookie will always be present
	uint8_t* opt_ptr = pkg->opt;
	size_t len = ( ptr_l - OPT_MAX_L );
	
	/// Begin in the byte four( Assuming Magic Cookie will always be present), then check for flags
	for( int i = 4; i < len; i++ ) {
		uint8_t opt_code = *(opt_ptr + i);
		switch(opt_code) {
			case 53:
				printf("Type found\n");
				break;
		}
	}

}

void set_bytes_from_dhcp_hdr(dhcp_hdr* pkg, uint8_t* ptr, size_t ptr_l) {
	// TODO
}
