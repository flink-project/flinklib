#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <flinklib.h>

#define DEFAULT_DEV "/dev/flink0"


#define PWM_INTERFACE_ID 0xC
#define GPIO_INTERFACE_ID 0x5
#define COUNTER_INTERFACE_ID 0x6	
#define WD_INTERFACE_ID 0x10
#define PPWA_INTERFACE_ID 0xD
#define ANALOG_INPUT_INTERFACE_ID 0x1




void typeIdToString(uint16_t id, char * result){
	switch(id){
	case PWM_INTERFACE_ID:
		sprintf(result,"PWM");		
		break;
	case GPIO_INTERFACE_ID:
		sprintf(result,"GPIO");		
		break;
	case COUNTER_INTERFACE_ID:
		sprintf(result,"COUNTER");		
		break;
	case WD_INTERFACE_ID:
		sprintf(result,"WATCHDOG");		
		break;
	case PPWA_INTERFACE_ID:
		sprintf(result,"PPWA");		
		break;
	case ANALOG_INPUT_INTERFACE_ID:
		sprintf(result,"ANALOG INPUT");		
		break;
	default:
		sprintf(result,"%x",id);
	}

}

int main(int argc, char* argv[]) {
	flink_t* dev;
	char* dev_name = DEFAULT_DEV;
	char c;
	int i = 0;
	int def = 0;
	char id_string[200];
	
	/* Compute command line arguments */

	while((c = getopt(argc, argv, "d:")) != -1) {
		switch(c) {
			case 'd': // device
				dev_name = optarg;
				printf("Device: %s\n",optarg);
				break;
	
			case '?':
				if(optopt == 'd') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				// TODO this part should not be necessary??
				def = 1;
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				break;
		}
		if (def == 1){
			break;		
		}
	}
	
	dev = flink_open(dev_name);
	if(dev == NULL) {
		printf("Failed to open device!\n");
		return -1;
	}
	
	printf("Found %d Subdevices:\n", flink_get_nof_subdevices(dev));
	printf("\n");
	for(i = 0; i<flink_get_nof_subdevices(dev);i++){
		printf("ID: %d\n", dev->subdevices[i].id);
		typeIdToString(dev->subdevices[i].type_id,id_string);	
		printf("Type id: %s\n", id_string);
		printf("Sub type id: 0x%x\n", dev->subdevices[i].sub_type_id);
		printf("Interface version: %u\n", dev->subdevices[i].if_version);
		printf("Base address: 0x%x\n", dev->subdevices[i].base_addr);
		printf("Memory size: %u\n", dev->subdevices[i].mem_size);
		printf("Number of channels: %u\n", dev->subdevices[i].nof_channels);
		printf("\n");
	}


	flink_close(dev);
	
    return EXIT_SUCCESS;
}
