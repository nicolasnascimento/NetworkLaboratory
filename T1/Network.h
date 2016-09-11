#ifndef NETWORK_H
#define NETWORK_H

#define MAC_ADDRESS_LENGTH 6 // Ethernet
#define BUFFER_SIZE 1500
#define SET_SHARED_SOCKET_TRUE 1
#define SET_SHARED_SOCKET_FALSE 0


#include "Arp.h"

/// The standard shared socket for the application
/// The integer it the FileDescriptor that represents the socket
extern int sharedSocket;
extern unsigned char localInterfaceMacAddress[6];
extern unsigned char localInterfaceIpAddress[4];

/// Initializes the shared socket with the standard options
/// Returns -1 if something goes wrong during initialization
/// The "error" variable will be set with the correspoding error
int initSharedSocketWithInterfaceName(const char* interfaceName);

/// Initializes a new raw socket with the standard options and returns its File Descriptor
/// Returns -1 if something goes wrong during initialization
/// The "error" variable will be set the corresping error
int initSocketWithInterfaceName(const char* interfaceName, int isShared);


/// Closes the socket
int closeSocketWithFileDescriptor(int fileDescriptor);

/// Closes the shared socket
int closeSharedSocket();


/// Sends a generic buffer through the network using the shared socket
/// Returns -1 if the sharedSocket is not initialized or if any errors happen during the sending process
int sendArpPackage(ArpPackage* package);

/// Creates a new ArpPackage with the incoming data using the shared socket
/// Also sets the sourceIpAddress and sourceMacAddress with the incoming data 
/// Returns -1 if the sharedSocket is not initialiazed of if any errors happen during the receiving process
/// WARNING - This is a blocking call
int receiveArpPackage(ArpPackage* package, unsigned short operation);


#endif
