#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "dhcp.h"


const uint8_t MAGIC_COOKIE[4] = { 99, 130, 83, 99 };

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
			
		// Safely receives data form the socket
		if( (recv_data_l = recvfrom(sock, buffer, dhcp_hdr_l, 0, (struct sockaddr *) &clnt_addr, &sockaddr_l)) < 0) {
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		// Sets the header before bringing it back
		set_dhcp_hdr_from_bytes(pkg, buffer, recv_data_l);
		
		// 
		printf("Received DHCP Package\n");
		printf("Returning now\n");
		
		break;
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
}



void set_bytes_from_dhcp_hdr(dhcp_hdr* pkg, uint8_t* ptr, size_t ptr_l) {
	
	/// Host to Network Conversions
	pkg->trs_id = htonl(pkg->trs_id);
	pkg->num_s = htons(pkg->num_s);
	pkg->flags = htons(pkg->flags);
	pkg->clt_ip = htonl(pkg->clt_ip);
	pkg->own_ip = htonl(pkg->own_ip);
	pkg->srv_ip = htonl(pkg->srv_ip);
	pkg->gtw_ip = htonl(pkg->gtw_ip);
	
	size_t dhcp_hdr_l = sizeof(dhcp_hdr);
	if( ptr_l >= dhcp_hdr_l ) {
		memcpy(ptr, pkg, sizeof(dhcp_hdr));
	
	}else{
		printf("Couldn't copy dhcp_hdr to pointer, the pointer size is too small\n");
	}
	
}

void set_dhcp_opt_from_dhcp_hdr(dhcp_opt* opt, dhcp_hdr* pkg) {
	
	/// Assures these values have a safe value before changing them
	opt->cookie_status = OK;
	opt->dhcp_msg = INVALID;
	memset(opt->hst_name, 0, MAX_HOST_NAME);
	memset(opt->clt_id, 0, ETHER_ADDR_L);
	memset(opt->sub_msk, 0, IP_ADDR_L);
	opt->rnw_time = 0;
	opt->rbn_time = 0;
	memset(opt->srv_id, 0, IP_ADDR_L);
	memset(opt->rtr_id, 0, IP_ADDR_L);
	memset(opt->rqt_id, 0, IP_ADDR_L);

	uint8_t* ptr = pkg->opt;
	
	/// Magic Cookie
	size_t magic_cookie_size = 4; // bytes
	for( size_t i = 0; i < magic_cookie_size; i++) {
		if( ptr[i] != MAGIC_COOKIE[i]) {
			opt->cookie_status = NOK;
		}
	}
	ptr += magic_cookie_size;
	
	/// TODO
	/// Other Flags
	while( ptr[0] != DHCP_END_OP  ) {
		uint8_t dhcp_opt_code = ptr[0];
		uint8_t dhcp_opt_len = 1;
		switch(dhcp_opt_code) {
			case DHCP_MSG_OP:
				
				break;
			case DHCP_HST_NAME_OP:
				break;
			case DHCP_CLT_ID_OP:
				break;
			
			case DHCP_SUB_MSK_OP:
				break;
			
			case DHCP_RNW_TIME_OP:
				break;
			
			case DHCP_RBN_TIME_OP:
				break;
			
			case DHCP_IP_LEASE_TIME_OP:
				break;
			
			case DHCP_SRV_ID_OP:
				break;
			
			case DHCP_RTR_ID_OP:
				break;
			
			case DHCP_RQT_ID_OP:
				break;
		}
		ptr += dhcp_opt_len;
	}

}

void set_dhcp_hdr_from_dhcp_opt(dhcp_opt* opt, dhcp_hdr* pkg) {
	// TODO
}
