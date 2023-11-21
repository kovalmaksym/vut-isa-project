// VUT FIT ISA - DNS Resolver.
// 20.11.2023
// DNS Resolver header file.
// Author - Maksym Koval <xkoval20@stud.fit.vutbr.cz>

#ifndef DNS_H
#define DNS_H

#include <arpa/inet.h>
#include <ctype.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#pragma pack(1)

// Flags, port, server name and address name
struct Arguments
{
    int option_r;
    int option_x;
    int option_ipv6;
    int option_address;
    int port;
    int is_server_domain;

    char *servername;
    char *addressname;
};

// DNS header structure
struct DNSHeader
{
    unsigned short id;

    unsigned short rd : 1;
    unsigned short tc : 1;
    unsigned short aa : 1;
    unsigned short opcode : 4;
    unsigned short qr : 1;

    unsigned short rcode : 4;
    unsigned short z : 3;
    unsigned short ra : 1;

    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

// DNS query structure
struct DNSQuery
{
    unsigned short qtype;
    unsigned short qclass;
};

struct DNSResRec
{
    unsigned short name;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rdlength;
};

enum DNSQtypes
{
    A = 1,
    NS,
    MD,
    MF,
    CNAME,
    SOA,
    MB,
    MG,
    MR,
    NULL_RR,
    WKS,
    PTR,
    HINFO,
    MINFO,
    MX,
    TXT,

    AAAA = 28,
};

enum DNSQclasses
{
    IN = 1,
    CH,
    HS,
    ANY
};

#define BUFF_MAX_LEN 256
#define QNAME_MAX_LEN 255
#define DNS_MAX_LENGHT 512
#define ANSWER_YES "Yes"
#define ANSWER_NO "No"

// Buffers
char servername[BUFF_MAX_LEN];
char addressname[BUFF_MAX_LEN];

int parse_arguments (struct Arguments *a, int argc,
                     char *argv[]);
void argc_check (int argc);
void print_usage ();
int is_port (char *port_buff);
int set_port (struct Arguments *a, char *port_buff);
void is_address (struct Arguments *a);
void set_address (struct Arguments *a, char *address_buff);
void encode_domain_name (struct Arguments *a,
                         char *dnsPacket,
                         int *packetLength);

void fill_DNSHeader (struct Arguments *a,
                     struct DNSHeader *dnsHeader);
void fill_DNSQuery (struct Arguments *a,
                    struct DNSQuery *dnsQuery);

void print_dns_packet (int length);
void parse_response (char *response);

void parse_name (char *response_without_offset,
                 char *response_ptr, char *label,
                 int *label_len);
char *parse_dns_qtype (int qtype);
char *parse_dns_qclass (int qclass);
int parse_rr (char *response, char *response_ptr);

int is_offset (char byte);
int get_offset (char *offset);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DEFAULT_PORT 53 // default port

#endif