#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>

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
	uint8_t       subdevice_id = 0;
	uint16_t      function;
	uint32_t      irq_nr = 0, flink_irq_nr = 0;
	char*         dev_name = DEFAULT_DEV;
	bool          verbose = false;
	int           error = 0;
	uint32_t      signal_offset;
	
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
	while((c = getopt(argc, argv, "d:s:i:f:v")) != -1) {
		switch(c) {
			case 'd': // device file
				dev_name = optarg;
				break;
			case 's': // subdevice id
				subdevice_id = atoi(optarg);
				break;
			case 'i': // irq
				irq_nr = atoi(optarg);
				break;
			case 'f': // flink irq
				flink_irq_nr = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case '?':
				if(optopt == 'd' || optopt == 'i' || optopt == 'f') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	if(function != IRQ_MULTIPLEXER_INTERFACE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}

	// set the irq
	error = flink_set_irq_multiplex(subdev, irq_nr, flink_irq_nr);
	if(error != 0) {
		printf("Could not set IRQ. error: %d!\n", error);
		return EREAD;
	}
	if(verbose) {
		printf("IRQ Nr: %u is set to flink irq: %u \n", irq_nr, flink_irq_nr);
	}
    return EXIT_SUCCESS;
}
