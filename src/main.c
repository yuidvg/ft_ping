#include "../include/all.h"

int main(int ac, char *av[])
{
	if (ac == 1)
	{
		printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
	}
	else if (ac == 2)
	{
		int sequenceNumber = 0;
		while (1)
		{
			//Send ICMP Echo Request
			const IcmpEchoRequest request = constructIcmpEchoRequest(getpid(), sequenceNumber);
			// Create raw socket
			int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
			if (sockfd < 0) {
				perror("socket");
				exit(EXIT_FAILURE);
			}

			// Destination address
			struct sockaddr_in dest_addr;
			memset(&dest_addr, 0, sizeof(dest_addr));
			dest_addr.sin_family = AF_INET;
			dest_addr.sin_addr.s_addr = inet_addr(av[1]);

			// ICMP header
			struct icmphdr icmp_hdr;
			memset(&icmp_hdr, 0, sizeof(icmp_hdr));
			icmp_hdr.type = ICMP_ECHO;
			icmp_hdr.un.echo.id = getpid();
			icmp_hdr.un.echo.sequence = sequenceNumber;
			icmp_hdr.checksum = calculate_checksum(&icmp_hdr, sizeof(icmp_hdr));

			// Send ICMP packet
			if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
				perror("sendto");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			//Recive ICMP Echo Reply and Print (Maybe Somewhere Else Like in A Handler)
			++sequenceNumber;
		}
	}
}
