# VUT FIT ISA - DNS Resolver.
20.11.2023
DNS Resolver README file.
Author - Maksym Koval <xkoval20@stud.fit.vutbr.cz>

## DNS Resolver
This program is designed for resolving DNS.
It is designed to create, configure, send and receive DNS packets. Its main task is to demonstrate the ability to work with the DNS protocol and a small demonstration of its extensive capabilities.

## Features

- Domain Name Resolution: Resolves provided domain name to IP addresses.
- Basic DNS Protocol Implementation: Implements DNS protocol for hostname resolution.
- Supported types of RR DNS packet sending: A, AAAA, PTR
- Supported types of RR DNS packet recieving: A, AAAA, PTR, CNAME, NS, SOA

## Prerequisites

- C compiler (e.g., GCC)
- Unix-like operating system

## Usage

- make           - compiles code
- make tar       - creates archive
- make clean     - clean executables
- make run       - run with args: -r -s 8.8.8.8 www.github.com
- make test      - runs all tests
- make test{1,8} - runs a single test

Usage: dns [-r] [-x] [-6] -s server [-p port] address
  -s server  - the server to which the request is sent
  address    - the address that will be resolved
  -r         - recursive query
  -x         - reverse query
  -6         - AAAA query type
  -p <port>  - provide a custom port (default 53)

## Example
- Hostname resolving:
```sh
$ ./dns -s 8.8.8.8 www.google.com
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
 www.google.com, A, IN
Answer section (1)
 www.google.com, A, IN, 165, 172.217.20.4
Authority section (0)
Additional section (0)
```
- Hostname recursive resolving:
```sh
$ ./dns -r -s 8.8.8.8 www.github.com
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
 www.github.com, A, IN
Answer section (2)
 www.github.com, CNAME, IN, 3571, github.com
 github.com, A, IN, 60, 140.82.121.4
Authority section (0)
Additional section (0)
```
- Hostname IPv6 resolving:
```sh
$ ./dns -6 -s 8.8.8.8 www.facebook.com
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
 www.facebook.com, AAAA, IN
Answer section (2)
 www.facebook.com, CNAME, IN, 2508, star-mini.c10r.facebook.com
 star-mini.c10r.facebook.com, AAAA, IN, 50, 2a03:2880:f107:83:face:b00c:0:25de
Authority section (0)
Additional section (0)
```
- Reverse resolving:
```sh
$ ./dns -x -s 8.8.8.8 147.229.9.26in-addr.arpa.
Authoritative: No, Recursive: No, Truncated: No
Question section (1)
 147.229.9.26in-addr.arpa, PTR, IN
Answer section (0)
Authority section (1)
 arpa, SOA, IN, 52182, a.root-servers.net
Additional section (0)
```
- Resolving using a server:
```sh
$ dns -r -s kazi.fit.vutbr.cz www.github.com
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
  www.github.com., A, IN
Answer section (2)
  www.github.com., CNAME, IN, 3600, github.com.
  github.com., A, IN, 60, 140.82.121.3
Authority section (0)
Additional section (0)
```

## Libraries

- <regex.h>
- <getopt.h>
- <stdio.h>
- <stdlib.h>
- <string.h>
- <arpa/inet.h>
- <sys/socket.h>
- <netinet/in.h>
- <unistd.h>
- <stdint.h>
- <ctype.h>
- <netdb.h>