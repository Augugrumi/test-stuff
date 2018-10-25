/*
 * $Id: simple-tcp-proxy.c,v 1.11 2006/08/03 20:30:48 wessels Exp $
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <syslog.h>
#include <err.h>

#include <linux/tcp.h>
#include <linux/ip.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <netinet/in.h>

#include <arpa/ftp.h>
#include <arpa/inet.h>

#include <tins/tins.h>
#include <iterator>

#define BUF_SIZE 4096

char client_hostname[64];
std::vector<unsigned char> header_vector;
int header_size;

void
cleanup(int sig)
{
    syslog(LOG_NOTICE, "Cleaning up...");
    exit(0);
}

void
sigreap(int sig)
{
    int status;
    pid_t p;
    signal(SIGCHLD, sigreap);
    while ((p = waitpid(-1, &status, WNOHANG)) > 0);
    /* no debugging in signal handler! */
}

void
set_nonblock(int fd)
{
    int fl;
    int x;
    fl = fcntl(fd, F_GETFL, 0);
    if (fl < 0) {
    printf("fcntl F_GETFL: FD %d: %s\n", fd, strerror(errno));
    exit(1);
    }
    x = fcntl(fd, F_SETFL, fl | O_NONBLOCK);
    if (x < 0) {
    printf("fcntl F_SETFL: FD %d: %s\n", fd, strerror(errno));
    exit(1);
    }
}


int
create_server_sock(char *addr, int port)
{
    int addrlen, s, on = 1, x;
    static struct sockaddr_in client_addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    err(1, "socket");

    addrlen = sizeof(client_addr);
    memset(&client_addr, '\0', addrlen);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(addr);
    client_addr.sin_port = htons(port);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, 4);
    x = bind(s, (struct sockaddr *) &client_addr, addrlen);
    if (x < 0)
    err(1, "bind %s:%d", addr, port);

    x = listen(s, 5);
    if (x < 0)
    err(1, "listen %s:%d", addr, port);
    printf("listening on %s port %d\n", addr, port);

    return s;
}

int
open_remote_host(char *host, int port)
{
    struct sockaddr_in rem_addr;
    int len, s, x;
    struct hostent *H;
    int on = 1;

    H = gethostbyname(host);
    if (!H)
    return (-2);

    len = sizeof(rem_addr);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    return s;

    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, 4);

    len = sizeof(rem_addr);
    memset(&rem_addr, '\0', len);
    rem_addr.sin_family = AF_INET;
    memcpy(&rem_addr.sin_addr, H->h_addr, H->h_length);
    rem_addr.sin_port = htons(port);
    x = connect(s, (struct sockaddr *) &rem_addr, len);
    if (x < 0) {
    close(s);
    return x;
    }
    set_nonblock(s);
    return s;
}

int
get_hinfo_from_sockaddr(struct sockaddr_in addr, int len, char *fqdn)
{
    struct hostent *hostinfo;

    hostinfo = gethostbyaddr((char *) &addr.sin_addr.s_addr, len, AF_INET);
    if (!hostinfo) {
    sprintf(fqdn, "%s", inet_ntoa(addr.sin_addr));
    return 0;
    }
    if (hostinfo && fqdn)
    sprintf(fqdn, "%s [%s]", hostinfo->h_name, inet_ntoa(addr.sin_addr));
    return 0;
}


int
wait_for_connection(int s)
{
    static int newsock;
    static socklen_t len;
    static struct sockaddr_in peer;

    len = sizeof(struct sockaddr);
    printf("calling accept FD %d\n", s);
    newsock = accept(s, (struct sockaddr *) &peer, &len);
    /* dump_sockaddr (peer, len); */
    if (newsock < 0) {
    if (errno != EINTR) {
        printf("accept FD %d: %s\n", s, strerror(errno));
        return -1;
    }
    }
    get_hinfo_from_sockaddr(peer, len, client_hostname);
    set_nonblock(newsock);
    return (newsock);
}

int
mywrite(int fd, char *buf, int *len)
{
    int x = send(fd, buf, *len,0);

    if (x < 0)
        return x;
    if (x == 0)
        return x;
    if (x != *len)
        memmove(buf, buf+x, (*len)-x);
    *len -= x;
    printf("write x: %d\n", x);
    printf("buf: %s\n", buf);
    return x;
}

std::vector<unsigned char> create_header(const char* source_ip, const char* dest_ip,
                   uint16_t source_port, uint16_t dest_port) {
    Tins::IP ip = Tins::IP(dest_ip, source_ip) /
                  Tins::TCP(dest_port, source_port) /
                  Tins::RawPDU("");/*"I'm a payload!");*/

    return ip.serialize();
}

