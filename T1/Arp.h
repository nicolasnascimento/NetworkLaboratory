#ifndef ARP_SNIFFER_H
#define ARP_SNIFFER_H 


typedef struct ArpPackage {
	// 2 bytes - This will be Ethernet 10 Mbps
	unsigned short hardwareType;
	
	// 2 bytes - This will be Ipv4
	unsigned short protocolType;

	// 1 bytes - This will be 6 (Ethernet)
	unsigned char hardwareLength;
		
	// 1 byte
	unsigned char protocolLength;
} ArpPackage;

/// Initializes a background thread that will monitor the interface. 
/// The thread willl print all fields of incoming ARP Messages
int initArpSnifferWithInterfaceName(const char* interfaceName);

#endif
