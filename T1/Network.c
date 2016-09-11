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
#include "Arp.h"
#include "Ethernet.h"


// Shared Variables
int sharedSocket = -1;
struct sockaddr_ll sharedSocketAddress;
unsigned char localInterfaceMacAddress[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char localInterfaceIpAddress[4] = {0x0, 0x0, 0x0, 0x0};



int initSharedSocketWithInterfaceName(const char* interfaceName) {
	sharedSocket = initSocketWithInterfaceName(interfaceName, SET_SHARED_SOCKET_TRUE);
	return sharedSocket;
}

int initSocketWithInterfaceName(const char* interfaceName, int isShared) {

	// We will set this interface request as we obtain
	// Data from the local network interface	
	struct ifreq interfaceRequest;
	strcpy(interfaceRequest.ifr_name, interfaceName);

	// Create the standard raw socket
	// Last flag allow to get the Ethernet package from the Local Network
	int rawSocket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if( rawSocket < 0 ) {
		perror("Error in initial socket creation");
		return -1;
	}
	// Gets the index for the interface
	if( ioctl(rawSocket, SIOCGIFINDEX, &interfaceRequest) < 0 ) {
		perror("Error in interface index obtainance");
		return -1;	
	}

	/// Gets the initial flags for the interface
	if( ioctl(rawSocket, SIOCGIFFLAGS, &interfaceRequest) < 0 ) {
		perror("Error in interface flags obtainance");
		return -1;
	}
	
	/// Gets the Mac Address for the local interface
	if( ioctl(rawSocket, SIOCGIFHWADDR, &interfaceRequest) < 0 ) {
		perror("Error while getting Mac for the local interface");
		return -1;
	}
	
	/// Only set this if doing it for the shared socket
	if( isShared == SET_SHARED_SOCKET_TRUE ) {
		
		sharedSocketAddress.sll_ifindex = interfaceRequest.ifr_ifindex;
		sharedSocketAddress.sll_halen = ETH_ALEN; // Ethernet
		memcpy(localInterfaceMacAddress, interfaceRequest.ifr_hwaddr.sa_data, MAC_ADDRESS_LENGTH);
		
	} 

	/// Gets the Ip Address for the local interface
	if( ioctl(rawSocket, SIOCGIFADDR, &interfaceRequest) < 0 ) {
		perror("Error while obtaining Ip Address for Local Interface");
		return -1;
	}
	 
	if( isShared == SET_SHARED_SOCKET_TRUE ) {
		struct sockaddr_in* pointer = (struct sockaddr_in*)&interfaceRequest.ifr_addr;
		const char* stringIp = inet_ntoa(pointer->sin_addr);
		memcpy(localInterfaceIpAddress, stringIp, strlen(stringIp));
	}

	// Sets the interface to promiscuos
	interfaceRequest.ifr_flags |= IFF_PROMISC;
	if( ioctl(rawSocket, SIOCSIFFLAGS, &interfaceRequest) < 0 ) {
		perror("Error while setting interface to promiscuos");
		return -1;
	}

	return rawSocket;
}


int closeSharedSocket() {
	return closeSocketWithFileDescriptor(sharedSocket);
}

int closeSocketWithFileDescriptor(int fileDescriptor) {
	int operationCode = close(fileDescriptor);
	if( operationCode < 0 ) {
		perror("Error while closing socket with file Descriptor");
	}
	return operationCode;
}

int sendArpPackage(ArpPackage* package) {
		
	unsigned short arpEtherType = ARP_ETHERTYPE;
	arpEtherType = htons(arpEtherType);
	int frameLength = 0;
	// Initializes an empty buffer
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	
	// Prepares Package before sending it
	htonArpPackage(package);
	// Creates the Ethernet Header
	memcpy(sharedSocketAddress.sll_addr, package->targetMacAddress, MAC_ADDRESS_LENGTH);
	
	memcpy(buffer, package->targetMacAddress, MAC_ADDRESS_LENGTH);
	memcpy(buffer, localInterfaceMacAddress, MAC_ADDRESS_LENGTH);
	memcpy(buffer, &arpEtherType, sizeof(arpEtherType));
	
	memcpy(buffer, package, sizeof(ArpPackage));
	frameLength = 2*MAC_ADDRESS_LENGTH + sizeof(arpEtherType) + sizeof(ArpPackage);
	

	// Sends the package
	if( sendto(sharedSocket, buffer, frameLength, 0, (struct sockaddr *) &sharedSocketAddress, sizeof(struct sockaddr_ll)) < 0 ) {
		perror("Error while sending ArpPackage");
		return -1;
	}
	
	// Undo Network operation
	ntohArpPackage(package);
	
	return 0;
}

int receiveArpPackage(ArpPackage* package, unsigned short operation) {
	// Initialized an emtpy buffer
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	
	
	while(1) {
		if(recv(sharedSocket, buffer, BUFFER_SIZE, 0) < 0 ) {
			perror("Error while receiving ArpPackage");
			return -1;
		}
		EthernetPackage ethernetPackage = createEthernetPackageFromBuffer(buffer);	
		if( ethernetPackage.ethertype == ARP_ETHERTYPE ) {
			ArpPackage arpPackage = createArpPackageWithEthernetPackage(ethernetPackage);
			if( operation == arpPackage.operation ) {
				memcpy(package, &arpPackage, sizeof(ArpPackage));
				break;
			}
			
		}
	}
	
	return 0;
}