void service_client(int cfd, int sfd/*, const std::vector<unsigned char>& header_vector*/) {
    int maxfd;
    char *sbuf;
    char *cbuf;
    int x, n;
    int cbo = 0;
    int sbo = 0;
    fd_set R;

    sbuf = (char*)malloc(BUF_SIZE);
    cbuf = (char*)malloc(BUF_SIZE);
    maxfd = cfd > sfd ? cfd : sfd;
    maxfd++;

    const unsigned char* header = &header_vector[0];

    while (1) {
    struct timeval to;
    if (cbo) {
        printf("if cbo\n");
        /*unsigned char header[] = {0x45, 0x00, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x80, 0x06,
                             0x3c, 0xcd, 0x7f, 0x00, 0x00, 0x01, 0x7f, 0x00, 0x00, 0x01,
                             0x21, 0xfd, 0x22, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x50, 0x00, 0x7f, 0xa6, 0xee, 0x3d, 0x00, 0x00};
                            /*{0x45, 0x00, 0x00, 0x28, 0x00, 0x01, 0x00, 0x00, 0x80, 0x06,
                             0x3C, 0xCD, 0x0A, 0x16, 0x06, 0x6F, 0x0A, 0x16, 0x06, 0x6F,
                             0x21, 0xFD, 0x22, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x50, 0x00, 0x7F, 0xA6, 0xEE, 0x3D, 0x00, 0x00};*/
        //int header_size = 40; // sizeof(iphdr) + sizeof(tcphdr)
        
        char formatted_pkt[cbo + header_size];
        memcpy(formatted_pkt, header, header_size);
        memcpy(formatted_pkt + header_size, cbuf, cbo);
        cbo += header_size;
        if (mywrite(sfd, formatted_pkt, &cbo) < 0 && errno != EWOULDBLOCK) {
            printf("write %d: %s\n", sfd, strerror(errno));
                exit(1);
        }
    }
    if (sbo) {
        printf("if sbo\n");
        if (mywrite(cfd, sbuf, &sbo) < 0 && errno != EWOULDBLOCK) {
            printf("write %d: %s\n", cfd, strerror(errno));
                exit(1);
        }
    }
    FD_ZERO(&R);
    if (cbo < BUF_SIZE)
        FD_SET(cfd, &R);
    if (sbo < BUF_SIZE)
        FD_SET(sfd, &R);
    to.tv_sec = 0;
    to.tv_usec = 1000;
    x = select(maxfd+1, &R, 0, 0, &to);
    if (x > 0) {
        if (FD_ISSET(cfd, &R)) {
        printf("if1\n");
        n = read(cfd, cbuf+cbo, BUF_SIZE-cbo);
        printf("read %d bytes from CLIENT (%d)\n", n, cfd);
        if (n > 0) {
            cbo += n;
        } else {
            close(cfd);
            close(sfd);
            printf("exiting\n");
            _exit(0);
        }
        }
        if (FD_ISSET(sfd, &R)) {
        printf("if2");
        n = read(sfd, sbuf+sbo, BUF_SIZE-sbo);
        printf("read %d bytes from SERVER (%d)\n", n, sfd);
        if (n > 0) {
            sbo += n;
        } else {
            close(sfd);
            close(cfd);
            syslog(LOG_INFO, "exiting\n");
            _exit(0);
        }
        }
    } else if (x < 0 && errno != EINTR) {
        printf("select: %s\n", strerror(errno));
        close(sfd);
        close(cfd);
        printf("exiting\n");
        _exit(0);
    }
    }
}

int
main(int argc, char *argv[])
{
    char *localaddr = NULL;
    int localport = -1;
    char *remoteaddr = NULL;
    int remoteport = -1;
    int client = -1;
    int server = -1;
    int master_sock = -1;

    if (5 != argc) {
    fprintf(stderr, "usage: %s laddr lport rhost rport\n", argv[0]);
    exit(1);
    }

    localaddr = strdup(argv[1]);
    localport = atoi(argv[2]);
    remoteaddr = strdup(argv[3]);
    remoteport = atoi(argv[4]);

    assert(localaddr);
    assert(localport > 0);
    assert(remoteaddr);
    assert(remoteport > 0);

    openlog(argv[0], LOG_PID, LOG_LOCAL4);

    signal(SIGINT, cleanup);
    signal(SIGCHLD, sigreap);

    header_vector = create_header(localaddr, remoteaddr, localport, remoteport);
    header_size = header_vector.size();

    master_sock = create_server_sock(localaddr, localport);
    for (;;) {
    if ((client = wait_for_connection(master_sock)) < 0)
        continue;
    if ((server = open_remote_host(remoteaddr, remoteport)) < 0) {
        close(client);
        client = -1;
        continue;
    }
    if (0 == fork()) {
        /* child */
        printf("connection from %s fd=%d", client_hostname, client);
        printf("connected to %s:%d fd=%d", remoteaddr, remoteport, server);
        close(master_sock);
        service_client(client, server);
        abort();
    }
    close(client);
    client = -1;
    close(server);
    server = -1;
    }

}
