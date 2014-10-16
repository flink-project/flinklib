#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"

int main(int argc, char* argv[]) {
	flink_dev* dev;
	char*      dev_name = DEFAULT_DEV;
	bool       verbose = false;
	
	/* Compute command line arguments */
	int c;
	while((c = getopt(argc, argv, "d:v")) != -1) {
		switch(c) {
			case 'd': // device file
				dev_name = optarg;
				break;
			case 'v': // verbose mode
				verbose = true;
				break;
			case '?':
				if(optopt == 'd') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				abort();
		}
	}
	
	dev = flink_open(dev_name);
	if(dev == NULL) {
		printf("Failed to open device!\n");
		return -1;
	}
	
	int nof_subdevs = flink_get_nof_subdevices(dev);
	
	printf("Subdevices of %s (%d):\n", dev_name, nof_subdevs, nof_subdevs);
	
	for(int i = 0; i < nof_subdevs; i++) {
		flink_subdev* subdev = flink_get_subdevice_by_id(dev, i);
		if(subdev) {
			uint8_t  id          = flink_subdevice_get_id(subdev);
			uint32_t baseaddr    = flink_subdevice_get_baseaddr(subdev);
			uint32_t memsize     = flink_subdevice_get_memsize(subdev);
			uint32_t lastaddr    = baseaddr + memsize;
			uint16_t type        = flink_subdevice_get_type(subdev);
			uint8_t  subtype     = flink_subdevice_get_subtype(subdev);
			uint8_t  ifversion   = flink_subdevice_get_ifversion(subdev);
			uint32_t nofchannels = flink_subdevice_get_nofchannels(subdev);
			
			if(verbose) {
				printf("  %3d:\n", id);
				printf("       Address range:      0x%08x - 0x%08x\n", baseaddr, lastaddr);
				printf("       Memory size:        0x%08x (%d bytes)\n", memsize, memsize);
				printf("       Type:               0x%04x (%d)\n", type, type); // TODO print string instead of decimal number
				printf("       Sub type:           0x%02x (%d)\n", subtype, subtype);
				printf("       Interface version:  %d\n", ifversion);
				printf("       Number of channels: %d\n", nofchannels);
			}
			else {
				printf("  %3d: 0x%08x - 0x%08x, Type %d.%d-%d, %d channels\n", id, baseaddr, lastaddr, type, subtype, ifversion, nofchannels);
			}
		}
	}
	
	flink_close(dev);
	
	return EXIT_SUCCESS;
}
