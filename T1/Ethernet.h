#ifndef ETHERNET_H
#define ETHERNET_H

/// Defines a standard Ethernet Package and its fields
typedef struct EthernetPackage {
	// 6 Bytes for the Mac Address
	unsigned char destinationMac[6];
	unsigned char sourceMac[6];
	// 2 bytes for the EtherType
	short int ethertype;
	// A pointer to the data
	unsigned char* data;
} EthernetPackage;


/// Creates an Ethernet Package from the standar buffer
EthernetPackage createEthernetPackageFromBuffer(unsigned char* buffer);



#endif // ETHERNET_H
