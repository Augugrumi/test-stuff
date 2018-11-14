#! /usr/bin/env python3
# coding: utf-8
# Author: poloniodavide@gmail.com
# License: GPLv3

from optparse import OptionParser
import logging
import threading
import csv
import time
import subprocess
import socket

logger = logging.getLogger()
handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
handler.setFormatter(formatter)
logger.setLevel(logging.DEBUG)
logger.addHandler(handler)

K8S_BIN='kubectl'
CREATE_COMMAND='create'
DELETE_COMMAND='delete'
FILE_FLAG='-f'



def parse_args():
    parser = OptionParser()

    parser.add_option('--launch',
                      help='Number of times the test has to be performed',
                      type=int)
    parser.add_option('--chain-length',
                      help='Chain length',
                      type=int)
    parser.add_option('--yaml',
                      help='Path to the yaml to launch')
    parser.add_option('--listen-port',
                      help='UDP port to listen')
    return parser.parse_args()


(options, args) = parse_args()


def launch_test():
    time_launch = time.time()
    time_stop = -1
    subprocess.check_call([K8S_BIN, CREATE_COMMAND, FILE_FLAG, options.yaml])
    ack_counter = 0
    flag = True
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', int(options.listen_port)))
    while flag:
        data, addr = sock.recvfrom(1024)
        logger.info('Received ack from ' + str(addr))
        ack_counter += 1
        if ack_counter == options.launch:
            flag = False
            time_stop = time.time()
    subprocess.check_call([K8S_BIN, DELETE_COMMAND, FILE_FLAG, options.yaml])
    sock.close()
    if time_stop > 0:
        return time_stop - time_launch
    else:
        logger.warning('A test failed')
        return -1

def main():
    logger.info('--SFC launch timer, v0.1--')
    logger.info('Number of lauches set to: ' + str(options.launch))
    logger.info('Elements in the chain: ' + str(options.chain_length))
    logger.info('YAML to launch: ' + options.yaml)

    results = list()
    for i in range(options.launch):
        logger.info('Executing test: ' + str(i))
        results.append(launch_test())
        logger.info('Test ' + str(i) + ' terminated')

    with open('result.csv', mode='w') as result_file:
        result_writer = csv.writer(result_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        result_writer.writerow(['Time', 'Elements'])
    result_writer.writerows(map(lambda r: (r, options.launch), results))


if __name__ == '__main__':
    parse_args()
    main()
