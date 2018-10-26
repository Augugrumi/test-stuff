#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <cstring>
#include <stdint.h>
#include <cstdint>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <tins/tins.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
//#include "client.h"

#define PORT    8701
#define MAXLINE 1024

using namespace Tins;

struct sfc_header {
    /**************************BASE HEADER*************************/
    uint16_t ver:2,         // protocol version
             oam:1,         // OAM bit
             u1:1,          // unused
             ttl:6,         // number of sf traversals
             len:6;         // total length of SBH, SH, CH in 4-byte words
    uint8_t  u2:4,
             md_type:4;

    uint8_t  prot;          //protocol used to encaps data

    /*************************SFC HEADER*************************/
    uint32_t p_id:24,       //Service Path Identifier (SPI): 24 bits
             si:8;          //Service Index (SI): 8 bits

    /***********************CONTEXT HEADER***********************/
    uint32_t source_address,
             destination_address;
    uint16_t source_port,
             destination_port;

    uint8_t  direction_flag:1;

    uint32_t other:24;
}__attribute__((__packed__)); // disallow optimization of byte positioning


void send_only(unsigned char *message, size_t message_len, char *dst, uint16_t port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd;
    int s;
    ssize_t res = -1;
    bool send_flag = true;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_UDP; /* UDP protocol */

    s = getaddrinfo(dst, std::to_string(port).c_str(), &hints, &result);

    if (s != 0) {
        perror("Error getting info for destination");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != nullptr && send_flag; rp = rp->ai_next) {

        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd > 0 && connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            res = sendto(
                    sfd,
                    message,
                    message_len,
                    MSG_CONFIRM,
                    rp->ai_addr,
                    rp->ai_addrlen);
            if (res > 0) {
                send_flag = false;
            }
        }
    }

    printf("Hello message sent.\n"); 
          
    /*char buffer[1024] = {0}; 
    res = recvfrom(sfd,
                   (char *)buffer, 1024,
                   MSG_WAITALL,
                   rp->ai_addr, &(rp->ai_addrlen));
    printf("Server : %s\n", buffer);*/
    close(sfd);
}


// Driver code 
int main() { 
    int sockfd; 
    char buffer[MAXLINE];

    struct sfc_header header;
    header.ver = 0;
    header.oam = 0;
    header.u1 = 0;
    header.ttl = 7;
    header.u2 = 0;
    header.md_type = 0x1; // oxF otherwise
    header.prot = 0x1;
    header.p_id = 0;
    header.si = 0;
    header.source_address = 169215599;
    header.source_port = 8701;
    header.destination_address = 169215599;
    header.destination_port = 8705;
    header.direction_flag = 0;

    IP ip = IP("82.54.184.76", "192.168.29.26") /
            TCP(50123, 8701) /
            RawPDU("");/*"I'm a payload!");*/

    std::vector<unsigned char> pkt_vector = ip.serialize();
    unsigned char* pkt_w_header = &pkt_vector[0];
    //char *hello = "Hello from client";

    /*unsigned char *pkt_w_header = new unsigned char[sizeof(sfc_header) + sizeof(hello)];
    memcpy((char*)pkt_w_header, &header, sizeof(sfc_header));
    memcpy((char*)pkt_w_header + sizeof(sfc_header), hello, sizeof(hello));*/

    //send_only(pkt_w_header, sizeof(sfc_header) + sizeof(hello), "localhost", 8767);

    std::ofstream output_file("./prova.txt");
    std::ostream_iterator<unsigned char> output_iterator(output_file);
    std::copy(pkt_vector.begin(), pkt_vector.end(), output_iterator);
    return 0;
}
