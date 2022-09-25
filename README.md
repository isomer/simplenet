# simplenet

Simple echo client/server for tcp/udp/dccp/sctp/udplite/mptcp

example:
```shell
$ ./simplenet tcp listen :: 1234
```

```shell
$ echo Hello World | ./simplenet tcp connect :: 1234
```

usage: simplenet _protocol_ connect|listen _hostname_ _port_

Supported protocols:
 * tcp: Transmission Control Protocol
 * udp: User Datagram Protocol
 * dccp: Datagram Congestion Control Protocol
 * sctp: Stream Control Transmission Protocol
 * mptcp: MultiPath Transmission Control Protocol


