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
 *  fLink userspace library, base functionality                    *
 *                                                                 *
 *******************************************************************/
 
/** @file base.c
 *  @brief Contains the base functionality for flink.
 *
 *  Contains functions to initialize, add and remove flink devices
 *  and subdevices
 *
 *  @author Martin Züger
 *  @author Urs Graf
 */

#include "flinklib.h"
#include "flinkioctl.h"
#include "types.h"
#include "valid.h"
#include "error.h"
#include "log.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


/*******************************************************************
 *                                                                 *
 *  Internal (private) methods                                     *
 *                                                                 *
 *******************************************************************/

/**
 * @brief Read number of subdevices from flink device.
 * 
 * @param dev: Device to read
 * @return int: Number of flink devices or -1 in case of error.
 */
static int read_nof_subdevices(flink_dev* dev) {
	uint8_t n = 0;
	
	dbg_print("reading number of subdevices...\n");
	
	if(flink_ioctl(dev, READ_NOF_SUBDEVICES, &n) < 0) {
		dbg_print("   --> failed!\n");
		libc_error();
		return EXIT_ERROR;
	}
	dbg_print("  --> %u\n", n);
	return n;
}

/**
 * @brief Read header of all subdevices and update flink device.
 * 
 * @param dev: flink device to update
 * @return int: number of subdevices read, or -1 in case of error.
 */
static int get_subdevices(flink_dev* dev) {
	flink_subdev* subdev = NULL;
	int i = 0, ret = 0;
	
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	// Read nof subdevices
	dev->nof_subdevices = read_nof_subdevices(dev);
	
	// Allocate memory
	dev->subdevices = calloc(dev->nof_subdevices, sizeof(flink_subdev));
	if(dev->subdevices == NULL) { // allocation failed
		libc_error();
		dev->nof_subdevices = 0;
		return EXIT_ERROR;
	}
	
	// Fillup all information
	for(i = 0; i < dev->nof_subdevices; i++) { // for each subdevice
		subdev = dev->subdevices + i;
		subdev->id = i;
		ret = flink_ioctl(dev, READ_SUBDEVICE_INFO, subdev);
		if (ret < 0) return ret;
		subdev->parent = dev;
	}
	
	return i;
}


/*******************************************************************
 *                                                                 *
 *  Public methods                                                 *
 *                                                                 *
 *******************************************************************/

/**
 * @brief Opens a flink device file
 * @param file_name: Device file (null terminated array).
 * @return flink_dev*: Pointer to the opened flink device or NULL in case of error.
 */
flink_dev* flink_open(const char* file_name) {
	flink_dev* dev = NULL;
	
	// Allocate memory for flink_t
	dev = malloc(sizeof(flink_dev));
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


/**
 * @brief Close an open flink device
 * @param dev: device to close.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_close(flink_dev* dev) {
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	if(dev->subdevices) {
		free(dev->subdevices);
	}
	
	close(dev->fd);
	free(dev);
	return EXIT_SUCCESS;
}


/**
 * @brief Returns the number of subdevices of a fink device.
 * @param dev: Device to read
 * @return int: Number of flink devices or -1 in case of error.
 */
int flink_get_nof_subdevices(flink_dev* dev) {
	
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return EXIT_ERROR;
	}
	
	return dev->nof_subdevices;
}


/**
 * @brief Reset a flink subdevice.
 * @param subdev: Subdevice to reset
 * @return int: 0 on success, else -1.
 */
