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
			// Create raw socket
			int rawSockfd = createRawSocketOrExitFailure();
			
			//Send ICMP Echo Request
			const IcmpEchoHeader icmpEchoHeader = constructIcmpEchoHeader(getpid(), sequenceNumber);

			// Destination address
			struct sockaddr_in dest_addr;
			memset(&dest_addr, 0, sizeof(dest_addr));
			dest_addr.sin_family = AF_INET;
			dest_addr.sin_addr.s_addr = inet_addr(av[1]);

			// Send ICMP packet
			if (sendto(rawSockfd, &icmpEchoHeader, sizeof(icmpEchoHeader), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
				perror("sendto");
				close(rawSockfd);
				exit(EXIT_FAILURE);
			}
			//Recive ICMP Echo Reply and Print (Maybe Somewhere Else Like in A Handler)
			++sequenceNumber;
		}
	}
}
