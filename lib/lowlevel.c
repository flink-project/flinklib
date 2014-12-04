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
 *  fLink userspace library, low level operations                  *
 *                                                                 *
 *******************************************************************/
 
/** @file lowlevel.c
 *  @brief Contains low level operations for flink.
 *
 *  Contains functions to read from and write to the memory bank of
 *  flink devices directly. These functions can be used to communicate
 *  with user-defined flink subdevices.
 *
 *  @author Martin Züger
 *  @author Urs Graf
 */

#include "flinklib.h"
#include "flinkioctl.h"
#include "types.h"
#include "error.h"
#include "log.h"
#include "valid.h"

#include <sys/ioctl.h>
#include <unistd.h>


/**
 * @brief IOCTL operation for a flink device.
 * @param dev: Flink device handle.
 * @param cmd: IOCTL command.
 * @param arg: IOCTL arguments.
 * @return int: IOCTL return value or -1 in case of failure.
 */
int flink_ioctl(flink_dev* dev, int cmd, void* arg) {
	int ret;
	
	dbg_print("flink_ioctl '0x%x'\n", cmd);
	
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	ret = ioctl(dev->fd, cmd, arg);
	if(ret < 0) {
		libc_error();
	}
	
	return ret;
}


/**
 * @brief Read from a flink subdevice.
 * @param subdev: Subdevice to read from.
 * @param offset: Read offset, relative to the subdevice base address.
 * @param size: Nof bytes to read.
 * @param rdata: Pointer to a buffer where the read bytes are written to.
 * @return ssize_t: Nof bytes read from the subdevcie or a negative error code.
 */
ssize_t flink_read(flink_subdev* subdev, uint32_t offset, uint8_t size, void* rdata) {
	int res = 0;
	ssize_t read_size = 0;
	
	// Check data pointer
	if(rdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	// Check flink subdevice structure
	if(!validate_flink_subdev(subdev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_subdevice_select(subdev, NONEXCL_ACCESS) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// seek offset
	if(lseek(subdev->parent->fd, offset, SEEK_SET) != offset) {
		libc_error();
		return EXIT_ERROR;
	}
	
	// read data from device
	read_size = read(subdev->parent->fd, rdata, size);
	if(read_size == -1) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return read_size;
}


/**
 * @brief Write to a flink subdevice.
 * @param subdev: Subdevice to write to.
 * @param offset: Write offset, relative to the subdevice base address.
 * @param size: Nof bytes to write.
 * @param wdata: Data to write.
 * @return ssize_t: Nof bytes written or -1 in case of error.
 */
ssize_t flink_write(flink_subdev* subdev, uint32_t offset, uint8_t size, void* wdata) {
	int res = 0;
	ssize_t write_size = 0;
	
	// Check data pointer
	if(wdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	// Check flink subdevice structure
	if(!validate_flink_subdev(subdev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_subdevice_select(subdev, NONEXCL_ACCESS) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// seek offset
	if(lseek(subdev->parent->fd, offset, SEEK_SET) != offset) {
		libc_error();
		return EXIT_ERROR;
	}
	
	// write data to device
	write_size = write(subdev->parent->fd, wdata, size);
	if(write_size == -1) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return write_size;
}


/**
 * @brief Read a single bit from a flink subdevice
 * @param subdev: Subdevice to read from.
 * @param offset: Read offset, relative to the subdevice base address.
 * @param bit: Bit number to read in the byte given by offset.
 * @param rdata: Pointer to a buffer where the bit read should be written
 * @return int: 0 on succes, else -1.
 */
int flink_read_bit(flink_subdev* subdev, uint32_t offset, uint8_t bit, void* rdata) {
	ioctl_bit_container_t ioctl_arg;
	ioctl_arg.offset = offset;
	ioctl_arg.bit    = bit;
	
	// Check data pointer
	if(rdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	// Check flink subdevice structure
	if(!validate_flink_subdev(subdev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_subdevice_select(subdev, NONEXCL_ACCESS) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// read data from device
	if(flink_ioctl(subdev->parent, READ_SINGLE_BIT, &ioctl_arg) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	
	*((uint8_t*)rdata) = ioctl_arg.value;
	
	return EXIT_SUCCESS;
}


/**
 * @brief Write a single bit to a flink subdevice
 * @param subdev: Subdevice to write to.
 * @param offset: Write offset, relative to the subdevice base address.
 * @param bit: Bit number to write in the byte given by offset.
 * @param wdata: Bit to write, a value of nonzero sets the bit, 0 clears the bit
 * @return int
 */
int flink_write_bit(flink_subdev* subdev, uint32_t offset, uint8_t bit, void* wdata) {
	ioctl_bit_container_t ioctl_arg;
	ioctl_arg.offset = offset;
	ioctl_arg.bit    = bit;
	ioctl_arg.value  = *((uint8_t*)wdata);
	
	// Check data pointer
	if(wdata == NULL) {
		flink_error(FLINK_ENULLPTR);
		return EXIT_ERROR;
	}
	
	// Check flink subdevice structure
	if(!validate_flink_subdev(subdev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Select subdevice
	if(flink_subdevice_select(subdev, NONEXCL_ACCESS) < 0) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	// write data to device
	if(flink_ioctl(subdev->parent, WRITE_SINGLE_BIT, &ioctl_arg) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}
