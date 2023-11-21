// VUT FIT ISA - DNS Resolver.
// 20.11.2023
// DNS Resolver main file.
// Author - Maksym Koval <xkoval20@stud.fit.vutbr.cz>

#include "dns.h"

// Debugging function. Prints provided arguments.
void
debug (struct Arguments *a)
{
    if (a->option_r)
        printf ("Recursive query enabled -R\n");
    if (a->option_x)
        printf ("Reverse query enabled -X\n");
    if (a->option_ipv6)
        printf ("AAAA query enabled -6\n");

    printf ("Server: %s\n", a->servername);
    printf ("Address: %s\n", a->addressname);
    printf ("Port: %i\n", a->port);
}

void
print_usage ()
{
    fprintf (stderr, "Usage: dns [-r] [-x] [-6] -s server "
                     "[-p port] address\n");
    fprintf (stderr, "  -s server  - the server to which "
                     "the request is sent\n");
    fprintf (stderr, "  address    - the address that will "
                     "be resolved\n");
    fprintf (stderr, "  -r         - recursive query\n");
    fprintf (stderr, "  -x         - reverse query\n");
    fprintf (stderr, "  -6         - AAAA query type\n");
    fprintf (stderr, "  -p <port>  - provide a custom port "
                     "(default 53)\n");
}

// Function checks argc number.
void
argc_check (int argc)
{
    if (argc < 4 || argc > 7)
        {
            fprintf (stderr, "Wrong arguments.\n");
            print_usage ();
            exit (EXIT_FAILURE);
        }
}

// Function valides a provided port.
int
is_port (char *port_buff)
{
    regex_t reegex;
    int regex_port
        = regcomp (&reegex, "^[0-9]\\{1,\\}$", 0);

    regex_port = regexec (&reegex, port_buff, 0, NULL, 0);
    if (regex_port)
        {
            return 0;
        }
    return 1;
}

// Function sets and validates a port.
int
set_port (struct Arguments *a, char *port_buff)
{
    a->port = atoi (port_buff);
    if (a->port <= 0 || a->port > 65535)
        {
            return 0;
        }
    return 1;
}

// Function sets an address in case if it is not set.
void
set_address (struct Arguments *a, char *address_buff)
{

    if (a->option_address == 0)
        {
            a->option_address = 1;
            a->addressname = address_buff;
        }
    else
        {
            fprintf (stderr,
                     "Only one address in allowed.\n");
            print_usage ();
            exit (EXIT_FAILURE);
        }
}

// Function check if address is set.
void
is_address (struct Arguments *a)
{
    if (a->option_address == 0)
        {
            fprintf (stderr, "Address is not set.\n");
            print_usage ();
            exit (EXIT_FAILURE);
        }
}

// Function to parse input arguments.
int
parse_arguments (struct Arguments *a, int argc,
                 char *argv[])
{

    int opt = 0;
    argc_check (argc);
    while (optind < argc)
        {
            if ((opt = getopt (argc, argv, "rx6p:s:"))
                != -1)
                {
                    switch (opt)
                        {
                        case 'r':
                            a->option_r = 1;
                            break;
                        case 'x':
                            a->option_x = 1;
                            break;
                        case '6':
                            a->option_ipv6 = 1;
                            break;
                        case 'p':
                            if (!is_port (optarg)
                                || !set_port (a, optarg))
                                {
                                    fprintf (stderr,
                                             "Invalid port "
                                             "number.\n");
                                    exit (EXIT_FAILURE);
                                }
                            break;
                        case 's':
                            a->servername = optarg;
                            break;
                        case '?':
                            print_usage ();
                            exit (EXIT_FAILURE);
                        default:
                            fprintf (stderr,
                                     "Unknown option.\n");
                            print_usage ();
                            exit (EXIT_FAILURE);
                        }
                }
            else
                {
                    set_address (a, argv[optind]);
                    optind++;
                }
        }
    is_address (a);
    return 0;
}

// Function to configure DNS Header structure.
void
fill_DNSHeader (struct Arguments *a,
                struct DNSHeader *dnsHeader)
{
    int pid = getpid ();
    dnsHeader->id = htons (pid);
    dnsHeader->rd = a->option_r;
    dnsHeader->qdcount = htons (1);
}

