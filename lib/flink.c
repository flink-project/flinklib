/*******************************************************************
 *   _________     _____      _____    ____  _____    ___  ____    *
 *  |_   ___  |  |_   _|     |_   _|  |_   \|_   _|  |_  ||_  _|   *
 *    | |_  \_|    | |         | |      |   \ | |      | |_/ /     *
 *    |  _|        | |   _     | |      | |\ \| |      |  __'.     *
 *   _| |_        _| |__/ |   _| |_    _| |_\   |_    _| |  \ \_   *
 *  |_____|      |________|  |_____|  |_____|\____|  |____||____|  *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *  fLink userspace library                                        *
 *                                                                 *
 *******************************************************************/
 
#include "flinklib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//#define DEBUG

// ############ Base operations ############

flink_t* flink_open(const char* file_name) {
	flink_t* dev = NULL;
	
	// Allocate memory for flink_t
	dev = malloc(sizeof(flink_t));
	if(dev == NULL) { // allocation failed
		libc_error();
		return NULL;
	}
	
	// Open device file
	dev->fd = open(file_name, O_RDWR);
	if(dev->fd < 0) { // failed to open device
		free(dev);
		libc_error();
		return NULL;
	}
	
	if(get_subdevices(dev) < 0) { // reading subdevices failed
		close(dev->fd);
		free(dev);
		return NULL;
	}
	
	return dev;
}

int flink_close(flink_t* dev) {
	subdevice_t* subdev = NULL;
	int i = 0;

	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	for(i = 0; i < dev->nof_subdevices; i++) { // for each subdevice
		subdev = dev->subdevices + i;
		// TODO free subdevice type specific data
	}
	
	if(dev->subdevices) {
		free(dev->subdevices);
	}
	close(dev->fd);
	free(dev);
	return EXIT_SUCCESS;
}

// ############ Low level operations ############

int flink_ioctl(flink_t* dev, ioctl_cmd_t cmd, void* arg) {
	int ret;
	
	#if defined(DEBUG)
		printf("[DEBUG] flink_ioctl '0x%x'\n", cmd);
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	ret = ioctl(dev->fd, cmd, arg);
	if(ret < 0) {
		libc_error();
	}
	
	return ret;
}

ssize_t flink_read(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t size, void* rdata) {
	int res = 0;
	ssize_t read_size = 0;
	
	// Check flink device structure
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_select_subdevice(dev, subdev, 0) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// seek offset
	if(lseek(dev->fd, offset, SEEK_SET) != offset) {
		libc_error();
		return EXIT_ERROR;
	}
	
	// read data from device
	read_size = read(dev->fd, rdata, size);
	if(read_size == -1) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return read_size;
}

ssize_t flink_write(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t size, void* wdata) {
	int res = 0;
	ssize_t write_size = 0;
	
	// Check flink device structure
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_select_subdevice(dev, subdev, 0) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// seek offset
	if(lseek(dev->fd, offset, SEEK_SET) != offset) {
		libc_error();
		return EXIT_ERROR;
	}
	
	// write data to device
	write_size = write(dev->fd, wdata, size);
	if(write_size == -1) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return write_size;
}

