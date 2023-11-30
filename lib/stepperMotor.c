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
 *  flink userspace library, subdevice function "stepper motor"    *
 *                                                                 *
 *******************************************************************/
 
/** @file stepperMotor.c
 *  @brief flink userspace library, subdevice function "stepperMotor".
 *
 *  Contains the high-level functions for a flink subdevice
 *  which realizes the function "stepperMotor".
 *
 *  @author Patrick Good
 */

#include "flinklib.h"
#include "types.h"
#include "error.h"
#include "log.h"

#define LOCAL_CONF_OFFSET 0              //number of first register with one channel
#define LOCAL_CONF_SET_ATOMIC_OFFSET 1   //number to set bit(s) atomic with one channel
#define LOCAL_CONF_RESET_ATOMIC_OFFSET 2 //number to reset bit(s) atomic with one channel
#define PRESCALER_START_OFFSET 3         //number of first register with one channel
#define PRESCALER_TOP_OFFSET 4           //number of first register with one channel
#define ACCELERATION_OFFSET 5            //number of first register with one channel
#define STEPS_TO_DO_OFFSET 6             //number of first register with one channel
#define STEPS_DONE_OFFSET 7              //number of first register with one channel

// ========================================================================
//                          private functions
// ========================================================================

/**
 * private write function
 */
int flink_stepperMotor_set(flink_subdev* subdev, uint32_t channel, uint32_t register_offset, uint32_t data) {
	uint32_t offset_header;
	uint32_t offset_subdev;
	uint32_t offset;

	dbg_print(" --> Setting stepperMotor register for channel %d on subdevice %d\n", subdev->id, channel);
	
	offset_header = HEADER_SIZE + SUBHEADER_SIZE;
	offset_subdev = STEPPER_MOTOR_FIRST_CONF_OFFSET + subdev->nof_channels * REGISTER_WITH * register_offset+ REGISTER_WITH * channel;
	offset = offset_header + offset_subdev;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);

	if(flink_write(subdev, offset, REGISTER_WITH, &data) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}


/**
 * private read function
 */
