# UDP proxy for header creation
This simple udp prroxy allow to specify the header UDP that will be created and
added to the original packet in order to be used by the chain.

```
                                     -------------
                                     |  addition |    header with data specified
            -------------            -------------
original    |           |            |           |    original
payload     |  original |   ----->   |  original |    payload
sent        |           |            |           |    sent
            -------------            -------------
```

## Dependencies
To create the additional header it use the library [Scapy](https://scapy.net/) for python.

## Usage
Example
```bash
..$ python3 udpproxy.py --proxy-ip 192.168.1.6 --proxy-port 8706 \
--src-ip 192.168.1.6 --src-port 8706 --dst-ip 192.168.1.6 \
--dst-port 8705 --ingress-ip 172.20.0.3 --ingress-port 8701
```

Where:
 - `--proxy-ip` is the ip used by the proxy
 - `--proxy-port` is the port used by the proxy
 - `--src-ip` is the ip to specify as source in the additional header
 - `--src-port` is the port to specify as source in the additional header
 - `--dst-ip` is the ip to specify as destination in the additional header
 - `--dst-port` is the port to specify as destination in the additional header
 - `--ingress-ip` is the ip of the ingress (to which forward the packets)
 - `--ingress-port` is the port of the ingress (to which forward the packets)
