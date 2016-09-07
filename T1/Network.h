#ifndef NETWORK_H
#define NETWORK_H


#define BUFFER_SIZE 1500

/// The standard shared socket for the application
/// The integer it the FileDescriptor that represents the socket
extern int sharedSocket;

/// Initializes the shared socket with the standard options
/// Returns -1 if something goes wrong during initialization
/// The "error" variable will be set with the correspoding error
int initSharedSocketWithInterfaceName(const char* interfaceName);

/// Initializes a new raw socket with the standard options and returns its File Descriptor
/// Returns -1 if something goes wrong during initialization
/// The "error" variable will be set the corresping error
int initSocketWithInterfaceName(const char* interfaceName);

/// Sends a generic buffer through the network using the shared socket
/// Returns -1 if the sharedSocket is not initialized or if any errors happen during the sending process
int sendData(const char* buffer, const char* destinationIpAddress);

/// Sets "buffer" with the incoming data using the shared socket
/// Returns -1 if the sharedSocket is not initialiazed of if any errors happen during the receiving process
/// WARNING - This is a blocking call
int receiveData(char* buffer, const char* sourceIpAddress);


#endif
