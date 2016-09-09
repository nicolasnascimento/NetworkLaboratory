#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>

#include "Util.h"
#include "Network.h"
#include "Arp.h"

// IPV4
#define IP_ADDRESS_LENGTH 4

// These will be shared across the application
char interfaceName[IFNAMSIZ];
char targetIpAddress[IP_ADDRESS_LENGTH];


int main(int argc, const char* argv[]) {		

	// Safety check
	if( argc != 3 ) {
		printf("Usage: ./run <InterfaceName> <TargetIpAddress>\n");
		return EXIT_FAILURE;
	}
	strcpy(interfaceName, argv[1]);
	strcpy(targetIpAddress, argv[2]);
	
	// Initializes the shared Socket with the provided interfaceName
	initSharedSocketWithInterfaceName((const char*)interfaceName);
	
	// Initializes the background thread to print Arp Packages
	initArpSnifferWithInterfaceName((const char*)interfaceName);
	
	

	while(1);
	// End of program
	exit(EXIT_SUCCESS);
}
