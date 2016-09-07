#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include "Network.h"

int sharedSocket = -1;


int initSharedSocketWithInterfaceName(const char* interfaceName) {
	sharedSocket = initSocketWithInterfaceName(interfaceName);
}

int initSocketWithInterfaceName(const char* interfaceName) {

	// We will set this interface request as we obtain
	// Data from the local network interface	
	struct ifreq interfaceRequest;
	strcpy(interfaceRequest.ifr_name, interfaceName);

	// Create the standard raw socket
	// Last flag allow to get the Ethernet package from the Local Network
	sharedSocket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if( sharedSocket < 0 ) {
		perror("Error in initial socket creation");
		return -1;
	}
	// Gets Flags for the interface
	if( ioctl(sharedSocket, SIOCGIFINDEX, &interfaceRequest) < 0 ) {
		perror("Error in interface index obtainance");
		return -1;	
	}
	
	/// Gets the initial for the interface
	if( ioctl(sharedSocket, SIOCGIFFLAGS, &interfaceRequest) < 0 ) {
		perror("Error in interface flags obtainance");
		return -1;
	}

	// Sets the interface to promiscuos
	interfaceRequest.ifr_flags |= IFF_PROMISC;
	if( ioctl(sharedSocket, SIOCSIFFLAGS, &interfaceRequest) < 0 ) {
		perror("Error while setting interface to promiscuos");
		return -1;
	}
	
	return 0;
}

int sendData(const char* buffer, const char* destinationIpAddress) {

	return -1;
}

int receiveData(char* buffer, const char* sourceIpAddress) {
	
	return -1;
}

