#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/// A flag to indicate wheter debug printing should be performed
int verboseMode = 0;

void getInitialFlags(int argc, char** argv) {
	int value = 0;
	// Loop to look for flags
	while( (value = getopt(argc, argv, "v:")) != -1 ) {
		switch( value ) {
			case 'v':
				verboseMode = 1;
				break;
			case '?':
				break;
			default:
				break;
		}	
	}
}

/// Alloc & Initialze objects. 
/// Perform Initial Setup of the program
void init(void) {
	
}

/// Dealloc objects
/// Perform deinitialization for the objects
void deinit(void) {

}

int main(int argc, char** argv) {		
	// Gets Initial Flags
	getInitialFlags(argc, argv);

	// Register exit function
	atexit(deinit);	
	
	// End of program
	exit(EXIT_SUCCESS);
}
