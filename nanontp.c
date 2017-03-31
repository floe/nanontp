// an ultra-minimal NTP client
// inspired by http://seriot.ch/ntp.php

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

int main( int argc, char* argv[] ) {

	struct sockaddr_in peeraddr;
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

	// create UDP socket
	udpsock = socket(AF_INET,SOCK_DGRAM,0);
	if (udpsock < 0) {
		printf("Can't create UDP socket - error %i.\n",errno);
		exit(1);
	}

	// create target address
	memset((char*)&peeraddr,0,sizeof(peeraddr));
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_addr.s_addr = inet_addr(argv[1]);
	peeraddr.sin_port = htons(123); // NTP port

	// send request
	result = sendto(udpsock,&buffer,sizeof(buffer),0,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
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

