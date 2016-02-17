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
	char*         dev_name = DEFAULT_DEV;
	uint8_t       subdevice_id = 0;
	uint32_t      channel = 0;
	uint32_t      ppwa_period = 0;
	float         period_sec;
	uint32_t      ppwa_hightime = 0;
	uint32_t      base_clk;
	bool          verbose = false;
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
	while((c = getopt(argc, argv, "d:s:c:v")) != -1) {
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
			case 'v':
				verbose = true;
				break;
			case '?':
				if(optopt == 'd' || optopt == 's' || optopt == 'c' || optopt == 'f' || optopt == 'h') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	
	// Read the subdevice base clock
	error = flink_ppwa_get_baseclock(subdev, &base_clk);
	if(error != 0) {
		printf("Reading subdevice base clock failed!\n");
		return EREAD;
	}
	if(verbose) {
		printf("Subdevice base clock: %u Hz (%f MHz)\n", base_clk, (float)base_clk / 1000000.0);
	}
	
	// Read the period time of the PPWA
	error = flink_ppwa_get_period(subdev, channel, &ppwa_period);
	if(error != 0) {
		printf("Reading PPWA period failed!\n");
		return EREAD;
	}
	period_sec = (float)ppwa_period / base_clk;
	printf("PPWA period for channel %d on subdevice %d: %f s (value: %d).\n", channel, subdevice_id, period_sec, ppwa_period);

	// Read the hightime of the PPWA
	error = flink_ppwa_get_hightime(subdev, channel, &ppwa_hightime);
	if(error != 0) {
		printf("Reading PPWA hightime failed!\n");
		return EREAD;
	}
	printf("PPWA hightime for channel %d on subdevice %d: %d%% (value: %d).\n", channel, subdevice_id, ppwa_hightime*100/ppwa_period, ppwa_hightime);
	
	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