// Function to configure DNS Query structure.
void
fill_DNSQuery (struct Arguments *a,
               struct DNSQuery *dnsQuery)
{
    if (a->option_ipv6)
        {
            dnsQuery->qtype = htons (AAAA);
        }
    else if (a->option_x)
        {
            dnsQuery->qtype = htons (PTR);
        }
    else
        {
            dnsQuery->qtype = htons (A);
        }
    dnsQuery->qclass = htons (IN);
}

// Function to configure sockaddr_in structure.
void
fill_ServerAddr (struct Arguments *a,
                 struct sockaddr_in *serverAddr)
{

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons (a->port);

    serverAddr->sin_addr.s_addr = inet_addr (a->servername);
}

// Function checks if provided server is a valid hostname or
// IP (only for -s).
void
check_ip_or_domain (struct Arguments *a)
{
    int is_server_valid = 0;

    regex_t reegex_domainname;
    regex_t reegex_ipv4;

    memset (&reegex_domainname, 0,
            sizeof (reegex_domainname));
    memset (&reegex_ipv4, 0, sizeof (reegex_ipv4));

    const char *pattern_domain
        = "^[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?(\\."
          "[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?)*\\."
          "([A-Za-z]{2,6})$";
    const char *pattern_ipv4
        = "^([0-9]{1,3}\\.){3}[0-9]{1,3}$";

    int regex_domainname = regcomp (
        &reegex_domainname, pattern_domain, REG_EXTENDED);
    if (regex_domainname)
        {
            fprintf (stderr,
                     "Could not compile hostname regex.\n");
            exit (EXIT_FAILURE);
        }

    int regex_ipv4 = regcomp (&reegex_ipv4, pattern_ipv4,
                              REG_EXTENDED);
    if (regex_ipv4)
        {
            fprintf (stderr,
                     "Could not compile IPv4 regex.\n");
            exit (EXIT_FAILURE);
        }

    // check if provided server is a valid domain name.
    regex_domainname = regexec (&reegex_domainname,
                                a->servername, 0, NULL, 0);
    if (regex_domainname != REG_NOMATCH)
        {
            is_server_valid = 1;
            a->is_server_domain = 1;
        }

    // check if provided server is a valid IP.
    regex_ipv4
        = regexec (&reegex_ipv4, a->servername, 0, NULL, 0);
    if (regex_ipv4 != REG_NOMATCH)
        {
            is_server_valid = 1;
        }

    if (!is_server_valid)
        {
            fprintf (stderr,
                     "Provided server is unvalid.\n");
            exit (EXIT_FAILURE);
        }
}

// Gets ip of the provided server (-s server).
void
domain_to_ip (struct Arguments *a)
{

    struct hostent *host_info;

    host_info = gethostbyname (a->servername);
    if (host_info == NULL)
        {
            fprintf (stderr, "Error getting server IP.\n");
            exit (EXIT_FAILURE);
        }

    a->servername
        = inet_ntoa (*(struct in_addr *)host_info->h_addr);

    return;
}

// Main function of DNS Resolver.
int
main (int argc, char *argv[])
{
    int domain_name_lenght;

    int sockfd;
    int packetLength = 0;

    socklen_t len;
    struct sockaddr_in serverAddr;
    struct Arguments a;
    struct DNSHeader dnsHeader;
    struct DNSQuery dnsQuery;
    struct timeval tv;
    char response[DNS_MAX_LENGHT] = { 0 };
    char dnsPacket[DNS_MAX_LENGHT] = { 0 };

    memset (&serverAddr, 0, sizeof (serverAddr));
    memset (&a, 0, sizeof (a));
    memset (&dnsHeader, 0, sizeof (dnsHeader));
    memset (&dnsQuery, 0, sizeof (dnsQuery));

    // Timeout for 5 secs
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    a.port = DEFAULT_PORT;
    parse_arguments (&a, argc, argv);
    // debug(&a);

    check_ip_or_domain (&a);

    if (a.is_server_domain)
        {
            domain_to_ip (&a);
        }

    fill_ServerAddr (&a, &serverAddr);
    fill_DNSHeader (&a, &dnsHeader);
    fill_DNSQuery (&a, &dnsQuery);

    // Create a socket
    if ((sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP))
        < 0)
        {
            fprintf (stderr,
                     "Failed to create a socket.\n");
            exit (EXIT_FAILURE);
        }

    // Set socket timeout
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv,
                    sizeof (tv))
        < 0)
        {
            fprintf (stderr, "Setsockopt failed.\n");
            exit (EXIT_FAILURE);
        }

    char *packetPtr = dnsPacket;

    // Copy the header into the packet buffer
    memcpy (dnsPacket, &dnsHeader,
            sizeof (struct DNSHeader));
    packetLength += sizeof (struct DNSHeader);
    packetPtr += sizeof (struct DNSHeader);

    // Encode the domain name into the packet
    encode_domain_name (&a, packetPtr, &domain_name_lenght);
    packetLength += domain_name_lenght;
    packetPtr += domain_name_lenght;

    // Set the DNS query
    memcpy (packetPtr, &dnsQuery, sizeof (struct DNSQuery));
    packetLength += sizeof (struct DNSQuery);
    packetPtr += sizeof (struct DNSQuery);

    // Send the DNS packet to the DNS server
    if (sendto (sockfd, &dnsPacket, packetLength, 0,
                (struct sockaddr *)&serverAddr,
                sizeof (serverAddr))
        < 0)
        {
            fprintf (stderr, "Failed to send a packet.\n");
            close (sockfd);
            exit (EXIT_FAILURE);
        }

    // Receive the DNS response
    int bytes_received
        = recvfrom (sockfd, response, DNS_MAX_LENGHT, 0,
                    (struct sockaddr *)&serverAddr, &len);
    if (bytes_received == -1)
        {
            fprintf (stderr,
                     "Failed to receive DNS response.\n");
            close (sockfd);
            exit (EXIT_FAILURE);
        }
    parse_response (response);
    close (sockfd);
    return 0;
}

