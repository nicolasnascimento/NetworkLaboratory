/*
 * UDP echo server
 * Based on code from:
 * http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/
 */

#include <stdio.h>		/* for printf() and fprintf() */
#include <sys/socket.h>		/* for socket() and bind() */
#include <arpa/inet.h>		/* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>		/* for atoi() and exit() */
#include <string.h>		/* for memset() */
#include <unistd.h>		/* for close() */

#define ECHOMAX 255		/* Longest string to echo */

int main(int argc, char *argv[])
{
	int sock;		/* Socket */
	struct sockaddr_in echoServAddr;	/* Local address */
	struct sockaddr_in echoClntAddr;	/* Client address */
	unsigned int cliAddrLen;	/* Length of incoming message */
	char echoBuffer[ECHOMAX];	/* Buffer for echo string */
	unsigned short echoServPort;	/* Server port */
	int recvMsgSize;	/* Size of received message */

	if (argc != 2) {	/* Test for correct number of parameters */
		fprintf(stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[0]);
		exit(1);
	}

	echoServPort = atoi(argv[1]);	/* First arg:  local port */

	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(1);
	}

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof (echoServAddr));	/* Zero out structure */
	echoServAddr.sin_family = AF_INET;	/* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort);	/* Local port */

	/* Bind to the local address */
	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof (echoServAddr)) < 0) {
		perror("bind");
		exit(1);
	}

	for (;;) {		/* Run forever */
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof (echoClntAddr);

		/* Block until receive message from a client */
		if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
					    (struct sockaddr *) &echoClntAddr,
					    &cliAddrLen)) < 0) {
			perror("recvfrom");
			exit(1);
		}

		printf("Recebi do cliente %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("Mensagem: %s\n", echoBuffer);
	}
	/* NOT REACHED */

	close(sock);
	return 0;
}
