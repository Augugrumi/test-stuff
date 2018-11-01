#! /usr/bin/env python3
# coding: utf-8

from optparse import OptionParser
import socket
import logging
from scapy.all import *
from binascii import hexlify

logger = logging.getLogger()
handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
handler.setFormatter(formatter)
logger.setLevel(logging.DEBUG)
logger.addHandler(handler)


def parse_args():
    parser = OptionParser()

    parser.add_option('--bind-address',
                      help='The address to bind, 0.0.0.0 for all ip address.')
    parser.add_option('--port',
                      help='The port to listen, eg. 623.',
                      type=int)
    parser.add_option('--dst-ip',
                      help='Destination host ip, eg. 192.168.3.101.')
    parser.add_option('--dst-port',
                      help='Destination host port, eg. 623.',
                      type=int)

    return parser.parse_args()

(options, args) = parse_args()


def recv():
    sock_src = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_dst = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    recv_addr = (options.bind_address, options.port)
    dst_addr = (options.dst_ip, options.dst_port)

    sock_dst.connect(dst_addr)

    # Waiting for incoming TCP connection
    sock_src.bind(recv_addr)
    sock_src.listen(5)
    while True:
        (clientsocket, address) = sock_src.accept()
        while True:
            data = clientsocket.recv(65535)
            if not data:
                logger.error('error while receiving data')
                break
            logger.debug('received data')
            pkt = IP(src=options.bind_address, dst=options.dst_ip) / \
                TCP(sport=options.port, dport=options.dst_port)
            data = raw(pkt) + data
            sock_dst.send(data)
            data = sock_dst.recv(65535)
            clientsocket.send(data)

        clientsocket.close()

    sock_src.close()
    sock_dst.close()

if __name__ == '__main__':
    parse_args()
    try:
        recv()
    except KeyboardInterrupt:
        exit(0)
