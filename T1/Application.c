#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#include "Util.h"
#include "Network.h"

// IPV4
#define IP_ADDRESS_LENGTH 4

// These will 
char interfaceName[IFNAMSIZ];
char targetIpAddres[IP_ADDRESS_LENGTH];


int main(int argc, const char* argv[]) {		

	// Safety check
	if( argc != 3 ) {
		printf("Usage: ./run <InterfaceName> <TargetIpAddress>\n");
		return EXIT_FAILURE;
	}
	strcpy(interfaceName, argv[1]);
	strcpy(targetIpAddress, argv[2]);
	
	// Initialized Socket with the provided interfaceName
	initSharedSocketWithInterfaceName((const char*)interfaceName);
	
	
	
	// End of program
	exit(EXIT_SUCESS);
}
