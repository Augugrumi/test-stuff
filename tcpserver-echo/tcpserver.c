#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

#define BUFFER_SIZE 1024
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int main (int argc, char *argv[]) {
  if (argc < 2) on_error("Usage: %s [port]\n", argv[0]);

  int port = atoi(argv[1]);

  int server_fd, client_fd, err;
  struct sockaddr_in server, client;
  char buf[BUFFER_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) on_error("Could not create socket\n");

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if (err < 0) on_error("Could not bind socket\n");

  err = listen(server_fd, 128);
  if (err < 0) on_error("Could not listen on socket\n");

  printf("Server is listening on %d\n", port);

  while (1) {
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

    if (client_fd < 0) on_error("Could not establish new connection\n");

    bool first = 1;

    while (1) {
      int read = recv(client_fd, buf, BUFFER_SIZE, 0);


      printf("packet: ");
      std::vector<unsigned char> pkt_vector;
      for(int i = 0; i < read; i++) {
        pkt_vector.push_back(buf[i]);
        //printf("%x", buf[i]);
        //fprintf(fp, "ciao %x", buf[i]);
      }
      printf("%d", pkt_vector.size());
      if(first) {
      std::ofstream output_file("./zdfs.txt");
      std::ostream_iterator<unsigned char> output_iterator(output_file);
      std::copy(pkt_vector.begin(), pkt_vector.end(), output_iterator);
      first = 0;
      }
      printf("\n");
      if (!read) break; // done reading
      if (read < 0) on_error("Client read failed\n");

      unsigned char h[] = {0x45, 0x00, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x80,
                          0x06, 0x3c, 0xcd, 0x7f, 0x00, 0x00, 0x01, 0x7f, 0x00,
                          0x00, 0x01, 0x21, 0xfd, 0x22, 0x01, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x7f, 0xa6,
                          0xee, 0x3d, 0x00, 0x00};
      char formatted_pkt[read + sizeof(iphdr) + sizeof(tcphdr)];
      memcpy(formatted_pkt, h, sizeof(iphdr) + sizeof(tcphdr));
      memcpy(formatted_pkt + sizeof(iphdr) + sizeof(tcphdr), buf, read);
      printf("to send: ");
      for (int i = 0; i < read + sizeof(iphdr) + sizeof(tcphdr); i++) {
        printf("%x", formatted_pkt[i]);
      }
      printf("\n");
      err = send(client_fd, formatted_pkt, read + sizeof(iphdr) + sizeof(tcphdr), 0);
      //err = send(client_fd, buf, read, 0);
      if (err < 0) on_error("Client write failed\n");
    }
  }

  return 0;
}

