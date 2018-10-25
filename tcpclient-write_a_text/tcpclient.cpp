#include "tcpclient.h"

void ClientTCP::connect_to_server(const char* dst, uint16_t port) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, dst, &serv_addr.sin_addr)<=0) {
        perror("invalid address / address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }
}

void ClientTCP::send_and_receive(unsigned char* message,
                                 size_t message_len,
                                 unsigned char* received,
                                 ssize_t received_len) {
    for (int i = 0; i < message_len; i++)
        printf("%x", *(message + i));
    printf("\n");

    received = new unsigned char[1024];

    send(sock , message , message_len, 0);
    printf("Hello message sent\n");
    received_len = read(sock , received, 1024);
    if (received_len < -1) {
        perror("error receiving data");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < received_len; i++)
        printf("%x", *(received + i));
    printf("\n");
}

int ClientTCP::access_to_socket() const {
    return sock;
}

void ClientTCP::close_connection() {
    close(sock);
}