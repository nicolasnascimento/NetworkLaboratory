#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "dhcp.h"


void wait_dhcp_pkg(dhcp_hdr* pkg) {
	
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
			
	
		if( recv_data_l = recvfrom(sock, buffer, dhcp_hdr_l, 0, (struct sockaddr *) &clnt_addr, &sockaddr_l)) {
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
	
		// TODO - Check incoming package
	}
}

void set_dhcp_hdr_from_bytes(dhcp_hdr* pkg, uint8_t* ptr) {
	// TODO
}

void set_bytes_from_dhcp_hdr(dhcp_hdr* pkg, uint8_t* ptr) {
	// TODO
}
