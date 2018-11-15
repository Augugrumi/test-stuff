#! /usr/bin/env python
# coding: utf-8

from optparse import OptionParser
import socket
from scapy.all import *
import csv
import time


def parse_args():
    parser = OptionParser()

    parser.add_option('--repeat',
                      help='Number of times to repeat the test')
    parser.add_option('--dst-ip',
                      help='Destination ip, eg. 192.168.3.101.')
    parser.add_option('--dst-port',
                      help='Destination port, eg. 623.',
                      type=int)

    return parser.parse_args()


if __name__ == '__main__':
    (options, args) = parse_args()
    res = [['Before send', 'After send', 'Received']]
    for i in range(int(options.repeat)):
        print(i)

        sock_dst = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        dst_addr = (options.dst_ip, options.dst_port)

        data = "xxxxxxxx".encode("utf-8")
        sock_dst.sendto(data, dst_addr)
        bsent = int(time.time())
        sock_dst.recvfrom(65535)
        asent = int(time.time())
        received = int(time.time())

        res.append([bsent, asent, received])
        sock_dst.close()
        time.sleep(1)

    with open('result.csv', mode='w') as result_file:
        result_writer = csv.writer(
            result_file, delimiter=',', quotechar='"',
            quoting=csv.QUOTE_MINIMAL)

        result_writer.writerows(res)
