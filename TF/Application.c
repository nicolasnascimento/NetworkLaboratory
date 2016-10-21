#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

void cleanUp(void) {

}

int main(int argc, const char* argv[]) {		
	
	// Register exit function
	atexit(cleanUp);	
	
	// End of program
	exit(EXIT_SUCCESS);
}