// Function that decodes response and prints output.
void
parse_response (char *response)
{

    // void parse_response(char * response) {
    char buf_label[BUFF_MAX_LEN] = { 0 };
    int questions_number = 0;
    int answers_number = 0;
    int authorities_number = 0;
    int additions_number = 0;
    int size = 0;
    int label_len = 0;
    char *response_ptr = 0;
    struct DNSHeader *dnsHeader;
    struct DNSQuery *dnsQuery;

    dnsHeader = (struct DNSHeader *)response;

    printf (
        "Authoritative: %s, Recursive: %s, Truncated: %s\n",
        dnsHeader->aa ? ANSWER_YES : ANSWER_NO,
        dnsHeader->rd ? ANSWER_YES : ANSWER_NO,
        dnsHeader->tc ? ANSWER_YES : ANSWER_NO);

    questions_number = htons (dnsHeader->qdcount);
    answers_number = htons (dnsHeader->ancount);
    authorities_number = htons (dnsHeader->nscount);
    additions_number = htons (dnsHeader->arcount);

    response_ptr = response + sizeof (*dnsHeader);

    printf ("Question section (%i)\n",
            ntohs (dnsHeader->qdcount));
    for (; questions_number > 0; questions_number--)
        {

            parse_name (response, response_ptr, buf_label,
                        &label_len);
            response_ptr += label_len;
            dnsQuery = (struct DNSQuery *)response_ptr;
            printf (
                " %s, %s, %s\n", buf_label,
                parse_dns_qtype (htons (dnsQuery->qtype)),
                parse_dns_qclass (
                    htons (dnsQuery->qclass)));
            response_ptr += sizeof (*dnsQuery);
        }

    printf ("Answer section (%i)\n",
            ntohs (dnsHeader->ancount));
    for (; answers_number > 0; answers_number--)
        {

            size = parse_rr (response, response_ptr);
            response_ptr += size;
        }

    printf ("Authority section (%i)\n",
            ntohs (dnsHeader->nscount));
    for (; authorities_number > 0; authorities_number--)
        {

            size = parse_rr (response, response_ptr);
            response_ptr += size;
        }

    printf ("Additional section (%i)\n",
            ntohs (dnsHeader->arcount));
    for (; additions_number > 0; additions_number--)
        {

            size = parse_rr (response, response_ptr);
            response_ptr += size;
        }

    return;
}

// Parses DNS Qtype to string value.
char *
parse_dns_qtype (int qtype)
{
    switch (qtype)
        {
        case A:
            return "A";
            break;
        case AAAA:
            return "AAAA";
        case CNAME:
            return "CNAME";
        case NS:
            return "NS";
        case SOA:
            return "SOA";
        case PTR:
            return "PTR";
        default:
            break;
        }
    return "UNK";
}

