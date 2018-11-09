#!/bin/bash

. <(curl -s https://raw.githubusercontent.com/Polpetta/minibashlib/master/minibashlib.sh)

mb_load "logging"
mb_load "assertions"

function stop_vm(){
    VBoxManage controlvm $1 acpipowerbutton
    while [ "$(VBoxManage list runningvms)" != "" ]
    do
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
        /usr/bin/time -f"%e" -ao "$3" nc -l 16123
        stop_vm $2
    done
}

function check_docker() {
    for i in $(seq 1 $2)
    do
        msg info "Run $i - Docker started"
        docker run --rm -name astaire-test augugrumi/test-stuff:astaire
        /usr/bin/time -f"%e" -ao "$3" nc -l 16123
        msg info "Docker terminated"
    done
}

function main() {
    msg info "-- VM vs Docker Tool Checker, v0.1"
    msg info "Output log: $1"
    msg info "Numer of iterations set: $2"
    msg info "VM name: $3"
    msg info "Docker name: $4"
    msg info "The server is listening in port: $5"

    check_vbox $2 $3 "$1.vm"
#    check_docker $2 $4 "$1.docker"

    msg info "Tests finished. Bye!"
}

main $@
