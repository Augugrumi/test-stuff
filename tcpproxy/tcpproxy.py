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
    parser.add_option('--recv-ip',
                      help='Receiver ip, eg 192.168.4.54')
    parser.add_option('--recv-port',
                      help='Receiver port, eg. 623',
                      type=int)

    parser.add_option('--reverse',
                      action='store_true',
                      help='Reverse the header addition')

    return parser.parse_args()

(options, args) = parse_args()

def get_my_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    res = s.getsockname()[0]

    s.close()
    return res

def handle_single_connection(clientsocket):
    sock_dst = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    dst_addr = (options.dst_ip, options.dst_port)
    my_ip = get_my_ip()

    sock_dst.connect(dst_addr)
    while True:
        data = clientsocket.recv(65535)
        if not data:
            logger.error('error while receiving data')
            sock_dst.shutdown(socket.SHUT_RDWR)
            sock_dst.close()
            break
        logger.debug('received data')
        pkt = IP(src=my_ip, dst=options.recv_ip) / \
            TCP(sport=options.port, dport=options.recv_port)
        if not options.reverse:
            data = raw(pkt) + data

        sock_dst.send(data)
        data = sock_dst.recv(65535)
        if options.reverse:
            data = raw(pkt) + data

        clientsocket.send(data)

def recv():
    sock_src = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    recv_addr = (options.bind_address, options.port)

    # Waiting for incoming TCP connection
    sock_src.bind(recv_addr)
    sock_src.listen(5)
    while True:
        logger.debug('waiting for an incoming connection...')
        (clientsocket, address) = sock_src.accept()
        logger.debug('incoming connection accepted')
        handle_single_connection(clientsocket)
        clientsocket.close()
        logger.debug('current connection closed')

    sock_src.close()

if __name__ == '__main__':
    parse_args()
    try:
        recv()
    except KeyboardInterrupt:
        exit(0)
