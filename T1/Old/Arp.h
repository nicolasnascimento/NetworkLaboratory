#ifndef ARP_SNIFFER_H
#define ARP_SNIFFER_H 


#define ARP_ETHERTYPE ETH_P_ARP

#include "Ethernet.h"

/// Defines a Arp Package
/// It assumes Ethernet & IPv4
typedef struct ArpPackage {
	// 2 bytes - This will be Ethernet 10 Mbps
	unsigned short hardwareType;
	
	// 2 bytes - This will be Ipv4
	unsigned short protocolType;

	// 1 bytes - This will be 6 (Ethernet)
	unsigned char hardwareLength;
		
	// 1 byte - This will be 4 (Ipv4)
	unsigned char protocolLength;
	
	// 2 bytes - This will be the Either Request or Reply
	unsigned short operation;
	
	// 6 bytes - Sender Mac
	unsigned char senderMacAddress[6];

	// 4 bytes - Sender Ip
	unsigned char senderIpAddress[4];
	
	// 6 bytes - Target Mac
	unsigned char targetMacAddress[6];
	
	// 4 bytes - Target Ip
	unsigned char targetIpAddress[4];
} ArpPackage;

/// Initializes a background thread that will monitor the interface. 
/// The thread willl print all fields of incoming ARP Messages
int initArpSnifferWithInterfaceName(const char* interfaceName);

/// Creates a new Arp Package from the Ethernet package
ArpPackage createArpPackageWithEthernetPackage(EthernetPackage package);

/// Prints all Fields for the ArpPackage
void printArpPackage(ArpPackage package);

/// Host-To-Network for Arp Package
void htonArpPackage(ArpPackage* package);

/// Network-to-host for Arp Package
void ntohArpPackage(ArpPackage* package);

#endif
