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
 *  flink userspace library, types                                 *
 *                                                                 *
 *******************************************************************/

#ifndef FLINKLIB_TYPES_H_
#define FLINKLIB_TYPES_H_

#include "stdint.h"
#include "flinklib.h"

struct _flink_dev {
	int            fd;
	uint8_t        nof_subdevices;
	flink_subdev*  subdevices;
};

struct _flink_subdev {
	uint8_t        id;
	uint16_t       type_id;
	uint8_t        sub_type_id;
	uint8_t        if_version;
	uint32_t       base_addr;
	uint32_t       mem_size;
	uint32_t       nof_channels;
	flink_dev*     parent;
};

#endif // FLINKLIB_TYPES_H_
