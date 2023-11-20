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

flink_dev*    dev;
uint32_t      irq_nr = 0;
uint32_t      signal_offset;

void customsignalhandler(int sig) {
	printf("IRQ: %u arrived.\n", sig-signal_offset);
}

void sigintHandler(int signum) {
	int error = 0;
    error = flink_unregister_irq(dev, irq_nr);
	if(error != 0) {
		printf("Could not unregister IRQ. error: %u!\n", error);
	}

	// Close flink device
	flink_close(dev);

    exit(0);
}

int main(int argc, char* argv[]) {
	char*         dev_name = DEFAULT_DEV;
	bool          verbose = false;
	int           error = 0;
	uint32_t      signal_nr = 0;
	
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
	while((c = getopt(argc, argv, "d:i:v")) != -1) {
		switch(c) {
			case 'd': // device file
				dev_name = optarg;
				break;
			case 'i': // irq
				irq_nr = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case '?':
				if(optopt == 'd' || optopt == 'c') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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

	// catch ctrl+c. To avoid a dead irq inside the kernel
	signal(SIGINT, sigintHandler);

	// register the requested irq inside teh kernel. This will also setup the signal inside the kernel
	signal_nr = flink_register_irq(dev, irq_nr);
	if(signal_nr < 0) {
		printf("Could not register IRQ. error: %d!\n", error);
		return EREAD;
	}

	if(verbose) {
		printf("IRQ Nr: %u \n", irq_nr);
	}

	// Read the signal offset
	error = flink_get_signal_offset(dev,&signal_offset);
	if(error != 0) {
		printf("Get signal offset failed!\n");
		return EREAD;
	}
	if(verbose) {
		printf("Signal offset is: %u \n", signal_offset);
		printf("Listening to Signal Nr. (calcuated):      %u \n", signal_offset + irq_nr);
		printf("Listening to Signal Nr. (direct recived): %u \n", signal_nr);
	}

	// Register the routine to a signal that is fired when the IRQ occurs.
	signal(signal_nr, customsignalhandler);

	printf("Press ctrl+c to exit program\n");
	fflush(stdout);

	while (1) {
        sleep(1);
    }	
    return EXIT_SUCCESS;
}
