#!/bin/bash

set -e

if [ -n "$KUBERNETES_PORT" ] && [ -n "$DNS_IP" ]
then
    tmpfile=$(mktemp)
    (echo "nameserver $DNS_IP" && tac /etc/resolv.conf) > $tmpfile
    cat $tmpfile > /etc/resolv.conf
    rm $tmpfile
fi

echo -n "ready" > /dev/udp/$ACK_URL/$ACK_PORT
astaire $@

exit 0
