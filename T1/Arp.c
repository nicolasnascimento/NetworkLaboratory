#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
#include <pthread.h>

#include "Arp.h"
#include "Network.h"


#define BUFFER_SIZE 1500
#define ARP_ETHERTYPE ETH_P_ARP

// Keep a reference to the interfaName as it will be used by the background thread
char sharedInterfaceName[IFNAMSIZ];


void* arpSnifferLoop(void* data) {
	// Create a new raw socket
	int rawSocket = initSocketWithInterfaceName(sharedInterfaceName);
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
					
	}
}


int initArpSnifferWithInterfaceName(const char* interfaceName) {
	
	// Copies the interfaceName
	strcpy(sharedInterfaceName, interfaceName);	

	// The thread to be used
	pthread_t backgroundThread;
	
	// Initializes the sniffer loop
	if( pthread_create(&backgroundThread, NULL, arpSnifferLoop, NULL )) {
		fprintf(stderr, "Error while creating background thread for the Arp Sniffer\n");
		return -1;
	}

	return 0;
}

