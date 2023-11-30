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
 *  flink userspace library, subdevice function "sensor input"     *
 *  subtype "reflective photoelectric sensor"                      *
 *                                                                 *
 *******************************************************************/
 
/** @file reflectiveSensor.c
 *  @brief flink userspace library, subdevice function "sensor input", subtype "reflective photoelectric sensor".
 *
 *  Contains the high-level functions for a flink subdevice
 *  which realizes the function "sensor input" and is used for 
 *  reflective photoelectric sensors of type TRCT1000.
 *  The subdevice is capable of generating interrupts when the 
 *  input signal is below a lower level or above an upper level.
 *
 *  @author Patrick Good
 */

#include "flinklib.h"
#include "types.h"
#include "error.h"
#include "log.h"

#include <stdint.h>

/**
 * @brief Gets the resolution of the subdevice
 * @param subdev: Subdevice.
 * @param resolution: Contains the resolution in number of resolvable steps.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_get_resolution(flink_subdev* subdev, uint32_t* resolution){
	uint32_t offset;
	offset = HEADER_SIZE + SUBHEADER_SIZE;
	
	if(flink_read(subdev, offset, REGISTER_WITH, resolution) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Reads an reflective sensor input channel
 * @param subdev: Subdevice containing the channel.
 * @param channel: Channel number.
 * @param value: Contains the digitized value of the channel input.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_get_value(flink_subdev* subdev, uint32_t channel, uint32_t* value){
	uint32_t offset;

	dbg_print("Get value of sensor input for channel %d on subdevice %d\n", subdev->id, channel);
	offset = HEADER_SIZE + SUBHEADER_SIZE + REFLECTIVE_SENSOR_FIRST_VALUE_OFFSET + channel*REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_read(subdev, offset, REGISTER_WITH, value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Sets the upper limit for interrupt generation.
 * @param subdev: Subdevice containing the channel.
 * @param channel: Channel number.
 * @param value: Contains the value for the upper bound.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_set_upper_level_int(flink_subdev* subdev, uint32_t channel, uint32_t value){
	uint32_t offset;

	dbg_print("Set value of sensor input for channel %d on subdevice %d\n", subdev->id, channel);
	offset = HEADER_SIZE + SUBHEADER_SIZE + REFLECTIVE_SENSOR_FIRST_VALUE_OFFSET + REGISTER_WITH*subdev->nof_channels;
	offset += channel*REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_write(subdev, offset, REGISTER_WITH, &value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Reads the upper level for interrupt generation.
 * @param subdev: Subdevice containing the channel.
 * @param channel: Channel number.
 * @param value: Contains the value for the upper bound.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_get_upper_level_int(flink_subdev* subdev, uint32_t channel, uint32_t* value){
	uint32_t offset;

	dbg_print("Get value of sensor input for channel %d on subdevice %d\n", subdev->id, channel);
	offset = HEADER_SIZE + SUBHEADER_SIZE + REFLECTIVE_SENSOR_FIRST_VALUE_OFFSET + REGISTER_WITH*subdev->nof_channels;
	offset += channel*REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_read(subdev, offset, REGISTER_WITH, value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Sets the lower limit for interrupt generation.
 * @param subdev: Subdevice containing the channel.
 * @param channel: Channel number.
 * @param value: Contains the value for the upper bound.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_set_lower_level_int(flink_subdev* subdev, uint32_t channel, uint32_t value){
	uint32_t offset;

	dbg_print("Set value of sensor input for channel %d on subdevice %d\n", subdev->id, channel);
	offset = HEADER_SIZE + SUBHEADER_SIZE + REFLECTIVE_SENSOR_FIRST_VALUE_OFFSET + 2*REGISTER_WITH*subdev->nof_channels;
	offset += channel*REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_write(subdev, offset, REGISTER_WITH, &value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Reads the lower level for interrupt generation.
 * @param subdev: Subdevice containing the channel.
 * @param channel: Channel number.
 * @param value: Contains the value for the upper bound.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_reflectivesensor_get_lower_level_int(flink_subdev* subdev, uint32_t channel, uint32_t* value){
	uint32_t offset;

	dbg_print("Get value of sensor input for channel %d on subdevice %d\n", subdev->id, channel);
	offset = HEADER_SIZE + SUBHEADER_SIZE + REFLECTIVE_SENSOR_FIRST_VALUE_OFFSET + 2*REGISTER_WITH*subdev->nof_channels;
	offset += channel*REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_read(subdev, offset, REGISTER_WITH, value) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}
