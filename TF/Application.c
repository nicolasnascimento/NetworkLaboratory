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
#include <stdarg.h>

/// A flag to indicate wheter debug printing should be performed
int verbose_mode = 0;

/// Always use this print, which is only enabled when verbose mode is enabled.
void d_printf(char* format, ...) {
	va_list arguments;
	if( !verbose_mode ) {
		return;	
	}
	va_start(arguments, format);
	vprintf(format, arguments);
	va_end(arguments);
}

/// Gets the initial flags from the Command Line
void get_initial_flags(int argc, char** argv) {
	int value = 0;
	// Loop to look for flags
	while( (value = getopt(argc, argv, "v")) != -1 ) {
		switch( value ) {
			case 'v':
				verbose_mode = 1;
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
	d_printf("Initializing\n");	
}

/// Dealloc objects
/// Perform deinitialization for the objects
void deinit(void) {	
	d_printf("Deinitializing\n");
}

/// This should perform the DHCP Spoofing
void init_DHCP_server() {
	
}
/// This should begin monitoring the networking and opening the incoming packages
void init_sniffer() {
	
}

int main(int argc, char** argv) {		
	// Gets Initial Flags
	get_initial_flags(argc, argv);

	// Initialization
	init();

	// Register exit function
	atexit(deinit);	
	
	init_DHCP_server();
	
	init_sniffer();

	// End of program
	exit(EXIT_SUCCESS);
}
