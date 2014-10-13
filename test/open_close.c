#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"

int main(int argc, char* argv[]) {
	flink_dev* dev;
	char* dev_name = DEFAULT_DEV;
	char c;
	
	/* Compute command line arguments */
	while((c = getopt(argc, argv, "d:")) != -1) {
		switch(c) {
			case 'd': // device
				dev_name = optarg;
				break;
			case '?':
				if(optopt == 'd') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				abort();
		}
	}
	
	printf("Opening device %s...\n", dev_name);
	dev = flink_open(dev_name);
	if(dev == NULL) {
		printf("Failed to open device!\n");
		return -1;
	}
	
	printf("Closing device %s...\n", dev_name);
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
