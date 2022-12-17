#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT "23499"
#define localhost "127.0.0.1"

void bootUpMsg()
{
    printf("The ServerP is up and running using UDP on port %s\n", PORT);
}

int GAP(int S1,int S2)
{
	int num = 0;
	if (S1>S2)
	{
		num=S1-S2;
	}
	else
	{
		num = S2-S1;
	}
	return num/(S1+S2);
}

int main()
{

    bootUpMsg();

    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int status;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints); // make sure hints is empty
    hints.ai_family = AF_INET;       // use IPv4
    hints.ai_socktype = SOCK_DGRAM;  // use datagram sockets

    // error checking for getaddrinfo

    if ((status = getaddrinfo(localhost, PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // make a socket:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // error checking for socket creation

    if (sockfd == -1)
    {
        perror("ServerP: socket");
        exit(1);
    }

    // bind it to the port and IP address we passed in to getaddrinfo():

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        close(sockfd);
        perror("ServerP: bind");
        exit(1);
    }

    freeaddrinfo(res); // all done with this structure
	int numbytes;
	char buffer[512];
    if ((numbytes = recv(child_fd, buffer, sizeof buffer, 0)) == -1)
                {
                    perror("serverP: recv");
                    exit(1);
                }
    buffer[numbytes] = '\0';

    printf("%d", GAP(buffer[0],buffer[1]));
}