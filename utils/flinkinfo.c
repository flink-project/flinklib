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
	int           error = 0;
	char          str[29];
	
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
	if(function != INFO_DEVICE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}

	// Read description
	printf("Reading info device: subdevice %u\n", subdevice_id);
	error = flink_info_get_description(subdev, str);
	if(error != 0) {
		printf("Reading description failed!\n");
		return EREAD;
	} else {
		printf("Description: %s\n", str);
	}

	// Close flink device
	flink_close(dev);

    return EXIT_SUCCESS;
}
