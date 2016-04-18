#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"
#define INFO_DEVICE_ID 0
#define INFO_DEVICE_UNIQUE_ID 1
#define INFO_DEVICE_DESCRIPTOR "flinkgpiotest"
#define INFO_DEVICE_DESCRIPTOR_LENGTH strlen(INFO_DEVICE_DESCRIPTOR)
#define INFO_FUNCTION_ID 0x0
#define GPIO_FUNCTION_ID 0x5
#define IN_GPIO_UNIQUE_ID 0x2
#define OUT_GPIO_UNIQUE_ID 0x3
#define NUMBER_OF_CHANNELS_TEST 128
#define NUMBER_OF_TESTS 1000


int testInfoDevice(flink_dev* dev,int unique_id, char* designDescriptor,int descriptorLength);


int main(int argc, char* argv[]) {
	flink_dev* dev;
	char*      dev_name = DEFAULT_DEV;
	bool       verbose = false;
	int error = 0;
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
	printf("FLink GPIO Testing\n");
	dev = flink_open(dev_name);
	if(dev == NULL) {
		printf("Failed to open device!\n");
		return -1;
	}
	
	int nof_subdevs = flink_get_nof_subdevices(dev);

	if(nof_subdevs != 3){
		printf("Wrong number of subdevices: %d\n",nof_subdevs);
		return -1;
	}
	
	if (testInfoDevice(dev,INFO_DEVICE_UNIQUE_ID, INFO_DEVICE_DESCRIPTOR,INFO_DEVICE_DESCRIPTOR_LENGTH) != 0 ){
		printf("Testing info device error! Is the right design loaded?\n");
	}


	
	flink_subdev* out_gpio_device = flink_get_subdevice_by_unique_id(dev, OUT_GPIO_UNIQUE_ID);
	if(out_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",OUT_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(out_gpio_device) != GPIO_FUNCTION_ID) { //TODO get from include
		printf("out gpio device has wrong function id!\n");
		return -1;
	}


	flink_subdev* in_gpio_device = flink_get_subdevice_by_unique_id(dev, IN_GPIO_UNIQUE_ID);
	if(in_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",IN_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(in_gpio_device) != GPIO_FUNCTION_ID) { //TODO get from include
		printf("in gpio device has wrong function id!\n");
		return -1;
	}


	int number_of_channels = flink_subdevice_get_nofchannels(out_gpio_device);
	if(flink_subdevice_get_nofchannels(out_gpio_device) != number_of_channels){
		printf("gpio devices have not the same amount of channels!\n");
		return -1;
	}
	
	if(number_of_channels != NUMBER_OF_CHANNELS_TEST){
		printf("gpio devices wrong number of channels!\n");
		return -1;
	}

	for(int i = 0; i < NUMBER_OF_CHANNELS_TEST; i++){
		error = flink_dio_set_direction(out_gpio_device, i, FLINK_OUTPUT);
		if(error != 0) {
			printf("Configuring out GPIO direction failed!\n");
			return -1;
		}
		error = flink_dio_set_direction(in_gpio_device, i, FLINK_INPUT);
		if(error != 0) {
			printf("Configuring in GPIO direction failed!\n");
			return -1;
		}
	}

		
	//set out gpio and imidiatly read in gpio 
	int set_read_error = 0;
	for(int i = 0; i < NUMBER_OF_CHANNELS_TEST; i++){
		uint8_t result = 0;
		error = flink_dio_set_value(out_gpio_device, i, 1);
		if(error != 0) {
			printf("Set value error!\n");
			return -1;
		}
		error = flink_dio_get_value(in_gpio_device, i, &result);
		if(error != 0) {
			printf("Get value error!\n");
			return -1;
		}
		if(result != 1){
			set_read_error++;		
		}
	}
	if(set_read_error != 0){
		printf("Error set gpio to high and read imidiatly back: %d\n",set_read_error);
		return -1;
	}


	//set random channel to random value
	int channels[NUMBER_OF_TESTS];
	int values[NUMBER_OF_TESTS];

	srand(time(NULL));
	for(int i = 0; i< NUMBER_OF_TESTS; i++){
		int r = rand();
		float f = r*1.0/RAND_MAX;
		channels[i] = (int) (f*NUMBER_OF_CHANNELS_TEST);
		r = rand();
		f = r*1.0/RAND_MAX;
		if(f>0.5){
			values[i] = 1;			
		}else{
			values[i] = 0;	
		}
	}

	set_read_error = 0;
	for(int i = 0; i< NUMBER_OF_TESTS; i++){
		uint8_t result = 0;
		flink_dio_set_value(out_gpio_device, channels[i], values[i]);
		flink_dio_get_value(in_gpio_device, channels[i], &result);
		if(result != values[i]){
			set_read_error++;		
		}
	}
	if(set_read_error != 0){
		printf("Error set random number to random channel: %d\n",set_read_error);
		return -1;
	}




	//set all gpios first than read them
	for(int i = 0; i< NUMBER_OF_TESTS;i++){
		values[i] = 0;
		channels[i] = 0;	
	}
	for(int i = 0; i + NUMBER_OF_CHANNELS_TEST < NUMBER_OF_TESTS; i = i + NUMBER_OF_CHANNELS_TEST){
		int r = rand();
		float f = r*1.0/RAND_MAX;
		int firstchannel = (int) (f*NUMBER_OF_CHANNELS_TEST);
		for (int u = 0; u < NUMBER_OF_CHANNELS_TEST; u++){
			
			channels[i+u] =	(firstchannel + u) % NUMBER_OF_CHANNELS_TEST;
			r = rand();
			f = r*1.0/RAND_MAX;
			if(f>0.5){
				values[i+u] = 1;			
			}else{
				values[i+u] = 0;	
			}	
		}
	}

	set_read_error = 0;
	for(int i = 0; i + NUMBER_OF_CHANNELS_TEST < NUMBER_OF_TESTS; i = i + NUMBER_OF_CHANNELS_TEST){
		for (int u = 0; u < NUMBER_OF_CHANNELS_TEST; u++){
			flink_dio_set_value(out_gpio_device, channels[i+u], values[i+u]);
			
		}
		uint8_t result = 0;
		for (int u = 0; u < NUMBER_OF_CHANNELS_TEST; u++){
			if(i+u<NUMBER_OF_TESTS){
				flink_dio_get_value(in_gpio_device, channels[i+u], &result);
				if(result != values[i+u]){
					printf("Error at channel: %d, value: %d, result: %d, nr= %d\n",channels[i+u],values[i+u],result,i+u);
					set_read_error++;		
				}
 			}
		}
	}
	if(set_read_error != 0){
		printf("Error set random number to random channel first write than read: %d\n",set_read_error);
		return -1;
	}


	printf("Test Successfull!\n");
	flink_close(dev);
	
	return EXIT_SUCCESS;
}





int testInfoDevice(flink_dev* dev,int unique_id, char* designDescriptor,int descriptorLength){
	char str[INFO_DESC_SIZE];
	char descriptor[descriptorLength + 1];

	flink_subdev* info_device = flink_get_subdevice_by_id(dev, 0); //Info device is allways at id zero
	if(flink_subdevice_get_function(info_device) != INFO_FUNCTION_ID){ //TODO add function id
		printf("Subdevice with id %d is no info device\n",0);
		return -1;
	}
	if(flink_subdevice_get_unique_id(info_device) != unique_id){
		printf("Info device wrong unique id\n");
		return -1;
	}
	
	if(flink_info_get_description(info_device, str) != 0) {
		printf("Reading info device description failed!\n");
		return -1;
	} 
	int u = 0;
	for(int i = 0; i < INFO_DESC_SIZE;i++){
		if(str[i]!=0 && u < descriptorLength){
			descriptor[u] = str[i];	
			u++;	
			
		}
	}
	descriptor[descriptorLength] = 0;	
	if (strcmp(descriptor,designDescriptor) != 0) {
		printf("Wrong descriptor from info device: %s/%s \n",descriptor,designDescriptor);
		return -1;
	}	
	return 0;
}
