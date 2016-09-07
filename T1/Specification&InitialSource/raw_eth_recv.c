#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>

#define BUFFER_SIZE 1500
#define ETHERTYPE 0x0FFF
#define ARP_ETHERTYPE ETH_P_ARP

int main(int argc, char *argv[])
{
	int fd;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char *data;
	struct ifreq ifr;
	char ifname[IFNAMSIZ];

	if (argc != 2) {
		printf("Usage: %s iface\n", argv[0]);
		return 1;
	}
	strcpy(ifname, argv[1]);

	/* Cria um descritor de socket do tipo RAW */
	fd = socket(PF_PACKET,SOCK_RAW, htons(ETH_P_ALL));
	if(fd < 0) {
		exit(1);
	}

	/* Obtem o indice da interface de rede */
	strcpy(ifr.ifr_name, ifname);
	if(ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
		perror("ioctl");
		exit(1);
	}

	/* Obtem as flags da interface */
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0){
		perror("ioctl");
		exit(1);
	}

	/* Coloca a interface em modo promiscuo */
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		exit(1);
	}

	printf("Esperando pacotes ... \n");
	while (1) {
		unsigned char mac_dst[6];
		unsigned char mac_src[6];
		short int ethertype;

		/* Recebe pacotes */
		if (recv(fd,(char *) &buffer, BUFFER_SIZE, 0) < 0) {
			perror("recv");
			close(fd);
			exit(1);
		}
        
		/* Copia o conteudo do cabecalho Ethernet */
		memcpy(mac_dst, buffer, sizeof(mac_dst));
		memcpy(mac_src, buffer+sizeof(mac_dst), sizeof(mac_src));
		memcpy(&ethertype, buffer+sizeof(mac_dst)+sizeof(mac_src), sizeof(ethertype));
		ethertype = ntohs(ethertype);
		data = (buffer+sizeof(mac_dst)+sizeof(mac_src)+sizeof(ethertype));


		if (ethertype == ARP_ETHERTYPE) {
			printf("MAC destino: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_dst[0], mac_dst[1], mac_dst[2], mac_dst[3], mac_dst[4], mac_dst[5]);
			printf("MAC origem:  %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_src[0], mac_src[1], mac_src[2], mac_src[3], mac_src[4], mac_src[5]);
			printf("EtherType: 0x%04x\n", ethertype);
		
			struct arphdr arpHeader;
			memcpy(&arpHeader, data, sizeof(arpHeader));
			printf("HW type: %u\n", arpHeader.ar_hrd);
			printf("Protocol Type: %u\n", arpHeader.ar_pro);
			printf("HLEN: %u\n", arpHeader.ar_hln);
			printf("DLEN: %u\n", arpHeader.ar_pln);
			printf("ARP Opcode: %u\n", arpHeader.ar_op);


            /*
                #define	ARPOP_REQUEST	1 ARP request. 
                #define	ARPOP_REPLY	2 ARP reply. 
                #define	ARPOP_RREQUEST	3 RARP request.
                #define	ARPOP_RREPLY	4 RARP reply.  
                #define	ARPOP_InREQUEST	8 InARP request.
                #define	ARPOP_InREPLY	9 InARP reply.
                #define	ARPOP_NAK	10  (ATM)ARP NAK.
            */
            switch(arpHeader.ar_op) {
                case ARPOP_REQUEST:
                    printf("Arp request\n");
                    break;  
                case ARPOP_REPLY:
                    printf("Arp reply\n");
                    break;
                default:
                    printf("Untreated ARP Opcode: %x", arpHeader.ar_op);        
            }
		
			//printf("Dado: %s\n", data);
			printf("\n");
		}
	}

	close(fd);
	return 0;
}
