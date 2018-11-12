#! /usr/bin/env python3
# coding: utf-8

from optparse import OptionParser
import socket
import logging
import threading
from scapy.all import *
from binascii import hexlify
import time

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


def send_thread(sock_dst, options, pkt, clientsocket):
    while True:
        data = sock_dst.recv(65507)
        if options.reverse:
            data = raw(pkt) + data
        if len(data) > 45000:
            sock_dst.send(data[:45000])
            data = data[45000:]
            if not options.reverse:
                data = raw(pkt) + data
        clientsocket.send(data)


def handle_single_connection(clientsocket):
    sock_dst = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_dst.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    dst_addr = (options.dst_ip, options.dst_port)
    my_ip = get_my_ip()

    sock_dst.connect(dst_addr)

    pkt = IP(src=my_ip, dst=options.recv_ip) / \
        TCP(sport=options.port, dport=options.recv_port)

    Thread(target=send_thread, args=[
           sock_dst, options, pkt, clientsocket]).start()

    print("sending not thread")
    while True:
        data = clientsocket.recv(65507)
        if not data:
            logger.error('error while receiving data')
            sock_dst.shutdown(socket.SHUT_RDWR)
            sock_dst.close()
            break
        logger.debug('received data')
        while len(data) > 0:
            if not options.reverse:
                data = raw(pkt) + data
            sock_dst.send(data[:2048])
            data = data[2048:]
            time.sleep(0.05)
        #if not options.reverse:
        #    data = raw(pkt) + data
        #if (len(data) > 45000):
        #    sock_dst.send(data[:45000])
        #    data = data[45000:]
        #    if not options.reverse:
        #        data = raw(pkt) + data
        #time.sleep(1)
        #sock_dst.send(data)


def recv():
    sock_src = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_src.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
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
