#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "dhcp.h"


const uint8_t MAGIC_COOKIE[4] = { 99, 130, 83, 99 };

in_addr_t wait_dhcp_hdr(dhcp_hdr* pkg) {
	
	int sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int recv_data_l;
	unsigned sockaddr_l = sizeof(struct sockaddr_in);	
	int dhcp_hdr_l = sizeof(dhcp_hdr);
	uint8_t buffer[dhcp_hdr_l];
	in_addr_t rtrn_v;

	// Initializes socket
	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Local Address Setup
	memset(&serv_addr, 0, sockaddr_l);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DHCP_PRT_NUM_S);
	
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
		
		/// Sets the returning value
		rtrn_v = clnt_addr.sin_addr.s_addr;
		
		printf("Received DHCP Package\n");
		printf("Returning now\n");
	
		break;
	}
	close(sock);
	
	return rtrn_v;
}

void send_dhcp_hdr(dhcp_hdr* pkg, in_addr_t addr) {
	int sock;
	struct sockaddr_in serv_addr;
	unsigned sockaddr_l = sizeof(struct sockaddr_in);
	int dhcp_hdr_l = sizeof(dhcp_hdr);
	uint8_t buffer[dhcp_hdr_l];
	
	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	memset(&serv_addr, 0, sockaddr_l);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(addr);
	serv_addr.sin_port = htons(DHCP_PRT_NUM_S);
	
	if( sendto(sock, buffer, dhcp_hdr_l, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != dhcp_hdr_l ) {
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	close(sock);
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

	/// Other Flags
	while( ptr[0] != DHCP_END_OP  ) {
		uint8_t dhcp_opt_code = ptr[0];
		uint8_t dhcp_opt_len = 1;
		uint8_t value = 0;
		uint8_t str_ll = 0;
		switch(dhcp_opt_code) {
			case DHCP_MSG_OP:
				value = *(ptr + 2);
				switch(value) {
					case 1: opt->dhcp_msg = DISCOVER;
						break;
					case 2: opt->dhcp_msg = OFFER;
						break;
					case 3: opt->dhcp_msg = REQUEST;
						break;
					case 4: opt->dhcp_msg = DECLINE;
						break;
					case 5: opt->dhcp_msg = ACK;
						break;
					case 6: opt->dhcp_msg = NAK;
						break;
					case 7: opt->dhcp_msg = RELEASE;
						break;
					case 8: opt->dhcp_msg = INFORM;
						break;
					default: opt->dhcp_msg = INVALID;
						break;
				}
				dhcp_opt_len = 3;
				break;
			case DHCP_HST_NAME_OP:
				memcpy(opt->hst_name, ptr + 2, str_ll);
				opt->hst_name[str_ll] = '\0';
				dhcp_opt_len = 2 + str_ll;
				break;
			case DHCP_CLT_ID_OP:
				memcpy(opt->clt_id, ptr + 2, ETHER_ADDR_L);
				dhcp_opt_len = 2 + *(ptr + 1);
				break;
			case DHCP_SUB_MSK_OP:
				memcpy(opt->sub_msk, ptr + 2, IP_ADDR_L); 
				dhcp_opt_len = 6;
				break;
			case DHCP_RNW_TIME_OP:
				memcpy(opt->rtr_id, ptr + 2, IP_ADDR_L);						
				dhcp_opt_len = 2 + *(ptr + 1);
				break;
			case DHCP_RBN_TIME_OP:
				dhcp_opt_len = 6;
				break;
			case DHCP_IP_LEASE_TIME_OP:
				dhcp_opt_len = 6;
				break;
			case DHCP_SRV_ID_OP:
				memcpy(opt->srv_id, ptr + 2, IP_ADDR_L);
				dhcp_opt_len = 2 + IP_ADDR_L;
				break;
			case DHCP_RTR_ID_OP:
				memcpy(opt->rtr_id, ptr + 2, IP_ADDR_L);
				// There could be multiple router, we'll only copy the first one
				dhcp_opt_len = 2 + *(ptr + 1);
				break;
			case DHCP_RQT_ID_OP:
				memcpy(opt->rqt_id, ptr + 2, IP_ADDR_L);
				dhcp_opt_len = 2 + IP_ADDR_L;
				break;
			/*default:
				//printf("Debug\n");*/
		}
		ptr += dhcp_opt_len;
	}

}

void set_dhcp_hdr_from_dhcp_opt(dhcp_opt* opt, dhcp_hdr* pkg) {
	
	// Begin pointing to the options field
	uint8_t* ptr = pkg->opt;
	
	// Cookie
	memcpy(ptr, MAGIC_COOKIE, 4);
	ptr += 4;
	
	// First the type
	uint8_t dhcp_msg_type = 0;
	switch( opt->dhcp_msg ) {
		case DISCOVER: dhcp_msg_type = 1;	
			break;
		case OFFER: dhcp_msg_type = 2;
			break;
		case REQUEST: dhcp_msg_type = 3;
			break;
		case DECLINE: dhcp_msg_type = 4;
			break;
		case ACK: dhcp_msg_type = 5;
			break;
		case NAK: dhcp_msg_type = 6;
			break;
		case RELEASE: dhcp_msg_type = 7;
			break;
		case INFORM: dhcp_msg_type = 8;
			break;
		default: dhcp_msg_type = 0;
			break;
	}
	if( dhcp_msg_type != 0 ) {
		ptr[0] = DHCP_MSG_OP;
		ptr[1] = 1;
		ptr[2] = dhcp_msg_type;
		ptr += 3;
	}

	// Host Name
	size_t hst_name_l = strlen(opt->hst_name);
	if( hst_name_l != 0 ) {
		ptr[0] = DHCP_HST_NAME_OP;
		ptr[1] = hst_name_l;
		memcpy(ptr + 2, opt->hst_name, hst_name_l);
		ptr += + 2 + hst_name_l;
	}
	
	// Client Identifier
	int clt_id_v = 0; // Flag to indicate if clt_id is valid
	for( int i = 0; i < ETHER_ADDR_L; i++ ) {
		if( opt->clt_id[i] != 0 ) {
			clt_id_v = 1;
			break;
		}
	}
	if( clt_id_v != 0 ) {
		ptr[0] = DHCP_CLT_ID_OP;
		ptr[1] = ETHER_ADDR_L + 1; //
		ptr[2] = 1; // Warning - Assumming this will always be Ethernet
		memcpy(ptr + 3, opt->clt_id, ETHER_ADDR_L);
		ptr += 3 + ETHER_ADDR_L;
	}
	
	// Subnet Mask
	int sub_msk_v = 0; // Flag to indicate if sub_msk is valid
	for( int i = 0; i < IP_ADDR_L; i++ ) {
		if( opt->sub_msk[i] != 0 ) {
			sub_msk_v = 1;
			break;
		}
	}
	if( sub_msk_v != 0 ) {
		ptr[0] = DHCP_SUB_MSK_OP;
		ptr[1] = IP_ADDR_L;
		memcpy(ptr + 2, opt->sub_msk, IP_ADDR_L);
		ptr += 2 + IP_ADDR_L;
	}
	// Renew Time
	if( opt->rnw_time != 0 ) {
		ptr[0] = DHCP_RNW_TIME_OP;
		ptr[1] = 4;
		memcpy(ptr + 2, &opt->rnw_time, 4);
		ptr += 2 + 4;
	}
	// Rebind Time
	if( opt->rbn_time != 0 ) {
		ptr[0] = DHCP_RBN_TIME_OP;
		ptr[1] = 4;
		memcpy(ptr + 2, &opt->rbn_time, 4);
		ptr += 2 + 4;
	}
	// Ip Lease Time
	if( opt->ip_lease_time != 0 ) {
		ptr[0] = DHCP_IP_LEASE_TIME_OP;
		ptr[1] = 4;
		memcpy(ptr + 2, &opt->ip_lease_time, 4);
		ptr += 2 + 4;
	}
	// Server Identifier
	int srv_id_v = 0;
	for( int i = 0; i < IP_ADDR_L; i++ ) {
		if( opt->srv_id[i] != 0 ) {
			srv_id_v = 1;
			break;
		}
	}
	if( srv_id_v != 0 ) {
		ptr[0] = DHCP_SRV_ID_OP;
		ptr[1] = IP_ADDR_L;
		memcpy(ptr + 2, opt->srv_id, IP_ADDR_L);
		ptr += 2 + IP_ADDR_L;
	}
	
	// Router Identifier
	int rtr_id_v = 0;
	for( int i = 0; i < IP_ADDR_L; i++ ) {
		if( opt->rtr_id[i] != 0 ) {
			rtr_id_v = 1;
			break;
		}
	}
	if( rtr_id_v != 0 ) {
		ptr[0] = DHCP_RTR_ID_OP;
		ptr[1] = IP_ADDR_L;
		memcpy(ptr + 2, opt->rtr_id, IP_ADDR_L);
		ptr += 2 + IP_ADDR_L;
	}

	// Request Identifier
	int rqt_id_v = 0;
	for( int i = 0; i < IP_ADDR_L; i++ ) {
		if( opt->rqt_id[i] != 0 ) {
			srv_id_v = 1;
			break;
		}
	}
	if( rqt_id_v != 0 ) {
		ptr[0] = DHCP_RQT_ID_OP;
		ptr[1] = IP_ADDR_L;
		memcpy(ptr + 2, opt->rqt_id, IP_ADDR_L);
		ptr += 2 + IP_ADDR_L;
	}
	
	// End of DHCP Options
	ptr[0] = DHCP_END_OP;
}
