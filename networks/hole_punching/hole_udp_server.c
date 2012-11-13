// refer to wep-site (http://www.rapapaing.com/blog/?p=24)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFLEN 512
#define NPACK 10

struct client { 
    int host;
    short port;
};

void die_func(char *s, int line)
{
    printf("line#%d ", line);
    fflush(stdout);
    perror(s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    struct sockaddr_in addr_me, addr_other;
    int s_socket, i, j, count;
    int addr_len;
    struct client clients[10];
    char buf[BUFLEN];
    addr_len = sizeof(struct sockaddr_in);

    if (argc != 2){
	printf("./hole_udp_server [port]\n");
	exit(1);
    }

    if ((s_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	die_func("socket", __LINE__);

    // si_me stores our local endpoint. Remember that this program
    // has to be run in a network with UDP endpoint previously known
    // and directly accessible by all clients. In simpler terms, the
    // server cannot be behind a NAT

    memset(&addr_me, 0x00, sizeof(struct sockaddr_in));
    addr_me.sin_family = PF_INET;
    addr_me.sin_port = htons(atoi(argv[1]));
    addr_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(s_socket , (struct sockaddr *)&addr_me, addr_len)) < 0)
	die_func("bind", __LINE__);


    count = 0;
    while(1) {
	if (recvfrom(s_socket, buf, BUFLEN, 0, (struct sockaddr*)&(addr_other), &addr_len) < 0)
	    die_func("recvfrom", __LINE__);

	// The client's public UDP endpoint data is now in si_other.
	// Notice that we're completely ignoring the datagram payload.
	// If we want to support multiple clients inside the same NAT,
	// we'd have clients send their own private UDP endpoints
	// encoded in some way inside the payload, and store those as
	// well.

	printf("Received packet from %s:%d\n", inet_ntoa(addr_other.sin_addr), ntohs(addr_other.sin_port));
	clients[count].host = addr_other.sin_addr.s_addr;
	clients[count].port = addr_other.sin_port;
	count ++;

	for (i = 0; i < count; i++) {
	    addr_other.sin_addr.s_addr  = clients[i].host;
	    addr_other.sin_port         = clients[i].port;
	    for (j = 0; j < count; j++) {
		printf("Sending to %s:%d\n", inet_ntoa(addr_other.sin_addr), ntohs(addr_other.sin_port));
		if(sendto(s_socket, &clients[j], 6, 0, (struct sockaddr*)&addr_other, addr_len) < 0)
		    die_func("sendto", __LINE__);
	    }                       
	}                           
	printf("Now we have %d clinet \n", count);
    }
    // Never reached here.
    close(s_socket);
    return 0;
}
  
