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
	
	printf("Subdevices of %s (%d):\n", dev_name, nof_subdevs);
	
	for(int i = 0; i < nof_subdevs; i++) {
		flink_subdev* subdev = flink_get_subdevice_by_id(dev, i);
		if(subdev) {
			uint8_t  id          = flink_subdevice_get_id(subdev);
			uint32_t baseaddr    = flink_subdevice_get_baseaddr(subdev);
			uint32_t memsize     = flink_subdevice_get_memsize(subdev);
			uint32_t lastaddr    = baseaddr + memsize - 1;
			uint16_t func        = flink_subdevice_get_function(subdev);
			uint8_t  subfunc     = flink_subdevice_get_subfunction(subdev);
			uint8_t  func_ver    = flink_subdevice_get_function_version(subdev);
			uint32_t nofchannels = flink_subdevice_get_nofchannels(subdev);
			
			if(verbose) {
				printf("  %3d:\n", id);
				printf("       Address range:      0x%08x - 0x%08x\n", baseaddr, lastaddr);
				printf("       Memory size:        0x%08x (%d bytes)\n", memsize, memsize);
				printf("       Function:           0x%04x (%s)\n", func, flink_subdevice_id2str(func));
				printf("       Sub function:       0x%02x (%d)\n", subfunc, subfunc);
				printf("       Function version:   %d\n", func_ver);
				printf("       Number of channels: %d\n", nofchannels);
			}
			else {
				printf("  %3d: 0x%08x - 0x%08x, Type %d.%d-%d, %d channels\n", id, baseaddr, lastaddr, func, subfunc, func_ver, nofchannels);
			}
		}
	}
	
	flink_close(dev);
	
	return EXIT_SUCCESS;
}
