// Program to illustrate the getopt() 
// function in C 

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <getopt.h>
#include <ctype.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DEFAULT_PORT 53 // default port

int main(int argc, char *argv[]) 
{ 
    int port = DEFAULT_PORT; 

	int opt; 
	while((opt = getopt(argc, argv, "rx6p::")) != -1) 
	{ 
		switch(opt) 
		{ 
			case 'r': 
				printf("Recursive query enabled\n"); 
				break; 
            case 'x': 
				printf("Reverse query enabled\n"); 
				break; 
            case '6': 
				printf("AAAA query enabled\n"); 
				break; 
			case 'p': 
                printf("defaut port is 53, optarg=%s\n", optarg); 
                if (optarg != NULL) {
                    port = atoi(optarg);
                }
				break; 

            // case ':':
            //     switch (optopt)
            //     {
            //     case 'p':
            //         printf("option -%c with default argument value\n", optopt);
            //         break;
            //     default:
            //         fprintf(stderr, "option -%c is missing a required argument\n", optopt);
            //         return EXIT_FAILURE;
            //     }
            //     break;
            case '?':
                fprintf(stderr, "invalid option: -%c\n", optopt);
                return EXIT_FAILURE;
        }
    }
    printf("port: %i\n", port);
	return 0; 
} 