int flink_stepperMotor_get(flink_subdev* subdev, uint32_t channel, uint32_t register_offset, uint32_t* data) {
	uint32_t offset_header;
	uint32_t offset_subdev;
	uint32_t offset;
		
	dbg_print("Reading period value from pwm %d of subdevice %d\n", channel, subdev->id);
	
	offset_header = HEADER_SIZE + SUBHEADER_SIZE;
	offset_subdev = STEPPER_MOTOR_FIRST_CONF_OFFSET + subdev->nof_channels * REGISTER_WITH * register_offset+ REGISTER_WITH * channel;
	offset = offset_header + offset_subdev;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_read(subdev, offset, REGISTER_WITH, data) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

// ========================================================================
//                          public functions
// ========================================================================


/**
 * @brief Reads the base clock of a stepper motor subdevice
 * @param subdev: Subdevice.
 * @param frequency: Contains the base clock in Hz.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_baseclock(flink_subdev* subdev, uint32_t* frequency) {
	uint32_t offset;

	dbg_print("Reading base clock from stepperMotor subdevice %d\n", subdev->id);
	
	offset = HEADER_SIZE + SUBHEADER_SIZE;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_read(subdev, offset, REGISTER_WITH, frequency) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Sets the stepperMotor local configuration
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param config: config register.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_local_config_reg(flink_subdev* subdev, uint32_t channel, uint32_t config) {
	dbg_print("Steppermotor local config reg:\n");
	return flink_stepperMotor_set(subdev, channel, LOCAL_CONF_OFFSET, config);
}

/**
 * @brief Gets the stepperMotor local configuration
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *config: config register
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_local_config_reg(flink_subdev* subdev, uint32_t channel, uint32_t* config) {
	dbg_print("Steppermotor local config reg:\n");
	return flink_stepperMotor_get(subdev, channel, LOCAL_CONF_OFFSET, config);
}

/**
 * @brief Sets bits in the local config reg atomic
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param config_set: config register bits to set
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_local_config_reg_bits_atomic(flink_subdev* subdev, uint32_t channel, uint32_t config_set) {
	dbg_print("Steppermotor local config reg set bits atomic:\n");
	return flink_stepperMotor_set(subdev, channel, LOCAL_CONF_SET_ATOMIC_OFFSET, config_set);
}

/**
 * @brief Resets bits in the local config reg atomic
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param config_reset: config register bits to reset
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_reset_local_config_reg_bits_atomic(flink_subdev* subdev, uint32_t channel, uint32_t config_reset) {
	dbg_print("Steppermotor local config reg reset bits atomic:\n");
	return flink_stepperMotor_set(subdev, channel, LOCAL_CONF_RESET_ATOMIC_OFFSET, config_reset);
}

/**
 * @brief Sets the stepperMotor start prescaler
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param prescaler: prescaler for base clock.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_prescaler_start(flink_subdev* subdev, uint32_t channel, uint32_t prescaler) {
	dbg_print("Steppermotor prescaler start:\n");
	return flink_stepperMotor_set(subdev, channel, PRESCALER_START_OFFSET, prescaler);
}

/**
 * @brief get the stepperMotor start prescaler
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *prescaler: prescaler for base clock.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_prescaler_start(flink_subdev* subdev, uint32_t channel, uint32_t* prescaler) {
	dbg_print("Steppermotor prescaler start:\n");
	return flink_stepperMotor_get(subdev, channel, PRESCALER_START_OFFSET, prescaler);
}

/**
 * @brief Sets the stepperMotor top speed prescaler
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param prescaler: prescaler for base clock..
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_prescaler_top(flink_subdev* subdev, uint32_t channel, uint32_t prescaler) {
	dbg_print("Steppermotor prescaler top:\n");
	return flink_stepperMotor_set(subdev, channel, PRESCALER_TOP_OFFSET, prescaler);
}

/**
 * @brief Gets the stepperMotor top speed prescaler
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *prescaler: prescaler for base clock.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_prescaler_top(flink_subdev* subdev, uint32_t channel, uint32_t* prescaler) {
	dbg_print("Steppermotor prescaler top:\n");
	return flink_stepperMotor_get(subdev, channel, PRESCALER_TOP_OFFSET, prescaler);
}

/**
 * @brief Sets the stepperMotor acceleration
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param acceleration: acceleration between start and top steed
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_acceleration(flink_subdev* subdev, uint32_t channel, uint32_t acceleration) {
	dbg_print("Steppermotor accelerator:\n");
	return flink_stepperMotor_set(subdev, channel, ACCELERATION_OFFSET, acceleration);
}

/**
 * @brief Gets the stepperMotor acceleration
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *acceleration: acceleration between start and top steed
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_acceleration(flink_subdev* subdev, uint32_t channel, uint32_t* acceleration) {
	dbg_print("Steppermotor accelerator:\n");
	return flink_stepperMotor_get(subdev, channel, ACCELERATION_OFFSET, acceleration);
}

/**
 * @brief Sets the steps of the stepper motor to be executed by the stepper motor
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param steps: Number of steps.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_set_steps_to_do(flink_subdev* subdev, uint32_t channel, uint32_t steps) {
	dbg_print("Steppermotor steps to do:\n");
	return flink_stepperMotor_set(subdev, channel, STEPS_TO_DO_OFFSET, steps);
}

/**
 * @brief Gets the steps of the stepper motor to be executed by the stepper motor
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *steps: Number of steps.
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_steps_to_do(flink_subdev* subdev, uint32_t channel, uint32_t* steps) {
	dbg_print("Steppermotor steps to do :\n");
	return flink_stepperMotor_get(subdev, channel, STEPS_TO_DO_OFFSET, steps);
}

/**
 * @brief Gets the steps of the stepper motor which were made
 * @param subdev: Subdevice.
 * @param channel: Channel number.
 * @param *example: example
 * @return int: 0 on success, -1 in case of failure.
 */
int flink_stepperMotor_get_steps_have_done(flink_subdev* subdev, uint32_t channel, uint32_t* steps) {
	dbg_print("Steppermotor stepps have done:\n");
	return flink_stepperMotor_get(subdev, channel, STEPS_DONE_OFFSET, steps);
}

/**
 * @brief Global reset step counter a flink steppermotor device.
 * @param subdev: Subdevice to reset stepps
 * @return int: 0 on success, else -1.
 */
int flink_steppermotor_global_step_reset(flink_subdev* subdev) {
	uint32_t offset = CONFIG_OFFSET;
	uint8_t reset = 1;
	
	if(flink_write_bit(subdev, offset, GLOBAL_STEP_RESET, &reset)) {
		libc_error();
		return EXIT_ERROR;
	}
	return EXIT_SUCCESS;
}
