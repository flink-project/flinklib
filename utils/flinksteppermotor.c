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
 *              flink terminal command, subdevice function "stepperMotor"                  *
 *                                                                                         *
 *******************************************************************************************/
 
/** @file stepperMotor.c
 *  @brief flink terminal command, subdevice function "stepperMotor".
 *
 *  Provides the terminal function flinksteppermotor. To test 
 *  in a short and easy way the subdevice
 *
 *  @author Patrick Good
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include <flinklib.h>

#define EOPEN     -1
#define ESUBDEVID -2
#define EREAD     -3
#define EWRITE    -4
#define EPARAM    -5

// config reg
#define DIRECTION_BIT 0
#define FULL_STEP_BIT 1
#define TWO_PHASE_BIT 2
#define MODE_1_BIT 3
#define MODE_2_BIT 4
#define START_BIT 5
#define RES_COUNTER_BIT 6


#define DEFAULT_DEV "/dev/flink0"

struct BIT32_T {
    uint32_t Bit0_Direction:1;
    uint32_t Bit1_FullStep:1;
    uint32_t Bit2_TwoPhase:1;
    uint32_t Bit3_Mode1:1;
    uint32_t Bit4_Mode2:1;
    uint32_t Bit5_Start:1;
    uint32_t Bit6_ResCounter:1;
    uint32_t Bit7_InterruptEnable:1;
    uint32_t Bit8_InterruptClear:1;
    uint32_t Bit9:1;
    uint32_t Bit10:1;
    uint32_t Bit11:1;
    uint32_t Bit12:1;
    uint32_t Bit13:1;
    uint32_t Bit14:1;
    uint32_t Bit15:1;
    uint32_t Bit16:1;
    uint32_t Bit17:1;
    uint32_t Bit18:1;
    uint32_t Bit19:1;
    uint32_t Bit20:1;
    uint32_t Bit21:1;
    uint32_t Bit22:1;
    uint32_t Bit23:1;
    uint32_t Bit24:1;
    uint32_t Bit25:1;
    uint32_t Bit26:1;
    uint32_t Bit27:1;
    uint32_t Bit28:1;
    uint32_t Bit29:1;
    uint32_t Bit30:1;
    uint32_t Bit31:1;
};
struct BYTE32_T {
	uint8_t Byte0;
	uint8_t Byte1;
	uint8_t Byte2;
	uint8_t Byte3;
};
struct WORD2_T {
	uint16_t Word0;
	uint16_t Word1;
};
union BITBYTEWORD_DWORD_T{
	struct BIT32_T BitStructure;
	struct BYTE32_T ByteStructure;
	struct WORD2_T WordStructure;
	uint32_t DoubleWord;
};

int main(int argc, char* argv[]) {
	flink_dev*    dev;
	flink_subdev* subdev;
	uint16_t      function;
	char*         dev_name = DEFAULT_DEV;
	uint8_t       subdevice_id = 0;
	uint32_t      channel = 0;
	int           step_start_frequency = 1; // [steps/s]
	int           step_max_frequency = 10; // [steps/s]
	int           acceleration = 10; // [nof steps between start and stop]
	int           steps_to_do = 0; // [nof steps between start and stop]
	int			  time = 30; // sec
	union BITBYTEWORD_DWORD_T config; // config register
	union BITBYTEWORD_DWORD_T config_temp; // config register
	uint32_t	  prescaler_start;
	uint32_t	  prescaler_max;
	uint32_t	  acceleration_register;
	uint32_t      steps_have_done = 0;
	uint32_t      base_clk;
	bool          verbose = false;
	int           error = 0;

	// init default values to config
	config.DoubleWord = 0; // init all bits to 0;
	config.BitStructure.Bit0_Direction = 1;
	config.BitStructure.Bit1_FullStep = 1;
	config.BitStructure.Bit2_TwoPhase = 1;
	config.BitStructure.Bit3_Mode1 = 0;
	config.BitStructure.Bit4_Mode2 = 1;
	config.BitStructure.Bit5_Start = 0;

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
	while((c = getopt(argc, argv, "d:s:c:f:h:p:m:g:t:a:n:x:v")) != -1) {
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
			case 'f': // direction
				if (atoi(optarg) == 1 || atoi(optarg) == 0) {
				 	config.BitStructure.Bit0_Direction = atoi(optarg);
				} else {
					fprintf(stderr, "Only 0 or 1 allowed with option`\\x%x'.\n", optopt);
					return EPARAM;
				}
				break;
			case 'h': // stepmode
				if (atoi(optarg) == 1 || atoi(optarg) == 0) {
				 	config.BitStructure.Bit1_FullStep = atoi(optarg);
				} else {
					fprintf(stderr, "Only 0 or 1 allowed with option`\\x%x'.\n", optopt);
					return EPARAM;
				}
				break;
			case 'p': // phasemode
				if (atoi(optarg) == 1 || atoi(optarg) == 0) {
				 	config.BitStructure.Bit2_TwoPhase = atoi(optarg);
				} else {
					fprintf(stderr, "Only 0 or 1 allowed with option`\\x%x'.\n", optopt);
					return EPARAM;
				}
				break;
			case 'm': // runmode
				if (atoi(optarg) == 1) {
				 	config.BitStructure.Bit3_Mode1 = 0;
					config.BitStructure.Bit4_Mode2 = 1;
				} else if (atoi(optarg) == 0){
				 	config.BitStructure.Bit3_Mode1 = 1;
					config.BitStructure.Bit4_Mode2 = 0;
				} else {
					fprintf(stderr, "Only 0 or 1 allowed with option`\\x%x'.\n", optopt);
					return EPARAM;
				}
				break;
			case 'g': // start speed
				step_start_frequency = atoi(optarg);
				break;
			case 't': // top speed
				step_max_frequency = atoi(optarg);
				break;
			case 'a': // acceleration
				acceleration = atoi(optarg);
				break;
			case 'n': // steps
				steps_to_do = atoi(optarg);
				break;
			case 'x': // time
				time = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case '?':
				if(optopt == 'd' || optopt == 's' || optopt == 'c' || optopt == 'f' || optopt == 'h' || optopt == 'p'/
				   optopt == 'm' || optopt == 'g' || optopt == 't' || optopt == 'a' || optopt == 'n' || optopt == 'x')
				   fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return EPARAM;
			default:
				abort();
		}
	}
	// check step speeds
	if (step_max_frequency <= step_start_frequency) {
		fprintf(stderr, "Start speed has to be smaller or equal than top speed:  %d<=%d\n", step_start_frequency, step_max_frequency);
		return EOPEN;
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
	if(function != STEPPER_MOTOR_INTERFACE_ID) {
		fprintf(stderr, "Subdevice with id %d has wrong function, check subdevice id!\n", subdevice_id);
		return ESUBDEVID;
	}
	
	// Read the subdevice base clock
	error = flink_stepperMotor_get_baseclock(subdev, &base_clk);
	if(error != 0) {
		printf("Reading subdevice base clock failed!\n");
		return EREAD;
	}
	if(verbose) {
		printf("Subdevice base clock: %u Hz (%f MHz)\n", base_clk, (float)base_clk / 1000000.0);
	}

	// Calculate top speed
	prescaler_max = base_clk / step_max_frequency;
	if (prescaler_max == 0) prescaler_max = 1; // prescaler should not smaller than 1

	// Calculate start speed
	prescaler_start = base_clk / step_start_frequency;
	if (prescaler_start == 0) prescaler_max = 1; // prescaler should not smaller than 1

	// Calculate acceleration
	if (prescaler_max < prescaler_start) // avoid overflow by following subtraction
		acceleration_register = (uint32_t)ceil((prescaler_start - prescaler_max) / (double)acceleration);

	// clear stepcounter
	config_temp.DoubleWord = 0;
	config_temp.BitStructure.Bit6_ResCounter = 1;
	error = flink_stepperMotor_set_local_config_reg(subdev, channel, config_temp.DoubleWord);
	if(error != 0) {
		fprintf(stderr, "Failed to set local conf reg on channel %u at subdevice %u!\n", channel, subdevice_id);
		return EWRITE;
	}
    
	// set all regs 
	error = flink_stepperMotor_set_prescaler_start(subdev, channel, prescaler_start);
	error += flink_stepperMotor_set_prescaler_top(subdev, channel, prescaler_max);
	error += flink_stepperMotor_set_acceleration(subdev, channel, acceleration_register);
	error += flink_stepperMotor_set_steps_to_do(subdev, channel, steps_to_do);
	error += flink_stepperMotor_set_local_config_reg(subdev, channel, config.DoubleWord);
	if(error != 0) {
		fprintf(stderr, "Failed to set one or multiple conf registers on channel %u at subdevice %u!\n", channel, subdevice_id);
		return EWRITE;
	}
	if(verbose) {
		printf("Config register first 16 Bit: 0x%x \n", config.WordStructure.Word0);
	}
	// start the motor
	config.BitStructure.Bit5_Start = 1;
	error = flink_stepperMotor_set_local_config_reg(subdev, channel, config.DoubleWord);
	if(error != 0) {
		fprintf(stderr, "Failed to set local conf register on channel %u at subdevice %u!\n", channel, subdevice_id);
		return EWRITE;
	}

	
	// wait for a specific time
	sleep(time); 

	// if in freerunning, stop motor and wait till motor has stopped
	if(config.BitStructure.Bit3_Mode1 == 0 && config.BitStructure.Bit4_Mode2 == 1){
		config.BitStructure.Bit5_Start = 0;
		error = flink_stepperMotor_set_local_config_reg(subdev, channel, config.DoubleWord);
		if(error != 0) {
			fprintf(stderr, "Failed to set local conf register on channel %u at subdevice %u!\n", channel, subdevice_id);
			return EWRITE;
		}
		if(verbose) {
			printf("Motor is stopping");
		}
		// calculate wait time until motor has stopped
		time = (uint32_t)((((double)1)/step_max_frequency - ((double)1)/step_max_frequency) / 2 * acceleration * 1000 + 100);
		usleep(time * 1000);
	}

	error = flink_stepperMotor_get_steps_have_done(subdev, channel, &steps_have_done);
	if(error != 0) {
		fprintf(stderr, "Failed to read stapps have done on channel %u at subdevice %u!\n", channel, subdevice_id);
		return EWRITE;
	}
	printf("Steps that have the motor done: %u Stepps\n", steps_have_done);

	// Close flink device
	flink_close(dev);
	
    return EXIT_SUCCESS;
}
