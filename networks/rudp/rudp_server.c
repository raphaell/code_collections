#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include "common.h"

void exit_handler(char *msg);

int main(int argc, char **argv)
{

    int sock;
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;
    MSG msg;
    int addr_size;

    if (argc != 2) { 
	printf("Usage: executable-file [Port]\n");
	exit(EXIT_FAILURE);
    }
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) { 
	exit_handler("Making socket");
    }

    memset(&s_addr, 0x00, sizeof(struct sockaddr_in));

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&s_addr, sizeof(struct sockaddr)) < 0)  {
	exit_handler("Making bind");
    }

    addr_size = sizeof(struct sockaddr_in);

    while(1) { 
	recvfrom(sock, &msg, sizeof(MSG), 0 /*Flags*/,  (struct sockaddr *)&c_addr, (socklen_t *)&addr_size);

	// first checking crc32 it is valid or not (UDP don't support confidence 

	// fragement checking 
    
    } // Nerver reached

    return 0;
}

void exit_handler(char *msg)
{
    if (msg)  
	printf("Process : %s\n", msg);
    printf("Reason: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

