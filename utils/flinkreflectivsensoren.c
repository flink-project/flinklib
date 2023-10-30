/*******************************************************************************************
 *    ____   _____ _______            _________  _____     _____  ____  _____  ___  ____   *
 *   / __ \ / ____|__   __|          |_   ___  ||_   _|   |_   _||_   \|_   _||_  ||_  _|  *
 *  | |  | | (___    | |    _______    | |_  \_|  | |       | |    |   \ | |    | |_/ /    *
 *  | |  | |\___ \   | |   |_______|   |  _|      | |   _   | |    | |\ \| |    |  __'.    *
 *  | |__| |____) |  | |              _| |_      _| |__/ | _| |_  _| |_\   |_  _| |  \ \_  *
 *   \____/|_____/   |_|             |_____|    |________||_____||_____|\____||____||____| *
 *                                                                                         *
 *******************************************************************************************
 *                                                                                         *
 *           flink terminal command, subdevice function "Reflectiv-Sensoren"                  *
 *                                                                                         *
 *******************************************************************************************/
 
/** @file flinkreflectivsensoren.c
 *  @brief flink terminal command, subdevice function "Reflectiv-Sensoren" .
 *
 *  Provides the terminal function "reflectiv-Sensoren". To test 
 *  in a short and easy way the subdevice.
 * 
 *  is based of flinkanaloginput.c
 *
 *  @author Patrick Good
 */


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
	bool          verbose = false;
	int           error = 0;
	uint32_t      resolution = 0;
	uint32_t      value = 0;
	
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
				if(optopt == 'd' || optopt == 's' || optopt == 'c') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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
	if(function != REFLECTIV_INTERFACE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}

	// Read the subdevice resolution
	error = flink_reflectivsensor_get_resolution(subdev,&resolution);
	if(error != 0) {
		printf("Reading subdevice resolution failed!\n");
		return EREAD;
	}
	printf("Subdevice resolution: %u \n", resolution);

	// Read the subdevice value
	error = flink_reflectivsensor_get_value(subdev,channel,&value);
	if(error != 0) {
		printf("Reading subdevice value failed!\n");
		return EREAD;
	}
	printf("Subdevice channel %u has value: %u \n", channel, value);

	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
