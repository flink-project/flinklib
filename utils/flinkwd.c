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
	int           time = 10; // [ms]
	uint32_t      repeat = 1;
	uint32_t      counter;
	uint32_t      base_clk;
	bool          verbose = false;
	bool          first = true;
	int           error = 0;
	
	/* Compute command line arguments */
	int c;
	while((c = getopt(argc, argv, "d:s:n:t:v")) != -1) {
		switch(c) {
			case 'd': // device file
				dev_name = optarg;
				break;
			case 's': // subdevice id
				subdevice_id = atoi(optarg);
				break;
			case 'n': // number of repeats
				repeat = atoi(optarg);
				break;
			case 't': // WD timeout
				time = atoi(optarg);
				if(time < 0 || time > 500) {
					fprintf(stderr, "Please enter the timeout in ms (max. 499 ms).\n");
					return EPARAM;
				}
				break;
			case 'v':
				verbose = true;
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
	
	// Read the subdevice base clock
	error = flink_pwm_get_baseclock(subdev, &base_clk);
	if(error != 0) {
		printf("Reading subdevice base clock failed!\n");
		return EREAD;
	}
	if(verbose) {
		printf("Subdevice base clock: %u Hz (%f MHz)\n", base_clk, (float)base_clk / 1000000.0);
	}
	
	// Calculate the counter value for the given timeout
	counter = (uint32_t)(((long)base_clk * (long)time) / 1000);
	if(verbose) {
		printf("Calculated counter value: %u\n", counter);
	}
	
	// Read counter value
	printf("WD active:\n");
	while(repeat-- > 0) {
		error = flink_wd_set_counter(subdev, counter);
		if(error != 0) {
			fprintf(stderr, "stderr, Writing counter failed!\n");
			return EREAD;
		}
		if(first) {
			error = flink_wd_arm(subdev);
			if(error != 0) {
				fprintf(stderr, "stderr, Arming WD failed!\n");
				return EWRITE;
			}
			else {
				if(verbose) printf("WD armed\n");
			}
			first = false;
		}
		if(verbose) {
			printf(".");
			fflush(stdout);
		}
		usleep(800 * time); // retrigger after 80% of the given time
	}
	if(verbose) printf("\n");
	
	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
