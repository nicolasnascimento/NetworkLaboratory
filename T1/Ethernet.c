#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Ethernet.h"

EthernetPackage createEthernetPackageFromBuffer(unsigned char* buffer) {
	EthernetPackage package;
	
	// Use the destination Mac to determine both sizes
	int macAddressLength = sizeof(package.destinationMac);
	int ethertypeLength = sizeof(package.ethertype);

	memcpy(package.destinationMac, buffer, macAddressLength);
	memcpy(package.sourceMac, buffer + macAddressLength, macAddressLength);
	memcpy(&package.ethertype, buffer + 2*macAddressLength, ethertypeLength);
	package.data = buffer + 2*macAddressLength + ethertypeLength;
	
	return package;
}