// Parses Resourse Record and prints output.
int
parse_rr (char *response, char *response_ptr)
{
    struct DNSResRec *dnsRR = 0;
    char buf_tmp[BUFF_MAX_LEN] = { 0 };
    char buf_label[BUFF_MAX_LEN] = { 0 };
    int label_len = 0;
    int ttl = 0;

    dnsRR = (struct DNSResRec *)response_ptr;

    ttl = ntohl (dnsRR->ttl);
    unsigned short type = ntohs (dnsRR->type);
    unsigned short class = ntohs (dnsRR->class);
    unsigned short rdlenght = ntohs (dnsRR->rdlength);

    // Calculate 14 bit offset using & 0011 1111 1111 1111
    unsigned short offset = htons (dnsRR->name) & 0x3fff;

    parse_name (response, response + offset, buf_label,
                &label_len);
    response_ptr += sizeof (*dnsRR);

    switch (type)
        {
        case A:
            inet_ntop (AF_INET, response_ptr, buf_tmp,
                       INET_ADDRSTRLEN);
            printf (" %s, %s, %s, %i, %s\n", buf_label,
                    parse_dns_qtype (type),
                    parse_dns_qclass (class), ttl, buf_tmp);
            break;
        case AAAA:
            inet_ntop (AF_INET6, response_ptr, buf_tmp,
                       INET6_ADDRSTRLEN);
            printf (" %s, %s, %s, %i, %s\n", buf_label,
                    parse_dns_qtype (type),
                    parse_dns_qclass (class), ttl, buf_tmp);
            break;
        case CNAME:
        case PTR:
        case NS:
        case SOA:
            parse_name (response, response_ptr, buf_tmp,
                        &label_len);
            printf (" %s, %s, %s, %i, %s\n", buf_label,
                    parse_dns_qtype (type),
                    parse_dns_qclass (class), ttl, buf_tmp);
            break;

        default:
            printf (" Unsupported RR type.\n");
            break;
        }

    response_ptr += rdlenght;
    return sizeof (*dnsRR) + rdlenght;
}

// Function parses DNS Qclass to string.
char *
parse_dns_qclass (int qclass)
{
    switch (qclass)
        {
        case IN:
            return "IN";
        case CH:
            return "CH";
        case HS:
            return "HS";
        case ANY:
            return "ANY";
        default:
            break;
        }
    return "UNK";
}

// Check if byte is a DNS response offset.
int
is_offset (char byte)
{

    if ((byte & 0xC0) == 0xC0)
        {
            return 1;
        }
    return 0;
}

// Function to calculate offset.
int
get_offset (char *offset)
{
    return ((offset[0] & 0x3F) << 8) | offset[1];
}

// Function to parse name as 3www3fit3vut2cz to
// www.fit.vut.cz
void
parse_name (char *response_without_offset,
            char *response_ptr, char *label, int *label_len)
{
    int i = 0;
    int offset = 0;
    int token_lenght = 0;
    for (; i < QNAME_MAX_LEN;)
        {

            if (is_offset (*response_ptr))
                {

                    offset = get_offset (response_ptr);
                    response_ptr
                        = response_without_offset + offset;
                    continue;
                }

            if (*response_ptr == 0)
                {
                    *(--label) = 0;
                    i++;
                    break;
                }

            token_lenght = *response_ptr;
            response_ptr++;

            if (token_lenght <= 0)
                {
                    *(--label) = 0;
                    i++;
                    break;
                }
            memcpy (label, response_ptr, token_lenght);
            label += token_lenght;

            // adding dot after token
            *label = '.';
            // moving pointer to next char
            label += 1;
            // increasing length of the word because of '.'
            i += 1;

            response_ptr += token_lenght;
            i += token_lenght;
        }

    *label_len = i;
}

// Encodes domain name www.fit.vut.cz to 3www3fit3vut2cz
void
encode_domain_name (struct Arguments *a, char *packetPtr,
                    int *packetLength)
{
    char tmp[BUFF_MAX_LEN] = { 0 };
    strcpy (tmp, a->addressname);

    char *token = strtok (tmp, ".");
    int domain_name_lenght = 0;
    while (token != NULL)
        {
            int token_size = strlen (token);
            *packetPtr = token_size;
            packetPtr++;
            domain_name_lenght += 1;

            strcpy (packetPtr, token);
            packetPtr += (int)strlen (token);

            domain_name_lenght += strlen (token);

            token = strtok (NULL, ".");
        }

    domain_name_lenght += 1;
    *packetLength = domain_name_lenght;
}