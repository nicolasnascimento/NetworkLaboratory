#ifndef ARP_SNIFFER_H
#define ARP_SNIFFER_H 

/// Initializes a background thread that will monitor the interface. 
/// The thread willl print all fields of incoming ARP Messages
int initArpSnifferWithInterfaceName(const char* interfaceName);

#endif
