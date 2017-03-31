// an ultra-minimal NTP client
// inspired by http://seriot.ch/ntp.php

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

int main( int argc, char* argv[] ) {

	struct addrinfo hints;
	struct addrinfo *target;

	int udpsock;
	unsigned char buffer[48];
  uint32_t* intbuf = (uint32_t*)buffer;
	int result;

  if (argc != 2) {
    printf("usage: nanontp ntp_server_address\n");
    exit(0);
  }

  // prepare minimal NTP request
  memset(buffer,0,sizeof(buffer));
  buffer[0] = 'c';

	// create target address
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC; // allow IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	result = getaddrinfo(argv[1],"123",&hints,&target); // 123 = NTP port
	if (result != 0) {
		printf("Can't get host address - error %s.\n",gai_strerror(result));
		exit(1);
	}

	// create UDP socket
	udpsock = socket(target->ai_family,target->ai_socktype,target->ai_protocol);
	if (udpsock < 0) {
		printf("Can't create UDP socket - error %i.\n",errno);
		exit(1);
	}

	// send request
	result = sendto(udpsock,&buffer,sizeof(buffer),0,target->ai_addr,target->ai_addrlen);
	if (result < 0) {
		printf("Can't send on UDP port - error %i.\n",errno);
		exit(2);
	}

	// wait for reply 
	result = recvfrom(udpsock,&buffer,sizeof(buffer),0,NULL,NULL);
	if (result < 0) {
		printf("Can't receive on UDP port - error %i.\n",errno);
		exit(3);
	}

  // convert NTP timestamp (base 1900) to UNIX timestamp (base 1970)
  printf("%u\n",ntohl(intbuf[10])-(uint32_t)2208988800);
}

