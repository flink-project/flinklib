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
 *  			ioctl definitions  	                   *
 *                                                                 *
 *******************************************************************/

/** @file flinkioctl_cmd.h
 *  @brief flink userspace library, ioctl comand definitions.
 *
 *  THIS FILE WAS CREATED AUTOMATICALLY
 *
 *  Createt with: flinklinux/shared_config/create_ioctl_files.sh
 *
 *  This header file contains definitions for ioctl calls.
 *
 *  @author Marcel Gehrig
 */
 

typedef enum {
	SELECT_SUBDEVICE			= 0x10,
	SELECT_SUBDEVICE_EXCL			= 0x11,
	READ_NOF_SUBDEVICES			= 0x20,
	READ_SUBDEVICE_INFO			= 0x21,
	READ_SINGLE_BIT			= 0x30,
	WRITE_SINGLE_BIT			= 0x31,
	SELECT_AND_READ_BIT			= 0x40,
	SELECT_AND_WRITE_BIT			= 0x41,
	SELECT_AND_READ			= 0x42,
	SELECT_AND_WRITE			= 0x43
} ioctl_cmd_t;
