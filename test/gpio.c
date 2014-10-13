#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"

int main(int argc, char* argv[]) {
	flink_dev*    dev;
	flink_subdev* subdev;
	char*         dev_name = DEFAULT_DEV;
	uint8_t       subdevice_id = 0;
	uint32_t      channel = 0;
	char          c;
	int           error = 0;
	uint8_t       output;
	uint8_t       val;
	
	/* Compute command line arguments */
	while((c = getopt(argc, argv, "d:s:c:rwhl")) != -1) {
		switch(c) {
			case 'd': // device
				dev_name = optarg;
				break;
			case 's':
				subdevice_id = atoi(optarg);
				break;
			case 'c':
				channel = atoi(optarg);
				break;
			case 'r':
				output = 0;
				break;
			case 'w':
				output = 1;
				break;
			case 'h':
				if(!output) {
					fprintf(stderr, "You can not set a value for an digital input");
				}
				else {
					val = 1;
				}
				break;
			case 'l':
				if(!output) {
					fprintf(stderr, "You can not set a value for an digital input");
				}
				else {
					val = 0;
				}
				break;
			case '?':
				if(optopt == 'd' || optopt == 'c' || optopt == 'p') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	
	subdev = flink_get_subdevice_by_id(dev, subdevice_id);
	
	printf("Configuring channel %u of subdevice %u as ", channel, subdevice_id);
	if(output) {
		printf("output\n");
	}
	else {
		printf("input\n");
	}
	error = flink_dio_set_direction(subdev, channel, output);
	
	if(error != 0) {
		printf("Configuring GPIO direction failed!\n");
		return -1;
	}
	
	printf("Writing %u to channel %u of subdevice %u\n", val, channel, subdevice_id);
	if(output) { // write
		error = flink_dio_set_value(subdev, channel, val);
		if(error != 0) {
			printf("Writing value failed!\n");
			return -1;
		}
	}
	else { // read
		uint8_t res;
		error = flink_dio_get_value(subdev, channel, &res);
		if(error != 0) {
			printf("Reading value failed!\n");
			return -1;
		}
		else {
			printf("Read: %u!\n", res);
		}
	}
	
	printf("Closing device %s...\n", dev_name);
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
