#!/bin/python3

import sys
import csv

def get_lines(filename):
    with open(filename) as f:
        content = f.readlines()
    return [x.strip() for x in content]


def zip_res(filenames):
    docker = filenames + ".docker"
    vm = filenames + ".vm"
    docker_lines = get_lines(docker)
    vm_lines = get_lines(vm)

    zipped_lines = zip(vm_lines, docker_lines)

    with open('result.csv', mode='w') as result_file:
        result_writer = csv.writer(result_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        result_writer.writerow(['VirtualBox', 'Docker'])
        result_writer.writerows(zipped_lines)


if __name__ == '__main__':
    zip_res(sys.argv[1])
