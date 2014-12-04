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
 *  fLink userspace library, ioctl definitions                     *
 *                                                                 *
 *******************************************************************/

/** @file flinkioctl.h
 *  @brief flink userspace library, ioctl definitions.
 *
 *  This header file contains definitions for ioctl calls.
 *
 *  @author Martin Züger
 */

#ifndef FLINKLIB_IOCTL_H_
#define FLINKLIB_IOCTL_H_

typedef enum {
	SELECT_SUBDEVICE 		= 0x10,
	SELECT_SUBDEVICE_EXCL	= 0x11,
	READ_NOF_SUBDEVICES		= 0x20,
	READ_SUBDEVICE_INFO		= 0x21,
	READ_SINGLE_BIT			= 0x30,
	WRITE_SINGLE_BIT		= 0x31
} ioctl_cmd_t;

typedef struct _ioctl_bit_container_t {
	uint32_t offset;		/// Offset of byte containing the single bit, taken from subdevice base address
	uint8_t  bit;			/// Bit number in byte
	uint8_t  value;			/// Value of the bit
} ioctl_bit_container_t;

#endif // FLINKLIB_IOCTL_H_
