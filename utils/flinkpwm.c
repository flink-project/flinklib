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
	int           pwm_frequency = 0; // [Hz]
	int           pwm_hightime_rel = 0; // [%]
	uint32_t      pwm_period = 0;
	uint32_t      pwm_hightime = 0;
	uint32_t      base_clk;
	bool          verbose = false;
	int           error = 0;
	
	/* Compute command line arguments */
	int c;
	while((c = getopt(argc, argv, "d:s:c:f:h:v")) != -1) {
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
			case 'f': // PWM frequency
				pwm_frequency = atoi(optarg);
				break;
			case 'h': // PWM high time
				pwm_hightime_rel = atoi(optarg);
				if(pwm_hightime_rel < 0 || pwm_hightime_rel > 100) {
					fprintf(stderr, "The PWM high time have to be a relative value between 0%% and 100%%!\n");
					return EPARAM;
				}
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
	error = flink_pwm_get_baseclock(subdev, &base_clk);
	if(error != 0) {
		printf("Reading subdevice base clock failed!\n");
		return EREAD;
	}
	if(verbose) {
		printf("Subdevice base clock: %u Hz (%f MHz)\n", base_clk, (float)base_clk / 1000000.0);
	}
	
	// Calculate the period time of the PWM
	if(pwm_frequency > 0 && pwm_frequency < base_clk) {
		pwm_period = base_clk / pwm_frequency;
	}
	else { // error
		fprintf(stderr, "Error while calculating PWM frequency (f_b = %d, f_PWM = %d)!\n", base_clk, pwm_frequency);
		return EPARAM;
	}
	
	pwm_hightime = (pwm_period * pwm_hightime_rel) / 100;
	
	if(pwm_period > 0) {
		printf("Setting PWM frequency for channel %d on subdevice %d to %d Hz (value: 0x%x).\n", channel, subdevice_id, pwm_frequency, pwm_period);
		error = flink_pwm_set_period(subdev, channel, pwm_period);
		if(error != 0) {
			fprintf(stderr, "Failed to set PWM frequency on channel %u at subdevice %u!\n", channel, subdevice_id);
			return EWRITE;
		}
		printf("Setting PWM high time for channel %d on subdevice %d to %d%% (value: 0x%x).\n", channel, subdevice_id, pwm_hightime_rel, pwm_hightime);
		error = flink_pwm_set_hightime(subdev, channel, pwm_hightime);
		if(error != 0) {
			fprintf(stderr, "Failed to set hight time to %u on channel %u at subdevice %u!\n", pwm_hightime, channel, subdevice_id);
			return EWRITE;
		}
	}
	
	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
