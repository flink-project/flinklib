#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"
#define SIZE 4

int main(int argc, char* argv[]) {
	flink_dev* dev;
	flink_subdev* subdev;
	char* dev_name = DEFAULT_DEV;
	uint8_t subdevice_id = 0;
	char c;
	uint32_t offset;
	uint32_t value;
	uint8_t write;
	int error = 0;
	
	/* Compute command line arguments */
	while((c = getopt(argc, argv, "d:s:o:v:rw")) != -1) {
		switch(c) {
			case 'd': // device
				dev_name = optarg;
				break;
			case 's':
				subdevice_id = atoi(optarg);
				break;
			case 'o':
				offset = atoi(optarg);
				break;
			case 'r':
				write = 0;
				break;
			case 'w':
				write = 1;
				break;
			case 'v':
				if(!write) {
					fprintf(stderr, "You can not set a value if you want to read from a device!");
				}
				else {
					value = atoi(optarg);
				}
				break;
			case '?':
				if(optopt == 'd' || optopt == 's' || optopt == 'o' || optopt == 'v') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	
	subdev = flink_get_subdevice_by_id(dev, subdevice_id);
	
	if(write) {
		printf("Writing 0x%x (%u) to offset 0x%x at subdevice %u on device %s...\n", value, value, offset, subdevice_id, dev_name);
		error = flink_write(subdev, offset, SIZE, &value);
		if(dev == NULL) {
			printf("Failure while writing to device: %u!\n", error);
			return -1;
		}
	}
	else {
		printf("Reading from offset 0x%x at subdevice %u on device %s...\n", offset, subdevice_id, dev_name);
		error = flink_read(subdev, offset, SIZE, &value);
		if(dev == NULL) {
			printf("Failure while reading from device: %u!\n", error);
			return -1;
		}
		else {
			printf("--> Read: 0x%x (%u)\n", value, value);
		}
	}
	
	printf("Closing device %s...\n", dev_name);
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
