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
 *  fLink userspace library, subdevice types                       *
 *                                                                 *
 *******************************************************************/

#include <stdint.h>

const char* flink_subdev_type_strings[] = {
	"Info",                         // 0x00
	"Analog input",                 // 0x01
	"Analog output",                // 0x02
	"reserved",                     // 0x03
	"reserved",                     // 0x04
	"Digital I/O",                  // 0x05
	"Counter",                      // 0x06
	"Timer",                        // 0x07
	"Memory",                       // 0x08
	"reserved",                     // 0x09
	"reserved",                     // 0x0A
	"reserved",                     // 0x0B
	"PWM",                          // 0x0C
	"PPWA",                         // 0x0D
	"unknown",                      // 0x0E
	"Watch dog",                    // 0x10
	"unknown",                      // 0x11
	"unknown",                      // 0x12
	"unknown",                      // 0x13
	"unknown",                      // 0x14
	"unknown",                      // 0x15
	"unknown",                      // 0x16
	"unknown",                      // 0x17
	"unknown",                      // 0x18
	"unknown",                      // 0x19
};
#define NOF_KNOWNSUBDEVIDS (sizeof(flink_subdev_type_strings) / sizeof(char*))


const char* flink_subdevice_id2str(uint8_t id) {
	if(id > NOF_KNOWNSUBDEVIDS - 1) { // unknown subdevice id
		return "unknown";
	}
	return flink_subdev_type_strings[id];
}
