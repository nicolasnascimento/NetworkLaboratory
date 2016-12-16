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

// Keep a reference to the interfaName as it will be used by the background thread
char sharedInterfaceName[IFNAMSIZ];

// The thread reference
pthread_t backgroundThread;


void* arpSnifferLoop() {
	printf("Initializing Arp Sniffer\n");
	
	// Create a new raw socket
	int rawSocket = initSocketWithInterfaceName(sharedInterfaceName, SET_SHARED_SOCKET_FALSE);

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
		/*printf("New Package  = [");
		for( int i = 0; i < BUFFER_SIZE; i++ ) {
			printf("%x", buffer[i]);
		}
		printf("]\n");*/
		
		EthernetPackage package = createEthernetPackageFromBuffer(buffer);
		if( package.ethertype == ARP_ETHERTYPE ) {
			printf("Arp Package:\n");
					
			ArpPackage arpPackage = createArpPackageWithEthernetPackage(package);
			
			printArpPackage(arpPackage);
		}else{
			//printf("Ehertype = %x\n", package.ethertype);
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

void copyMemoryAndAdvancePointer(void*,void*,int,unsigned char**);

ArpPackage createArpPackageWithEthernetPackage(EthernetPackage package) {
	ArpPackage arpPackage;
	unsigned char* dataPointer = package.data;
	// Copy Data
	copyMemoryAndAdvancePointer(&arpPackage.hardwareType, dataPointer, sizeof(arpPackage.hardwareType), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.protocolType, dataPointer, sizeof(arpPackage.protocolType), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.hardwareLength, dataPointer, sizeof(arpPackage.hardwareLength), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.protocolLength, dataPointer, sizeof(arpPackage.protocolLength), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.operation, dataPointer, sizeof(arpPackage.operation), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.senderMacAddress, dataPointer, sizeof(arpPackage.senderMacAddress), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.senderIpAddress, dataPointer, sizeof(arpPackage.senderIpAddress), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.targetMacAddress, dataPointer, sizeof(arpPackage.targetMacAddress), &dataPointer);
	copyMemoryAndAdvancePointer(&arpPackage.targetIpAddress, dataPointer, sizeof(arpPackage.targetIpAddress), &dataPointer);
	
	// Network to Host Conversion
	ntohArpPackage(&arpPackage);	

	return arpPackage;
}

void printMacAddress(unsigned char* array, int length) {
	for( int i = 0; i < length; i++ ) {
		if( i + 1 < length ) {
			printf("%x:", array[i]);
		}else{
			printf("%x\n", array[i]);
		}
	}
}

void htonArpPackage(ArpPackage* package) {
	package->hardwareType = htons(package->hardwareType);
	package->protocolType = htons(package->protocolType);
	package->operation = htons(package->operation);
}

void ntohArpPackage(ArpPackage* package) {
	package->hardwareType = ntohs(package->hardwareType);
	package->protocolType = ntohs(package->protocolType);
	package->operation = htons(package->operation);
}

void printIpAddress(unsigned char* array, int length) {
	for( int i = 0; i < length; i++ ) {
		if( i + 1 < length ) {
			printf("%u.",array[i]);
		}else{
			printf("%u\n", array[i]);
		}
	}
}

void printArpPackage(ArpPackage package) {
	printf("Hardware Type = %u\n", ntohs(package.hardwareType));
	printf("Protocol Type = %u\n", ntohs(package.protocolType));
	printf("Hardware Length = %u\n", package.hardwareLength);
	printf("Protocol Length = %u\n", package.protocolLength);
	printf("Operation = %x\n", ntohs(package.operation));
	printf("Sender Mac = ");
	printMacAddress(package.senderMacAddress, sizeof(package.senderMacAddress));
	printf("Sender Ip = ");
	printIpAddress(package.senderIpAddress, sizeof(package.senderIpAddress));
	printf("Target Mac = ");
	printMacAddress(package.targetMacAddress, sizeof(package.targetMacAddress));	
	printf("Target Ip = ");
	printIpAddress(package.targetIpAddress, sizeof(package.targetIpAddress));
}

void copyMemoryAndAdvancePointer(void* destination, void* source, int length, unsigned char** dataPointer) {
	memcpy(destination, source, length);
	(*dataPointer) += length;
}


