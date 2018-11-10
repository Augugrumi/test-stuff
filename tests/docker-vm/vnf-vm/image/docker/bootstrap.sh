#!/bin/bash

set -e

if [ -n "$KUBERNETES_PORT" ] && [ -n "$DNS_IP" ]
then
    tmpfile=$(mktemp)
    (echo "nameserver $DNS_IP" && tac /etc/resolv.conf) > $tmpfile
    cat $tmpfile > /etc/resolv.conf
    rm $tmpfile
fi

astaire $@ &
echo -n "ready" > /dev/tcp/192.168.1.4/16123

exit 0