int flink_subdevice_reset(flink_subdev* subdev) {
	uint32_t offset = CONFIG_OFFSET;
	uint8_t reset = 1;
	
	if(!validate_flink_subdev(subdev)) {
		flink_error(FLINK_EINVALSUBDEV);
		return EXIT_ERROR;
	}
	
	if(flink_write_bit(subdev, offset, RESET_BIT, &reset)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}


/**
 * @brief Select a flink subdevice for further operations.
 * @param subdev: Subdevice to select.
 * @param exclusive: Block access to this subdevice for other processes.
 * @return int: 0 on success, else -1.
 */
int flink_subdevice_select(flink_subdev* subdev, uint8_t exclusive) {
	ioctl_cmd_t cmd = SELECT_SUBDEVICE;
	if(exclusive) cmd = SELECT_SUBDEVICE_EXCL;
	
	if(flink_ioctl(subdev->parent, cmd, &(subdev->id)) < 0) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Find subdevice of a device with a given id.
 * @param dev: Device to search.
 * @param subdev_id: Id of subdevice.
 * @return flink_subdev*: Pointer to the subdevice or NULL in case of error.
 */
flink_subdev* flink_get_subdevice_by_id(flink_dev* dev, uint8_t subdev_id) {
	
	// Check flink device structure
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return NULL;
	}

	// Check subdevice id
	if(subdev_id > dev->nof_subdevices) {
		flink_error(FLINK_EINVALSUBDEV);
		return NULL;
	}
	
	return dev->subdevices + subdev_id;
}

/**
 * @brief Find subdevice of a device with a given unique id.
 * @param dev: Device to search.
 * @param unique_id: Unique id of subdevice.
 * @return flink_subdev*: Pointer to the subdevice or NULL in case of error.
 */
flink_subdev* flink_get_subdevice_by_unique_id(flink_dev* dev, uint32_t unique_id) {
	flink_subdev* subdev = dev->subdevices;

	// Check flink device structure
	if(!validate_flink_dev(dev)) {
		flink_error(FLINK_EINVALDEV);
		return NULL;
	}

	uint8_t nof = 0;
	while(nof++ < dev->nof_subdevices) {
		if(subdev->unique_id == unique_id) {
			return subdev;
		}
		subdev = subdev + 1;
	}
	return NULL;
}

/**
 * @brief Get the id of a subdevice.
 * @param subdev: The subdevice.
 * @return uint8_t: Id of the subdevice.
 */
uint8_t flink_subdevice_get_id(flink_subdev* subdev) {
	return subdev->id;
}

/**
 * @brief Get the function of a subdevice.
 * @param subdev: The subdevice.
 * @return uint16_t: Returns the function id.
 */
uint16_t flink_subdevice_get_function(flink_subdev* subdev) {
	return subdev->function_id;
}

/**
 * @brief Get the subfunction of a subdevice.
 * @param subdev: The subdevice.
 * @return uint8_t: Returns the subfunction id.
 */
uint8_t flink_subdevice_get_subfunction(flink_subdev* subdev) {
	return subdev->sub_function_id;
}

/**
 * @brief Get the function version of a subdevice.
 * @param subdev: The subdevice.
 * @return uint8_t: Returns the function version.
 */
uint8_t flink_subdevice_get_function_version(flink_subdev* subdev) {
	return subdev->function_version;
}

/**
 * @brief Get the base address of a subdevice.
 * @param subdev: The subdevice.
 * @return uint32_t: Returns the base address.
 */
uint32_t flink_subdevice_get_baseaddr(flink_subdev* subdev) {
	return subdev->base_addr;
}

/**
 * @brief Get the memory size of a subdevice.
 * @param subdev: The subdevice.
 * @return uint32_t: Returns the memory size.
 */
uint32_t flink_subdevice_get_memsize(flink_subdev* subdev) {
	return subdev->mem_size;
}

/**
 * @brief Get the number of channels of a subdevice.
 * @param subdev: The subdevice.
 * @return uint32_t: Returns the number of channels.
 */
uint32_t flink_subdevice_get_nofchannels(flink_subdev* subdev) {
	return subdev->nof_channels;
}

/**
 * @brief Get the unique id of a subdevice.
 * @param subdev: The subdevice.
 * @return uint32_t: Returns the unique id.
 */
uint32_t flink_subdevice_get_unique_id(flink_subdev* subdev) {
	return subdev->unique_id;
}
