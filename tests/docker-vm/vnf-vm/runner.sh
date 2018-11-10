#!/bin/bash

. <(curl -s https://raw.githubusercontent.com/Polpetta/minibashlib/master/minibashlib.sh)

mb_load "logging"
mb_load "assertions"

function stop_vm(){
    VBoxManage controlvm $1 acpipowerbutton
    while [ "$(VBoxManage list runningvms)" != "" ]
    do
        msg info "Waiting for machine $1 to being powered off..."
        sleep 3
    done
    msg info info "VM terminated"
}

function check_vbox() {
    for i in $(seq 1 $1)
    do
        msg info "Run $i - VM started"
        VBoxHeadless -s "$2" &
        msg info "Waiting for VM startup..."
        /usr/bin/time -f"%e" -ao "$3" nc -l $4
        stop_vm $2
    done
}

function check_docker() {
    for i in $(seq 1 $1)
    do
        msg info "Run $i - Docker started"
        local TMP_TIMING_REMOVE=$(mktemp)
        /usr/bin/time -f"%e" -o "$TMP_TIMING_REMOVE" nc -l $4 &
        docker run -d --name astaire-test $2
        local START=$(docker inspect --format='{{.State.StartedAt}}' astaire-test)
        local STOP=$(docker inspect --format='{{.State.FinishedAt}}' astaire-test)
        local START_TIMESTAMP=$(($(date --date=$START +%s%N)/1000000))
        local STOP_TIMESTAMP=$(($(date --date=$STOP +%s%N)/1000000))
        local MS_DIFF=$(($STOP_TIMESTAMP-$START_TIMESTAMP))
        local S_DIFF=$(bc -l <<< "$MS_DIFF/1000")
        local FORMATTED_S_DIFF=$(echo $S_DIFF | awk '{printf "%f", $0}')
        msg info "Registered time: $FORMATTED_S_DIFF"
        echo $FORMATTED_S_DIFF >> $3
        docker kill astaire-test 2>/dev/null
        docker rm astaire-test >/dev/null
        rm -f $TMP_TIMING_REMOVE
        msg info "Docker terminated"
    done
}

function main() {
    msg info "-- VM vs Docker Tool Checker, v0.1 --"
    msg info "Output log name: $1"
    msg info "Numer of iterations set: $2"
    msg info "VM name: $3"
    msg info "Docker name: $4"
    msg info "The server is listening in port: $5"

    check_vbox $2 $3 "$1.vm" $5
    check_docker $2 $4 "$1.docker" $5

    msg info "Tests finished. Output file: $1. Number of tests performed: $2. Bye!"
}

main $@
