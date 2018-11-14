# Utilities to perform ping test

## sfcclient.py
Python script to send a certain number of packets to the sfc waiting for a
response. UDP protocol is used and 8B at time are sent. Result of test are
saved on `result.csv` file.

### Usage
Example:
```bash
..$ python3 client.py --src-ip 192.168.1.6 --src-port 9090 \
--dst-ip 192.168.1.6 --dst-port 9999 \
--ingress-ip 192.168.1.6 --ingress-port 8701 \
--repeat 10
```
Where:
 - `--src-ip` is the ip to specify as source in the additional header
 - `--src-port` is the port to specify as source in the additional header
 - `--dst-ip` is the ip to specify as destination in the additional header
 - `--dst-port` is the port to specify as destination in the additional header
 - `--ingress-ip` is the ip of the ingress (to which forward the packets)
 - `--ingress-port` is the port of the ingress (to which forward the packets)
 - `--repeat` is the number of time to repeat the send

*NB* no proxy needed


## nosfcclient.py
Python script to send a certain number of packets to the destination waiting
for a response. UDP protocol is used and 8B at time are sent. Result of test
are saved on `result.csv` file. It is used to check rtt from source and
destination without sfc overhead.

### Usage
Example:
```bash
..$ python3 nosfcclient.py --repeat 10 --dst-ip 192.168.1.6 --dst-port 9999
```
Where:
 - `--repeat` is the number of time to repeat the send
 - `--dst-ip` is the ip of the destination
 - `--dst-port` is the port of the destination

## UDPEcho.java
Java server that echos the packet received without any other work.

### Compilation
```bash
..$ javac UDPEcho.java
```

### Run
```bash
..$ java UDPEcho <listen-port>
```

## yaml files
Each file has a complete sfc and the number after the prefix sfc indicate the number of link in the chain.