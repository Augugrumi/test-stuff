TCP Proxy section
===

# simple-tcp-proxy.c

Here you can find two TCP proxies. The first one, written in C, 
allows to send an encapsulated packet (with the packet header hardcoded) to an host.

# tcpproxy.py

The second one, written in Python, is more flexible and it gives the possibility to forge the packet header directly from
the program arguments.
For example:
```
./tcpproxy.py --bind-address 0.0.0.0 --port 10125 --dst-ip 192.168.30.13 --dst-port 32444 --recv-ip 192.168.30.19 --recv-port 8080
```
This will launch a proxy, listening in every interface on the port `10125`, with the destination of `192.168.30.13` and port `32444`,
encapsulating the data in a packet that it will have a destination ip of `192.168.30.19` and a destination port of `8080`.

To run the python script you need Python3 and Scrapy.
