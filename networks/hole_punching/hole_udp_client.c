#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFLEN	512
#define NPACK	10

struct client { 
    int host;
    short port;
};

// Just a function to kill the program when something goes wrong.
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
    int c_socket;
    int addr_size;
    int count;
    int i, j, k, f;

    count = 0;
    addr_size = sizeof(struct sockaddr_in);

    struct client buf;
    struct client server;
    struct client peers[10]; 

    if (argc != 4) { 
	printf("./hole_udp_client [udp-server] [udp-server-port] [my udp-port]\n");
	exit(EXIT_FAILURE);
    }
    else   
	printf("Server IP :%s Server Port : %d Own port : %d\n", argv[1], atoi(argv[2]), atoi(argv[3]));

    if ((c_socket = socket(PF_INET, SOCK_DGRAM, 0))< 0)
	    die_func("socket", __LINE__);

    memset(&addr_me, 0x00, sizeof(struct sockaddr_in));
    addr_me.sin_family = AF_INET;
    addr_me.sin_port = htons(atoi(argv[3]));
    addr_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    memset(&addr_other, 0x00, sizeof(struct sockaddr_in));
    addr_other.sin_family = AF_INET;
    addr_other.sin_port = htons(atoi(argv[2]));
    addr_other.sin_addr.s_addr = inet_addr(argv[1]);

    // Send a simple datagram to the server to let it know of our public UDP endpoint.
    // Not only the server, but other clients will send their data through this endpoint.
    // The datagram payload is irrelevant, but if we wanted to support multiple
    // clients behind the same NAT, we'd send our won private UDP endpoint information
    // as well.

    if ((sendto(c_socket, "hi", 2, 0, (struct sockaddr*)(&addr_other), addr_size)) < 0)
	die_func("sendto", __LINE__);

    // Right here, our NAT should have a session entry between our host and the server.
    // We can only hope our NAT maps the same public endpoint (both host and port) when we
    // send datagrams to other clients using our same private endpoint.
    while(1) { 
	// Receive data from the socket. Notice that we use the same socket for server and        
	// peer communications. We discriminate by using the remote host endpoint data, but
	// remember that IP addresses are easily spoofed (actually, that's what the NAT is
	// doing), so remember to do some kind of validation in here.
	if (recvfrom(c_socket, &buf, sizeof(buf), 0, (struct sockaddr *) (&addr_other), &addr_size) < 0)
	    die_func("recvfrom", __LINE__);
	printf("Received packet from %s: %d\n", inet_ntoa(addr_other.sin_addr), ntohs(addr_other.sin_port));

	if (server.host == addr_other.sin_addr.s_addr && server.port == (short) addr_other.sin_port) { 
	    // Receive data from the socket. Notice that we use the same socket for server and   
	    // peer communications. We discriminate by using the remote host endpoint data, but
	    // remember that IP addresses are easily spoofed (actually, that's what the NAT is
	   // doing), so remember to do some kind of validation in here.
	   for (i=0; i<count && f == 0; i++) { 
	       if (peers[i].host == buf.host && peers[i].port == buf.port) { 
		   f = 1;
	       }
	   }
	   // Only add it if we didn't have it before
	   if (f == 0) { 
	       peers[count].host = buf.host;
	       peers[count].port = buf.port;
	       count++;
	   }

	   addr_other.sin_addr.s_addr = buf.host;
	   addr_other.sin_port = buf.port;
	   printf("Added peer %s:%d\n", inet_ntoa(addr_other.sin_addr), ntohs(addr_other.sin_port));
	   printf("Now we have %d peers\n", count);

            // And here is where the actual hole punching happens. We are going to send
            // a bunch of datagrams to each peer. Since we're using the same socket we
            // previously used to send data to the server, our local endpoint is the same
            // as before.
            // If the NAT maps our local endpoint to the same public endpoint
            // regardless of the remote endpoint, after the first datagram we send, we
            // have an open session (the hole punch) between our local endpoint and the
            // peer's public endpoint. The first datagram will probably not go through
            // the peer's NAT, but since UDP is stateless, there is no way for our NAT
            // to know that the datagram we sent got dropped by the peer's NAT (well,
            // our NAT may get an ICMP Destination Unreachable, but most NATs are
            // configured to simply discard them) but when the peer sends us a datagram,
            // it will pass through the hole punch into our local endpoint.  
	    for (k = 0; k < 10; k++) { 
		//send 10 datagrams.
		for (i = 0; i < count; i++) { 
		    addr_other.sin_addr.s_addr = peers[i].host;
		    addr_other.sin_port = peers[i].port;
		    //once again, the payload is irrelavant. Feel free to send you VoIP
		    //data in here.
		    if (sendto(c_socket, "hi", 2, 0, (struct sockaddr*)(&addr_other), addr_size) < 0)
			    die_func("sendto()", __LINE__);
		}
	    }
	}
	else  { 
	    for (i = 0; i < count; i++) { 
		// identify which peer it came from
		if (peers[i].host == buf.host && peers[i].port == (short) (buf.port)) { 
		    // and do something usefule with the received payload
		    printf("Received frome peer %d\n", i);
		    break;
		}
	    }
	   
            // And here is where the actual hole punching happens. We are going to send
            // a bunch of datagrams to each peer. Since we're using the same socket we
            // previously used to send data to the server, our local endpoint is the same
            // as before.
            // If the NAT maps our local endpoint to the same public endpoint
            // regardless of the remote endpoint, after the first datagram we send, we
            // have an open session (the hole punch) between our local endpoint and the
            // peer's public endpoint. The first datagram will probably not go through
            // the peer's NAT, but since UDP is stateless, there is no way for our NAT
            // to know that the datagram we sent got dropped by the peer's NAT (well,
            // our NAT may get an ICMP Destination Unreachable, but most NATs are
            // configured to simply discard them) but when the peer sends us a datagram,
            // it will pass through the hole punch into our local endpoint. 
	}
    }

    // Actuallly, we never reach this point.
    close(c_socket);
    return 0;
}