int flink_read_bit(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t bit, void* rdata) {
	ioctl_bit_container_t ioctl_arg;
	
	// Check flink device structure
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	if(rdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	ioctl_arg.offset = offset;
	ioctl_arg.bit    = bit;
	
	// Select subdevice
	if(flink_select_subdevice(dev, subdev, 0) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// read data from device
	if(flink_ioctl(dev, READ_SINGLE_BIT, &ioctl_arg) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	
	*((uint8_t*)rdata) = ioctl_arg.value;
	
	return EXIT_SUCCESS;
}

int flink_write_bit(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t bit, void* wdata) {
	ioctl_bit_container_t ioctl_arg;
	
	// Check flink device structure
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	if(wdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	ioctl_arg.offset = offset;
	ioctl_arg.bit    = bit;
	ioctl_arg.value  = *((uint8_t*)wdata);
	
	// Select subdevice
	if(flink_select_subdevice(dev, subdev, 0) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// write data to device
	if(flink_ioctl(dev, WRITE_SINGLE_BIT, &ioctl_arg) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}


// ############ Validation functions ############

uint8_t valid_dev(flink_t* dev) {
	if(dev != NULL && dev->fd != 0) {
		return 1;
	}
	return 0;
}

uint8_t valid_subdev(flink_t* dev, subdevice_t* subdev) {
	// TODO
	return 0;
}


// ############ Subdevice operations ############

// General
uint8_t flink_get_nof_subdevices(flink_t* dev) {
	uint8_t n = 0;
	
	#if defined(DEBUG)
		printf("[DEBUG] reading number of subdevices...\n");
	#endif
	
	if(flink_ioctl(dev, READ_NOF_SUBDEVICES, &n) < 0) {
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed!\n");
		#endif
		libc_error();
		return EXIT_ERROR;
	}
	#if defined(DEBUG)
		printf("[DEBUG]   --> %u\n", n);
	#endif
	return n;
}

int flink_subdevice_reset(flink_t* dev, uint8_t subdevice_id) {
	uint32_t offset;
	subdevice_t* subdev = NULL;
	uint8_t reset = 1;
	
	#if defined(DEBUG)
		printf("[DEBUG] Resetting counter...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	subdev = dev->subdevices + subdevice_id;
	
	// TODO check subdevice type
	
	offset = CONFIG_OFFSET;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_write_bit(dev, subdevice_id, offset, RESET_BIT, &reset)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

int flink_select_subdevice(flink_t* dev, uint8_t subdev_id, uint8_t exclusive) {
	ioctl_cmd_t cmd = SELECT_SUBDEVICE;
	if(exclusive) cmd = SELECT_SUBDEVICE_EXCL;
	
	if(flink_ioctl(dev, cmd, &subdev_id) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

// Digital in-/output
int flink_dio_set_direction(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t output) {
	subdevice_t* subdev = NULL;
	uint32_t offset;
	uint8_t bit;
	
	#if defined(DEBUG)
		printf("[DEBUG] Setting digital I/O direction...\n", channel, subdevice_id);
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	
	// TODO check subdevice type
	
	subdev = dev->subdevices + subdevice_id;
	offset = HEADER_SIZE + SUBHEADER_SIZE + channel / (REGISTER_WITH * 8);
	bit = channel % (REGISTER_WITH * 8);
	
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x\n", offset);
		printf("[DEBUG]   --> calculated bit is %u\n", bit);
	#endif
	
	if(flink_write_bit(dev, subdevice_id, offset, bit, &output)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

int flink_dio_set_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t value) {
	subdevice_t* subdev = NULL;
	uint32_t offset;
	uint8_t bit;
	uint8_t val = (value != 0);
	
	#if defined(DEBUG)
		printf("[DEBUG] Setting digital output value to %u...\n", val);
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	
	// TODO check subdevice type
	
	subdev = dev->subdevices + subdevice_id;
	offset = HEADER_SIZE + SUBHEADER_SIZE + subdev->nof_channels / (REGISTER_WITH * 8) + channel / (REGISTER_WITH * 8);
	if(subdev->nof_channels % (REGISTER_WITH * 8) != 0) offset += 4;
	bit = channel % (REGISTER_WITH * 8);
	
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x\n", offset);
		printf("[DEBUG]   --> calculated bit is %u\n", bit);
	#endif
	
	if(flink_write_bit(dev, subdevice_id, offset, bit, &val)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

int flink_dio_get_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t* value) {
	subdevice_t* subdev = NULL;
	uint32_t offset;
	uint8_t bit;
	
	#if defined(DEBUG)
		printf("[DEBUG] Reading digital input value...\n", channel, subdevice_id);
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	
	// TODO check subdevice type
	
	subdev = dev->subdevices + subdevice_id;
	offset = HEADER_SIZE + SUBHEADER_SIZE + subdev->nof_channels / (REGISTER_WITH * 8) + channel / (REGISTER_WITH * 8);
	if(subdev->nof_channels % (REGISTER_WITH * 8) != 0) offset += 4;
	bit = channel % (REGISTER_WITH * 8);
	
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x\n", offset);
		printf("[DEBUG]   --> calculated bit is %u\n", bit);
	#endif
	
	if(flink_read_bit(dev, subdevice_id, offset, bit, value)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

// PWM
int flink_pwm_get_baseclock(flink_t* dev, uint8_t subdevice_id, uint32_t* frequency) {
	uint32_t offset;
	
	#if defined(DEBUG)
		printf("[DEBUG] Reading base clock...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_read(dev, subdevice_id, offset, REGISTER_WITH, frequency) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

int flink_pwm_set_period(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t period) {
	uint32_t offset;
	
	#if defined(DEBUG)
		printf("[DEBUG] Setting PWM period...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + PWM_FIRSTPWM_OFFSET + REGISTER_WITH * channel;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_write(dev, subdevice_id, offset, REGISTER_WITH, &period) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

int flink_pwm_set_hightime(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t hightime) {
	uint32_t offset;
	subdevice_t* subdev = NULL;
	
	#if defined(DEBUG)
		printf("[DEBUG] Setting PWM hight time...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	subdev = dev->subdevices + subdevice_id;
	
	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + PWM_FIRSTPWM_OFFSET + subdev->nof_channels * REGISTER_WITH + REGISTER_WITH * channel;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_write( dev, subdevice_id, offset, REGISTER_WITH, &hightime) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

// Counter
int flink_counter_set_mode(flink_t* dev, uint8_t subdevice_id, uint8_t mode) {
	// TODO
	return EXIT_ERROR;
}

int flink_counter_get_count(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t* data) {
	uint32_t offset;
	subdevice_t* subdev = NULL;
	
	#if defined(DEBUG)
		printf("[DEBUG] Reading counter value...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	subdev = dev->subdevices + subdevice_id;
	
	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + REGISTER_WITH * channel;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_read(dev, subdevice_id, offset, REGISTER_WITH, data) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

//Analog Input
int flink_analog_in_get_resolution(flink_t* dev, uint8_t subdevice_id, uint32_t* resolution){
	uint32_t offset;
	
	#if defined(DEBUG)
		printf("[DEBUG] Reading resolution...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	

	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}

	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_read(dev, subdevice_id, offset, REGISTER_WITH, resolution) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}
int flink_analog_in_get_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t* value){
	uint32_t offset;
	subdevice_t* subdev = NULL;
	
	#if defined(DEBUG)
		printf("[DEBUG] Reading adc value...\n");
	#endif
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid device!\n");
		#endif
		return EXIT_ERROR;
	}
	
	if(subdevice_id >= dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		#if defined(DEBUG)
			printf("[DEBUG]   --> failed, invalid subdevice id!\n");
		#endif
		return EXIT_ERROR;
	}
	subdev = dev->subdevices + subdevice_id;
	
	// TODO check subdevice type
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + REGISTER_WITH * channel;
	#if defined(DEBUG)
		printf("[DEBUG]   --> calculated offset is 0x%x!\n", offset);
	#endif
	
	if(flink_read(dev, subdevice_id, offset, REGISTER_WITH, value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;

}






// ############ Internal stuff ############
int get_subdevices(flink_t* dev) {
	subdevice_t* subdev = NULL;
	int i = 0, ret = 0;
	
	if(!valid_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Read nof subdevices
	dev->nof_subdevices = flink_get_nof_subdevices(dev);
	
	// Allocate memory
	dev->subdevices = calloc(dev->nof_subdevices, sizeof(subdevice_t));
	if(dev->subdevices == NULL) {
		libc_error();
		dev->nof_subdevices = 0;
		return EXIT_ERROR;
	}
	
	// Fillup all information
	for(i = 0; i < dev->nof_subdevices; i++) {
		subdev = dev->subdevices + i;
		subdev->id = i;
		ret = flink_ioctl(dev, READ_SUBDEVICE_INFO, subdev);
	}
	
	return ret;
}
