#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>

#include <flinklib.h>

#define EOPEN     -1
#define ESUBDEVID -2
#define EREAD     -3
#define EWRITE    -4
#define EPARAM    -5

#define DEFAULT_DEV "/dev/flink0"

int main(int argc, char* argv[]) {
	flink_dev*    dev;
	flink_subdev* subdev;
	uint16_t      function;
	char*         dev_name = DEFAULT_DEV;
	uint8_t       subdevice_id = 0;
	uint32_t      channel = 0;
	int           error = 0;
	bool          output = false;
	bool          val = false;
	
	// Error message if long dashes (en dash) are used
	int i;
	for (i=0; i < argc; i++) {
		 if ((argv[i][0] == 226) && (argv[i][1] == 128) && (argv[i][2] == 147)) {
			fprintf(stderr, "Error: Invalid arguments. En dashes are used.\n");
			return -1;
		 }
	}
	
	/* Compute command line arguments */
	int c;
	while((c = getopt(argc, argv, "d:s:c:rwhl")) != -1) {
		switch(c) {
			case 'd': // device file
				dev_name = optarg;
				break;
			case 's': // subdevice id
				subdevice_id = atoi(optarg);
				break;
			case 'c': // channel
				channel = atoi(optarg);
				break;
			case 'r': // read
				output = false;
				break;
			case 'w': // write
				output = true;
				break;
			case 'h': // set output high
				if(!output) fprintf(stderr, "You can not set a value for an digital input");
				else val = true;
				break;
			case 'l': // set output low
				if(!output) fprintf(stderr, "You can not set a value for an digital input");
				else val = false;
				break;
			case '?':
				if(optopt == 'd' || optopt == 's' || optopt == 'c') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				abort();
		}
	}
	
	// Open flink device
	dev = flink_open(dev_name);
	if(dev == NULL) {
		fprintf(stderr, "Failed to open device %s!\n", dev_name);
		return EOPEN;
	}
	
	// Get a pointer to the choosen subdevice
	subdev = flink_get_subdevice_by_id(dev, subdevice_id);
	if(subdev == NULL) {
		fprintf(stderr, "Illegal subdevice id %d!\n", subdevice_id);
		return ESUBDEVID;
	}
	
	// Check the subdevice function
	function = flink_subdevice_get_function(subdev);
	if(function != GPIO_INTERFACE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}

	// Set I/O direction
	printf("Configuring channel %u of subdevice %u as ", channel, subdevice_id);
	if(output) printf("output\n");
	else printf("input\n");
	error = flink_dio_set_direction(subdev, channel, output);
	if(error != 0) {
		printf("Configuring GPIO direction failed!\n");
		return -1;
	}
	
	// Read or write I/O
	if(output) { // write
		printf("Writing %u to channel %u of subdevice %u\n", val, channel, subdevice_id);
		error = flink_dio_set_value(subdev, channel, val);
		if(error != 0) {
			printf("Writing value failed!\n");
			return EWRITE;
		}
	}
	else { // read
		uint8_t res;
		error = flink_dio_get_value(subdev, channel, &res);
		if(error != 0) {
			printf("Reading value failed!\n");
			return EREAD;
		}
		else {
			printf("Read: %u!\n", res);
		}
	}
	
	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
