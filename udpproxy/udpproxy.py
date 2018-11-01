#! /usr/bin/env python
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

    parser.add_option('--proxy-ip',
                      help='The address to bind, 0.0.0.0 for all ip address.')
    parser.add_option('--proxy-port',
                      help='The port to listen, eg. 623.',
                      type=int)
    parser.add_option('--src-ip',
                      help='Source ip, eg. 192.168.3.101.')
    parser.add_option('--src-port',
                      help='Source port, eg. 623.',
                      type=int)
    parser.add_option('--dst-ip',
                      help='Destination ip, eg. 192.168.3.101.')
    parser.add_option('--dst-port',
                      help='Destination port, eg. 623.',
                      type=int)
    parser.add_option('--ingress-ip',
                      help='Ingress ip, eg. 192.168.3.101.')
    parser.add_option('--ingress-port',
                      help='Ingress port, eg. 623.',
                      type=int)

    return parser.parse_args()

(options, args) = parse_args()


def recv():
    sock_src = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_dst = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    recv_addr = (options.proxy_ip, options.proxy_port)
    dst_addr = (options.ingress_ip, options.ingress_port)
    sock_src.bind(recv_addr)

    while True:
        data, addr = sock_src.recvfrom(65565)
        if not data:
            logger.error('an error occured')
            break
        logger.debug('received: {0!r} from {1}'.format(data, addr))
        pkt = IP(src=options.src_ip, dst=options.dst_ip) / \
            UDP(sport=options.src_port, dport=options.dst_port)
        pkt.show()
        data = raw(pkt) + data
        sock_dst.sendto(data, dst_addr)
        data, _ = sock_dst.recvfrom(65565)
        sock_src.sendto(data, addr)

    sock_src.close()
    sock_dst.close()


if __name__ == '__main__':
    parse_args()
    try:
        #pkt = IP(src=options.src_ip, dst=options.dst_ip) / \
        #    UDP(sport=options.src_port, dport=options.dst_port)
        #pkt.show()
        recv()
    except KeyboardInterrupt:
        exit(0)
