#!/bin/bash

set -e
g++ -std=c++11 printer.c -fpermissive -ltins  -o printer && ./printer
hex=$(hexdump -ve '1/1 "%.2x"' ./prova.txt)
echo $(./formatter.py $hex)
rm printer prova.txt
