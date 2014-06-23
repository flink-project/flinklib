#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"

int main(int argc, char* argv[]) {
	flink_t* dev;
	char* dev_name = DEFAULT_DEV;
	uint8_t subdevice = 0;
	uint32_t channel = 0;
	uint32_t pwm_period = 0;
	uint32_t pwm_hightime = 0;
	int print_base_clk = 0;
	uint32_t base_clk = 0;
	char c;
	int error = 0;
	
	/* Compute command line arguments */
	while((c = getopt(argc, argv, "d:s:c:p:h:b")) != -1) {
		switch(c) {
			case 'd': // device
				dev_name = optarg;
				break;
			case 's':
				subdevice = atoi(optarg);
				break;
			case 'c':
				channel = atoi(optarg);
				break;
			case 'p':
				pwm_period = atoi(optarg);
				break;
			case 'h':
				pwm_hightime = atoi(optarg);
				break;
			case 'b':
				print_base_clk = 1;
				break;
			case '?':
				if(optopt == 'd' || optopt == 'c' || optopt == 'p' || optopt == 'h') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	
	if(print_base_clk) {
		printf("Reading base clock from subdevice %u...\n", subdevice);
		error = flink_pwm_get_baseclock(dev, subdevice, &base_clk);
		if(error != 0) {
			printf("Reading base clock from subdevice %u failed!\n", subdevice);
			return -1;
		}
		else{
			printf("-> Base clock: %u Hz\n", base_clk);
		}
	}
	
	if(pwm_period > 0) {
		printf("Setting PWM period time to %u (0x%x)...\n", pwm_period, pwm_period);
		error = flink_pwm_set_period(dev, subdevice, channel, pwm_period);
		if(error != 0) {
			printf("Failed to set period time to %u on channel %u at subdevice %u!\n", pwm_period, channel, subdevice);
			return -1;
		}
		
		error = flink_pwm_set_hightime(dev, subdevice, channel, pwm_hightime);
		if(error != 0) {
			printf("Failed to set hight time to %u on channel %u at subdevice %u!\n", pwm_hightime, channel, subdevice);
			return -1;
		}
	}
	
	printf("Closing device %s...\n", dev_name);
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
