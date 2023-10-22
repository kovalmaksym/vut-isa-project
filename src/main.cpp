

#include "main.hpp"

int main(int argc, char *argv[]) 
{ 
    bool option_r = false;
    bool option_x = false;
    bool option_ipv6 = false;
    std::string servername;
    std::string addressname;
    int port = DEFAULT_PORT; 

	int opt; 
	while((opt = getopt(argc, argv, "rx6p:s:")) != -1) 
	{ 
		switch(opt) 
		{ 
			case 'r': 
                option_r = true;
				break; 
            case 'x': 
                option_x = true;
				break; 
            case '6': 
                option_ipv6 = true;
				break; 
			case 'p': 
                if (optarg != NULL) {
                    port = atoi(optarg);
                    if (port <= 0 || port > 65535) {
                        std::cout << "Invalid port number." << std::endl;
                        return EXIT_FAILURE;
                    }
                }
				break; 
			case 's': 
                servername = optarg;
				break; 
            default:
                std::cerr << "Usage: " << argv[0] << "dns [-r] [-x] [-6] -s server [-p port] address" << std::endl;
                return EXIT_FAILURE;
        }
    }

    // Check for required arguments
    if (optind == argc) {
        std::cerr << "Missing address argument." << std::endl;
        return 1;
    }

    addressname = argv[optind];

    if (option_r) {
        std::cout << "Recursive query enabled" << std::endl;
    }

    if (option_x) {
        std::cout << "Reverse query enabled" << std::endl;
    }

    if (option_ipv6) {
        std::cout << "AAAA query enabled (IPv6)" << std::endl; 
    }

    std::cout << "Server name: " << servername << std::endl;
    std::cout << "Address name: " << addressname << std::endl;
    std::cout << "Defaut port is 53, port: " << port << std::endl;
	return 0; 
} 