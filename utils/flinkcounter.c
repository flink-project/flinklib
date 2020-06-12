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
	uint32_t      value = 0;
	uint32_t      repeat = 1;
	uint32_t      sleep_time = 1000; // [ms]
	int           error = 0;
	
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
	while((c = getopt(argc, argv, "d:s:c:n:t:r")) != -1) {
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
			case 'n': // number of repeats
				repeat = atoi(optarg);
				break;
			case 't': // time to sleep between reading
				sleep_time = atoi(optarg);
				if(sleep_time < 50 || sleep_time > 10000) {
					fprintf(stderr, "Please enter a time in ms (between 50 ms and 10 s)!\n");
					return EPARAM;
				}
				break;
			case '?':
				if(optopt == 'd' || optopt == 's' || optopt == 'c' || optopt == 'n' || optopt == 't') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return EPARAM;
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
	if(function != COUNTER_INTERFACE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}

	// Reset subdevice
	error = flink_subdevice_reset(subdev);
	if(error != 0) {
		fprintf(stderr, "Reseting subdevice %d failed!\n", subdevice_id);
		return EWRITE;
	}
	
	// Read counter value
	printf("Reading counter value(s):\n");
	while(repeat-- > 0) {
		error = flink_counter_get_count(subdev, channel, &value);
		if(error != 0) {
			fprintf(stderr, "stderr, Reading counter failed!\n");
			return EREAD;
		}
		else {
			printf("%u (0x%x)\n", value, value);
		}
		usleep(1000 * sleep_time); 
	}
	
	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
