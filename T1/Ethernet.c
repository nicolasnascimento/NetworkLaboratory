#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Ethernet.h"

void printEthernetPackage(EthernetPackage* package) {
	printf("Destination = ");
	for( int i = 0; i < 6; i++ ) {
		printf("%x", package->destinationMac[i]);
	}
	printf("\n");
	printf("Source = ");
	for( int i = 0; i < 6; i++) {
		printf("%x", package->sourceMac[i]);
	}
	printf("\n");
	printf("Ethertype = %u\n", package->ethertype);
}

EthernetPackage createEthernetPackageFromBuffer(unsigned char* buffer) {
	EthernetPackage package;
	
	// Use the destination Mac to determine both sizes
	int macAddressLength = sizeof(package.destinationMac);
	int ethertypeLength = sizeof(package.ethertype);
	
	memcpy(package.destinationMac, buffer, macAddressLength);
	memcpy(package.sourceMac, buffer + macAddressLength, macAddressLength);
	memcpy(&package.ethertype, buffer + 2*macAddressLength, ethertypeLength);
	package.data = buffer + 2*macAddressLength + ethertypeLength;
	
	//printf("New Ethernet Package\n");
	//printEthernetPackage(&package);
	return package;
}

