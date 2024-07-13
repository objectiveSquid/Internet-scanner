#include "InternetScanner/ping.hpp"
#include "InternetScanner/ipv4.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>

#define ICMP_HEADERLENGTH 8

#define ECHO_SUCCESS 1
#define ECHO_FAILURE 0

ushort calculateChecksum(void *buffer, uint length) {
    ushort *uShortBuffer = (ushort *)(buffer);
    uint sum = 0;
    ushort result;

    for (sum = 0; length > 1; length -= 2)
        sum += *uShortBuffer++;
    if (length == 1)
        sum += *(u_char *)uShortBuffer;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

uint8_t ping(const IPv4Address ipAddress, const uint32_t timeoutMilliseconds) {
    int socketFd;
    struct sockaddr_in address = {0};
    struct icmp icmpHeader = {0};
    char packet[ICMP_HEADERLENGTH + 64];
    socklen_t addressLength = sizeof(address);

    if ((socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket");
        return ECHO_FAILURE;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(ipAddress);

    icmpHeader.icmp_type = ICMP_ECHO;
    icmpHeader.icmp_code = 0;
    icmpHeader.icmp_id = getpid();
    icmpHeader.icmp_seq = 1;
    icmpHeader.icmp_cksum = 0;

    memcpy(packet, &icmpHeader, ICMP_HEADERLENGTH);
    memset(packet + ICMP_HEADERLENGTH, 0, sizeof(packet) - ICMP_HEADERLENGTH);

    icmpHeader.icmp_cksum = calculateChecksum(packet, sizeof(packet));
    memcpy(packet, &icmpHeader, ICMP_HEADERLENGTH);

    if (sendto(socketFd, packet, sizeof(packet), 0, (struct sockaddr *)&address, sizeof(address)) <= 0) {
        close(socketFd);
        return ECHO_FAILURE;
    }

    struct timeval timeoutValue;
    timeoutValue.tv_sec = timeoutMilliseconds / 1000;
    timeoutValue.tv_usec = timeoutMilliseconds;

    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(socketFd, &readFds);

    int selectReturnValue = select(socketFd + 1, &readFds, NULL, NULL, &timeoutValue);
    switch (selectReturnValue) {
    case -1:
    case 0:
        close(socketFd);
        return ECHO_FAILURE;

    default:
        char pingResponseBuffer[1024];
        if (recvfrom(socketFd, pingResponseBuffer, sizeof(pingResponseBuffer), 0, (struct sockaddr *)&address, &addressLength) <= 0) {
            close(socketFd);
            return ECHO_FAILURE;
        }

        close(socketFd);
        return ECHO_SUCCESS;
    }
}
