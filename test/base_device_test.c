#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <flinklib.h>
#include <../flinkinterface/flink_funcid.h>


#define DEFAULT_DEV "/dev/flink0"


#define INFO_DEVICE_DESCRIPTOR "baseDeviceTesting"
#define INFO_DEVICE_DESCRIPTOR_LENGTH strlen(INFO_DEVICE_DESCRIPTOR)

#define COUNTER_FUNCTION_ID 0x6



#define NUMBER_OF_SUBDEVICES 8
#define INFO_DEVICE_UNIQUE_ID 1
#define ENC_B_GPIO_UNIQUE_ID 0x2
#define ENC_A_GPIO_UNIQUE_ID 0x3
#define FQD_GPIO_UNIQUE_ID 0x4

#define PWM_UNIQUE_ID 0x5
#define PWM_IN_GPIO_UNIQUE_ID 0x6
#define NUMBER_OF_PWM_CHANNELS 4
#define PWM_RATIO_THRESHOLD 0.2
#define PWM_PERIOD_THRESHOLD 0.2
#define NUMBER_OF_PWM_RATIO_PERIOD_TEST 100
#define PWM_TIMEOUT 10000000



#define IN_GPIO_UNIQUE_ID 0x7
#define OUT_GPIO_UNIQUE_ID 0x8
#define NUMBER_OF_GPIO_CHANNELS_TEST 128
#define NUMBER_OF_GPIO_TESTS 1000





#define NUMBER_OF_FQD_CHANNELS 4
#define OUT_IO_SUBFUNCTION_ID 1
#define IN_IO_SUBFUNCTION_ID 2



int testInfoDevice(flink_dev* dev,int unique_id, char* designDescriptor,int descriptorLength);
void stepVorward(int channel);
void stepBackward(int channel);
int testFQD();
int testGPIODevice();
int testPWMDevice();
int testRatio(float ratio,uint32_t period,int channel);


flink_dev* dev;
flink_subdev* enc_a_gpio_device;
flink_subdev* enc_b_gpio_device;


int main(int argc, char* argv[]) {
	char*      dev_name = DEFAULT_DEV;

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
	printf("FLink Base Device Testing\n");
	
	//get flink device
	dev = flink_open(dev_name);
	if(dev == NULL) {
		printf("Failed to open device!\n");
		return -1;
	}
	int nof_subdevs = flink_get_nof_subdevices(dev);

	if(nof_subdevs != NUMBER_OF_SUBDEVICES){
		printf("Wrong number of subdevices: %d\n",nof_subdevs);
		return -1;
	}



	//test info device
	printf("Testing info device.....\n");
	if (testInfoDevice(dev,INFO_DEVICE_UNIQUE_ID, INFO_DEVICE_DESCRIPTOR,INFO_DEVICE_DESCRIPTOR_LENGTH) != 0 ){
		printf("Testing info device error! Is the right design loaded?\n");
	}

	printf("Testing FQD device.....\n");
	if(testFQD() != 0){
		printf("Testing fqd device error!\n");
	}
	printf("Testing GPIO device.....\n");
	if(testGPIODevice() != 0){
		printf("Testing gpio device error!\n");
	}
	printf("Testing PWM device.....\n");
	if(testPWMDevice() != 0){
		printf("Testing pwm device error!\n");
	}


	printf("Test Successfull!\n");
	flink_close(dev);
	
	return EXIT_SUCCESS;
}


void stepVorward(int channel){
	uint8_t value_a;
	uint8_t value_b;
	flink_dio_get_value(enc_a_gpio_device, channel,&value_a);
	flink_dio_get_value(enc_b_gpio_device, channel,&value_b);

	if(value_a == 0 && value_b == 0){
		flink_dio_set_value(enc_a_gpio_device, channel ,0x1);
	}else if(value_a == 1 && value_b == 0){
		flink_dio_set_value(enc_b_gpio_device, channel ,0x1);
	}else if(value_a == 1 && value_b == 1){
		flink_dio_set_value(enc_a_gpio_device, channel ,0x0);
	}else if(value_a == 0 && value_b == 1){
		flink_dio_set_value(enc_b_gpio_device, channel ,0x0);
	}
}

