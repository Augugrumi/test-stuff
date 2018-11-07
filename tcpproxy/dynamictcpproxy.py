#! /usr/bin/env python3
# coding: utf-8

from optparse import OptionParser
import socket
import logging
import threading
import queue
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

def sender_queue_elaborator():
    pass

def receiver_queue_elaborator():
    pass


def recv():
    pass

sender_worker = Thread(target=sender_queue_elaborator, name='sender_worker')
receiver_worker = Thread(target=receiver_queue_elaborator, name='receiver_worker')

if __name__ == '__main__':
    parse_args()
    try:
        recv()
    except KeyboardInterrupt:
        exit(0)
