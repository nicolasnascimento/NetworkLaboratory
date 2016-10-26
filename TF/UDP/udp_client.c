/*
 * UDP echo client
 * Based on code from:
 * http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/
 */
  
#include <stdio.h>		/* for printf() and fprintf() */
#include <sys/socket.h>		/* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>		/* for sockaddr_in and inet_addr() */
#include <stdlib.h>		/* for atoi() and exit() */
#include <string.h>		/* for memset() */
#include <unistd.h>		/* for close() */

#define ECHOMAX 255		/* Longest string to echo */

int main(int argc, char *argv[])
{
	int sock;		/* Socket descriptor */
	struct sockaddr_in echoServAddr;	/* Echo server address */
	struct sockaddr_in fromAddr;	/* Source address of echo */
	unsigned short echoServPort;	/* Echo server port */
	unsigned int fromSize;	/* In-out of address size for recvfrom() */
	char *servIP;		/* IP address of server */
	char *echoString;	/* String to send to echo server */
	char echoBuffer[ECHOMAX + 1];	/* Buffer for receiving echoed string */
	int echoStringLen;	/* Length of string to echo */
	int respStringLen;	/* Length of received response */

	if ((argc < 4)) {	/* Test for correct number of arguments */
		fprintf(stderr,
			"Usage: %s <Server IP> <Echo Port> <Echo Word>\n",
			argv[0]);
		exit(1);
	}

	servIP = argv[1];	/* First arg: server IP address (dotted quad) */
	echoServPort = atoi(argv[2]);	/* Second arg: server port */
	echoString = argv[3];	/* Third arg: string to echo */

	if ((echoStringLen = strlen(echoString)) > ECHOMAX) {	/* Check input length */
		printf("Echo word too long");
		exit(1);
	}

	/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(1);
	}

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof (echoServAddr));	/* Zero out structure */
	echoServAddr.sin_family = AF_INET;	/* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);	/* Server IP address */
	echoServAddr.sin_port = htons(echoServPort);	/* Server port */

	/* Send the string to the server */
	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
		   &echoServAddr, sizeof (echoServAddr)) != echoStringLen) {
		perror("sendto");
		exit(1);
	}

	close(sock);
	return 0;
}
