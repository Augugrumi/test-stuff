#!/bin/bash

set -e
touch result.total.csv
for i in $(seq $1 $2)
do
    echo "--USING CHAIN $i ELEMENTS LONG--"
    python3.6 sfc-startup.py --launch=$3 --chain-length=$i --yaml=./sfc.$i.yaml --listen-port=16123
    mv result.csv result.csv.$i
    cat result.csv.$i | tail -n +2 >> result.total.csv
    echo "--STOP USING CHAIN $i ELEMENTS LONG--"
done
