#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#include "Util.h"
#include "Network.h"

// IPV4
#define IP_ADDRESS_LENGTH 4

int main(int argc, const char* argv[]) {

	// The name of the local interface
	char interfaceName[IFNAMSIZ];
	// The target IpAddress
	char targetIpAddress[IP_ADDRESS_LENGTH];	

	// Safety check
	if( argc != 3 ) {
		printf("Usage: ./run <InterfaceName> <TargetIpAddress>\n");
		return EXIT_FAILURE;
	}
	strcpy(interfaceName, argv[1]);
	strcpy(targetIpAddress, argv[2]);
	
	// Initialized Socket with the provided interfaceName
	initSharedSocketWithInterfaceName((const char*)interfaceName);
	
}
