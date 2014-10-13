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
 *  flink userspace library, subdevice type "watchdog"             *
 *                                                                 *
 *******************************************************************/
 
#include "flinklib.h"
#include "types.h"
#include "error.h"
#include "log.h"

int flink_wd_get_baseclock(flink_subdev* subdev, uint32_t* base_clk) {
	uint32_t offset;
	
	printf("Reading base clock from watchdog subdevice %d\n", subdev->id);
	
	offset = HEADER_SIZE + SUBHEADER_SIZE;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_read(subdev, offset, REGISTER_WITH, base_clk) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}


int flink_wd_get_status(flink_subdev* subdev, uint8_t* status){
	uint32_t offset;
	
	dbg_print("Reading WD status from subdevice %d\n", subdev->id);
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_read_bit(subdev, offset, 0, &status)) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}

int flink_wd_set_counter(flink_subdev* subdev, uint32_t counter){
	uint32_t offset;
	
	dbg_print("Setting WD counter on subdevice %d to %d (%x)\n", subdev->id, counter, counter);
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + 2 * REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_write(subdev, offset, REGISTER_WITH, &counter) != REGISTER_WITH) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}


int flink_wd_arm(flink_subdev* subdev){
	uint32_t offset;
	uint8_t arm = 1;

	printf("[DEBUG] Arming WD (subdevice %d)\n", subdev->id);
	
	offset = HEADER_SIZE + SUBHEADER_SIZE + REGISTER_WITH;
	dbg_print("  --> calculated offset is 0x%x!\n", offset);
	
	if(flink_write_bit(subdev, offset, 1, &arm)) {
		libc_error();
		return EXIT_ERROR;
	}
	
	return EXIT_SUCCESS;
}
