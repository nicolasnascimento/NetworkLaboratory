#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

#include "Util.h"
#include "Network.h"
#include "Arp.h"

// IPV4
#define IP_ADDRESS_LENGTH 4
#define MAC_ADDRESS_LENGTH 6

// Program Constants
const char broadcastMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char routerIpAddress[] = {0xA, 0x20, 0x8F, 0x01};
const char broadcastIpAddress[] = {0xA, 0x20, 0x8F, 0xFF};

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
	initSharedSocketWithInterfaceName(interfaceName);
	
	// Initializes the background thread to print Arp Packages
	initArpSnifferWithInterfaceName(interfaceName);		
	ArpPackage initialPackage = {
		ARPHRD_ETHER,
		ETH_P_IP,
		MAC_ADDRESS_LENGTH,
		IP_ADDRESS_LENGTH,
		ARPOP_REQUEST,	
	};
	

	memcpy(initialPackage.senderMacAddress, localInterfaceMacAddress, MAC_ADDRESS_LENGTH);
	memcpy(initialPackage.senderIpAddress, localInterfaceIpAddress, IP_ADDRESS_LENGTH);
	memset(initialPackage.targetMacAddress, 0, IP_ADDRESS_LENGTH);
	memcpy(initialPackage.targetIpAddress, broadcastIpAddress, IP_ADDRESS_LENGTH);
	
	
	sendArpPackage(&initialPackage);

	receiveArpPackage(&initialPackage, ARPOP_REPLY);	

	// End of program
	exit(EXIT_SUCCESS);
}