void stepBackward(int channel){
	uint8_t value_a;
	uint8_t value_b;
	flink_dio_get_value(enc_a_gpio_device, channel,&value_a);
	flink_dio_get_value(enc_b_gpio_device, channel,&value_b);
	
	if(value_a == 0 && value_b == 0){
		flink_dio_set_value(enc_b_gpio_device, channel ,0x1);
	}else if(value_a == 0 && value_b == 1){
		flink_dio_set_value(enc_a_gpio_device, channel ,0x1);
	}else if(value_a == 1 && value_b == 1){
		flink_dio_set_value(enc_b_gpio_device, channel ,0x0);
	}else if(value_a == 1 && value_b == 0){
		flink_dio_set_value(enc_a_gpio_device, channel ,0x0);
	}
}


int testFQD(){
	//get enc_a io device	
	enc_a_gpio_device = flink_get_subdevice_by_unique_id(dev, ENC_A_GPIO_UNIQUE_ID);
	if(enc_a_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",ENC_A_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(enc_a_gpio_device) != GPIO_INTERFACE_ID) { 
		printf("out gpio device has wrong function id!\n");
		return -1;
	}

	if(flink_subdevice_get_subfunction(enc_a_gpio_device) != OUT_IO_SUBFUNCTION_ID) { //TODO get from include
		printf("gpio device enc a has wrong subfunction id!\n");
		return -1;
	}

	if(flink_subdevice_get_nofchannels(enc_a_gpio_device) != NUMBER_OF_FQD_CHANNELS) {
		printf("gpio device enc b has wrong number of channels!\n");
		return -1;
	}


	//get enc_b io device	
	enc_b_gpio_device = flink_get_subdevice_by_unique_id(dev, ENC_B_GPIO_UNIQUE_ID);
	if(enc_b_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",ENC_B_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(enc_b_gpio_device) != GPIO_INTERFACE_ID) { 
		printf("gpio device enc b has wrong function id!\n");
		return -1;
	}

	if(flink_subdevice_get_subfunction(enc_b_gpio_device) != OUT_IO_SUBFUNCTION_ID) { //TODO get from include
		printf("gpio device enc b has wrong subfunction id!\n");
		return -1;
	}

	if(flink_subdevice_get_nofchannels(enc_b_gpio_device) != NUMBER_OF_FQD_CHANNELS) {
		printf("gpio device enc b has wrong number of channels!\n");
		return -1;
	}

	for(int i = 0; i < NUMBER_OF_FQD_CHANNELS; i++){
		flink_dio_set_value(enc_a_gpio_device, i ,0x0);
		flink_dio_set_value(enc_b_gpio_device, i ,0x0);
	}



	//get fqd device
	flink_subdev* fqd_device = flink_get_subdevice_by_unique_id(dev, FQD_GPIO_UNIQUE_ID);
	if(fqd_device == NULL) {
		printf("No subdevice with unique id %d found.\n",FQD_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(fqd_device) != COUNTER_INTERFACE_ID) {
		printf("fqd device has wrong function id!\n");
		return -1;
	}

	if(flink_subdevice_get_nofchannels(fqd_device) != NUMBER_OF_FQD_CHANNELS) {
		printf("gpio device enc b has wrong number of channels!\n");
		return -1;
	}

	//reset device and test if the counter value resets to zero
	flink_subdevice_reset(fqd_device);
	uint32_t data = 0;
	int nrOfError = 0;
	for(int i = 0; i < NUMBER_OF_FQD_CHANNELS; i++){
		flink_counter_get_count(fqd_device,i,&data);
		if(data!=0){
			nrOfError++;			
		}
	}
	if(nrOfError != 0){
		printf("Error reseting device\n");
		return -1;
	}

	//start testing the fqd device
	//step forward
	nrOfError = 0;
	int numberOfSteps = 10000;
	for(int u = 0; u < numberOfSteps ; u++){
		for(int i = 0; i < NUMBER_OF_FQD_CHANNELS; i++){
			stepVorward(i);
			flink_counter_get_count(fqd_device,i,&data);
			if(data!=u+1){
				printf("data= %d!\n",data);
				nrOfError++;			
			}
		}
		usleep(100);
	}

	if(nrOfError != 0){
		printf("Error steping forward!\n");
		return -1;
	}
	//steb backward
	nrOfError = 0;
	for(int u = 0; u < numberOfSteps ; u++){
		for(int i = 0; i < NUMBER_OF_FQD_CHANNELS; i++){
			stepBackward(i);
			flink_counter_get_count(fqd_device,i,&data);
			if(data!= numberOfSteps-u-1){
				printf("data= %d!\n",data);
				nrOfError++;			
			}
		}
		usleep(100);
	}

	if(nrOfError != 0){
		printf("Error steping backwards!\n");
		return -1;
	}
	return 0;
}


int testGPIODevice(){
	int error;
	flink_subdev* out_gpio_device = flink_get_subdevice_by_unique_id(dev, OUT_GPIO_UNIQUE_ID);
	if(out_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",OUT_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(out_gpio_device) != GPIO_INTERFACE_ID) { 
		printf("out gpio device has wrong function id!\n");
		return -1;
	}


	flink_subdev* in_gpio_device = flink_get_subdevice_by_unique_id(dev, IN_GPIO_UNIQUE_ID);
	if(in_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",IN_GPIO_UNIQUE_ID);
		return -1;
	}

	if(flink_subdevice_get_function(in_gpio_device) != GPIO_INTERFACE_ID) { 
		printf("in gpio device has wrong function id!\n");
		return -1;
	}


	int number_of_channels = flink_subdevice_get_nofchannels(out_gpio_device);
	if(flink_subdevice_get_nofchannels(out_gpio_device) != number_of_channels){
		printf("gpio devices have not the same amount of channels!\n");
		return -1;
	}
	
	if(number_of_channels != NUMBER_OF_GPIO_CHANNELS_TEST){
		printf("gpio devices wrong number of channels!\n");
		return -1;
	}

	for(int i = 0; i < NUMBER_OF_GPIO_CHANNELS_TEST; i++){
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
	for(int i = 0; i < NUMBER_OF_GPIO_CHANNELS_TEST; i++){
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
	int channels[NUMBER_OF_GPIO_TESTS];
	int values[NUMBER_OF_GPIO_TESTS];

	srand(time(NULL));
	for(int i = 0; i< NUMBER_OF_GPIO_TESTS; i++){
		int r = rand();
		float f = r*1.0/RAND_MAX;
		channels[i] = (int) (f*NUMBER_OF_GPIO_CHANNELS_TEST);
		r = rand();
		f = r*1.0/RAND_MAX;
		if(f>0.5){
			values[i] = 1;			
		}else{
			values[i] = 0;	
		}
	}

	set_read_error = 0;
	for(int i = 0; i< NUMBER_OF_GPIO_TESTS; i++){
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
	for(int i = 0; i< NUMBER_OF_GPIO_TESTS;i++){
		values[i] = 0;
		channels[i] = 0;	
	}
	for(int i = 0; i + NUMBER_OF_GPIO_CHANNELS_TEST < NUMBER_OF_GPIO_TESTS; i = i + NUMBER_OF_GPIO_CHANNELS_TEST){
		int r = rand();
		float f = r*1.0/RAND_MAX;
		int firstchannel = (int) (f*NUMBER_OF_GPIO_CHANNELS_TEST);
		for (int u = 0; u < NUMBER_OF_GPIO_CHANNELS_TEST; u++){
			
			channels[i+u] =	(firstchannel + u) % NUMBER_OF_GPIO_CHANNELS_TEST;
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
	for(int i = 0; i + NUMBER_OF_GPIO_CHANNELS_TEST < NUMBER_OF_GPIO_TESTS; i = i + NUMBER_OF_GPIO_CHANNELS_TEST){
		for (int u = 0; u < NUMBER_OF_GPIO_CHANNELS_TEST; u++){
			flink_dio_set_value(out_gpio_device, channels[i+u], values[i+u]);
			
		}
		uint8_t result = 0;
		for (int u = 0; u < NUMBER_OF_GPIO_CHANNELS_TEST; u++){
			if(i+u<NUMBER_OF_GPIO_TESTS){
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


	return 0;

}
uint32_t frequency = 0;
flink_subdev* pwm_device;
flink_subdev* in_gpio_device;


int testPWMDevice(){
	//get pwm device	
	pwm_device = flink_get_subdevice_by_unique_id(dev, PWM_UNIQUE_ID);
	if(pwm_device == NULL) {
		printf("No subdevice with unique id %d found.\n",PWM_UNIQUE_ID);
		return -1;
	}
	
	if(flink_subdevice_get_function(pwm_device) != PWM_INTERFACE_ID) { 
		printf("pwm device has wrong function id!\n");
		return -1;
	}	

	if(flink_subdevice_get_nofchannels(pwm_device) != NUMBER_OF_PWM_CHANNELS) {
		printf("pwm device has wrong number of channels!\n");
		return -1;
	}
	if(flink_subdevice_reset(pwm_device) != 0){
		printf("reset pwm reset failed!\n");
		return -1;
	}

	if(flink_pwm_get_baseclock(pwm_device, &frequency) != 0){
		printf("get pwm base frequency failed!\n");
		return -1;
	}
	printf("pwm base frequency = %d\n",frequency);
	if(frequency <= 0){
		printf("pwm base frequency should be bigger than 0\n");
		return -1;
	}

	for (int i = 0; i< NUMBER_OF_PWM_CHANNELS; i++){
		
		if(flink_pwm_set_period(pwm_device,i,0) != 0){
			printf("set pwm period failed!\n");
			return -1;
		}
		if(flink_pwm_set_hightime(pwm_device,i,0) != 0){
			printf("set pwm hightime failed!\n");
			return -1;
		}
	}



	//get in io device	
	in_gpio_device = flink_get_subdevice_by_unique_id(dev, PWM_IN_GPIO_UNIQUE_ID );
	if(in_gpio_device == NULL) {
		printf("No subdevice with unique id %d found.\n",PWM_IN_GPIO_UNIQUE_ID );
		return -1;
	}

	if(flink_subdevice_get_function(in_gpio_device) != GPIO_INTERFACE_ID) { 
		printf("gpio device pwm in has wrong function id!\n");
		return -1;
	}

	if(flink_subdevice_get_subfunction(in_gpio_device) != IN_IO_SUBFUNCTION_ID) { //TODO get from include
		printf("gpio device in pwm has wrong subfunction id!\n");
		return -1;
	}

	if(flink_subdevice_get_nofchannels(in_gpio_device) != NUMBER_OF_PWM_CHANNELS) {
		printf("gpio device pwm has wrong number of channels!\n");
		return -1;
	}





	for (int i = 0 ; i < NUMBER_OF_PWM_RATIO_PERIOD_TEST; i++){
		int r = rand();
		float f = r*1.0/RAND_MAX;
		uint32_t period = 1 + (int) (f*999);
		r = rand();
		f = r*1.0/RAND_MAX;
		float ratio = 2.0 + f*97.5;
		r = rand();
		f = r*1.0/RAND_MAX;	
		int channel = (int) (f*NUMBER_OF_PWM_CHANNELS);

		//printf("Test pwm with period= %d, ratio= %f, channel= %d!\n",period,ratio,channel);
		if(testRatio(ratio,period,channel) != 0)return -1;
	}

	//reset device and test if the ratio and period register resets
	flink_subdevice_reset(pwm_device);
	
	for (int i = 0 ; i < NUMBER_OF_PWM_CHANNELS; i++){
		uint32_t period;
		uint32_t hightime;
		if(flink_pwm_get_period(pwm_device, i, &period) != 0){
			printf("error getting pwm period\n");
			return -1;
		}
		if(flink_pwm_get_hightime(pwm_device, i, &hightime) != 0){
			printf("error getting pwm hightime\n");
			return -1;
		};
		if(period != 0){
			printf("pwm wrong period value after reset!\n");
			return -1;
		}
		if(hightime != 0){
			printf("pwm wrong hightime value after reset!\n");
			return -1;
		}
	}


	return 0;
}


int testRatio(float ratio_desired,uint32_t desired_period, int channel){
	//set period with 50/50 ratio and measure it with polling
	float period_us = 1.0/desired_period*1000000;
	uint32_t period_reg = frequency/desired_period;
	uint32_t ratio_reg = (uint32_t) (period_reg*ratio_desired/100.0);
	
	for (int i = 0; i< NUMBER_OF_PWM_CHANNELS; i++){
		if(flink_pwm_set_period(pwm_device,i,period_reg) != 0){
			printf("set pwm period failed!\n");
			return -1;
		}
		if(flink_pwm_set_hightime(pwm_device,i,ratio_reg) != 0){
			printf("set pwm hightime failed!\n");
			return -1;
		}
	}

	struct timeval time;
	struct timeval old;
	struct timeval periodTime;
	uint8_t value;
	uint8_t oldvalue;
	bool running = true;
	uint8_t numberOfEdges = 0;
	int error = 0;
	int timeout = 0;
	gettimeofday(&old,NULL);
	flink_dio_get_value(in_gpio_device, channel, &oldvalue);
	while(running && timeout < PWM_TIMEOUT){
		timeout++;
		flink_dio_get_value(in_gpio_device, channel, &value);
			if(oldvalue != value){//edge detected
				numberOfEdges++;
				gettimeofday(&time,NULL);
				if(numberOfEdges > 1){
					unsigned long long deltaT= (time.tv_usec + 1000000 *time.tv_sec) -(old.tv_usec + 1000000 *old.tv_sec);
					float ratio = deltaT/period_us;
					if(ratio < ratio_desired + PWM_RATIO_THRESHOLD && ratio > ratio_desired - PWM_RATIO_THRESHOLD){
						printf("delta= %llu, ratio= %f/%f\n",deltaT,ratio,ratio_desired);
						error++;
					}	
				}
				if(numberOfEdges == 1){
					gettimeofday(&periodTime,NULL);				
				}
				if(numberOfEdges == 3){
					unsigned long long deltaT= (time.tv_usec + 1000000 *time.tv_sec) -(periodTime.tv_usec + 1000000 *periodTime.tv_sec);
					float period = 1.0/period_us*1000000;
					if(period < desired_period + PWM_PERIOD_THRESHOLD && period > desired_period - PWM_PERIOD_THRESHOLD){
						printf("delta= %llu, period= %f/%d\n",deltaT,period,desired_period);
						error++;
					}	
				}
				old = time;	
			}	
			oldvalue = value;
		
		running = false;
		for (int i = 0; i< NUMBER_OF_PWM_CHANNELS; i++){
			if(numberOfEdges <6){
				running = true;
			}
		}
		if(timeout >= PWM_TIMEOUT){
			printf("pwm period timeout ratio= %f,period= %d\n",ratio_desired,desired_period);
			return -1;
		}	
	}
	
	return error;
}



int testInfoDevice(flink_dev* dev,int unique_id, char* designDescriptor,int descriptorLength){
	char str[INFO_DESC_SIZE];
	char descriptor[descriptorLength + 1];

	flink_subdev* info_device = flink_get_subdevice_by_id(dev, 0); //Info device is allways at id zero
	if(flink_subdevice_get_function(info_device) != INFO_DEVICE_ID){
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
