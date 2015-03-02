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
	WRITE_SINGLE_BIT		= 0x31,
	SELECT_AND_READ_BIT		= 0x40,
	SELECT_AND_WRITE_BIT	= 0x41,
	SELECT_AND_READ			= 0x42,
	SELECT_AND_WRITE		= 0x43
} ioctl_cmd_t;

typedef struct _ioctl_bit_container_t {
	uint32_t offset;		/// Offset of byte containing the single bit, taken from subdevice base address
	uint8_t  bit;			/// Bit number in byte
	uint8_t  value;			/// Value of the bit
	uint8_t  subdevice;		/// subdevice to read from / write to
} ioctl_bit_container_t;

typedef struct _ioctl_container_t {
	uint8_t  subdevice;		/// subdevice to read from / write to
	uint32_t offset;		/// offset to base address of subdevice
	uint8_t  size;			/// size of data
	void*    data;			/// data to read/write
} ioctl_container_t;

#endif // FLINKLIB_IOCTL_H_
