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
 *  fLink userspace library, validation functions                  *
 *                                                                 *
 *******************************************************************/

#include "valid.h"

int validate_flink_dev(flink_dev* dev) {
	if(dev && dev->fd != 0) {
		return 1; // device struct valid
	}
	return 0;
}

int validate_flink_subdev(flink_subdev* subdev) {
	if(subdev->parent && subdev->nof_channels > 0) {
		return 1; // subdevice struct valid
	}
	return 0;
}
