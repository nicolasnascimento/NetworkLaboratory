#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
#include <pthread.h>

#include "Arp.h"
#include "Network.h"
#include "Ethernet.h"


#define BUFFER_SIZE 1500
#define ARP_ETHERTYPE ETH_P_ARP

// Keep a reference to the interfaName as it will be used by the background thread
char sharedInterfaceName[IFNAMSIZ];

// The thread reference
pthread_t backgroundThread;


void* arpSnifferLoop() {
	printf("Initializing Arp Sniffer\n");
	
	// Create a new raw socket
	int rawSocket = initSocketWithInterfaceName(sharedInterfaceName);

	if( rawSocket < 0 ) {
		perror("Error while creating raw socket");
		return NULL;
	}
	// The buffer to store incoming packages
	unsigned char buffer[BUFFER_SIZE];
	
	// Begin receiving packages from the Network
	while(1) {
		if( recv(rawSocket, (char*) &buffer, BUFFER_SIZE, 0) < 0 ) {
			// Error handling
			perror("Error while receiving packages from the Network");
			close(rawSocket);
			return NULL;
		}
		
		EthernetPackage package = createEthernetPackageFromBuffer(buffer);
		if( package.ethertype == ARP_ETHERTYPE ) {
			printf("Arp Package:\n");
					
			struct arphdr arpHeader;
			memcpy(&arpHeader, package.data, sizeof(arpHeader));
			arpHeader.ar_hrd = ntohs(arpHeader.ar_hrd);
			arpHeader.ar_pro = ntohs(arpHeader.ar_pro);
			arpHeader.ar_hln = ntohs(arpHeader.ar_hln);
			arpHeader.ar_pln = ntohs(arpHeader.ar_pln);
			arpHeader.ar_op = ntohs(arpHeader.ar_op);
			
								
		}					
	}
}


int initArpSnifferWithInterfaceName(const char* interfaceName) {
	
	// Copies the interfaceName
	strcpy(sharedInterfaceName, interfaceName);	
	
	// Initializes the sniffer loop
	if( pthread_create(&backgroundThread, NULL, arpSnifferLoop, NULL )) {
		fprintf(stderr, "Error while creating background thread for the Arp Sniffer\n");
		return -1;
	}

	return 0;
}

