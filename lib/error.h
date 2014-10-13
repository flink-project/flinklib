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
 *  fLink userspace library, error handling                        *
 *                                                                 *
 *******************************************************************/

#ifndef FLINKLIB_ERROR_H_
#define FLINKLIB_ERROR_H_

// ############ Errorhandling ############

#define FLINK_NOERROR		0x2000					// No error
#define FLINK_EUNKNOWN		(FLINK_NOERROR  + 1)	// Unknown error
#define FLINK_ENOTSUPPORTED	(FLINK_NOERROR + 2)		// Not supported
#define FLINK_EINVALDEV		(FLINK_NOERROR + 3)		// Invalid device
#define FLINK_EINVALSUBDEV	(FLINK_NOERROR + 4)		// Invalid subdevice
#define FLINK_EINVALCHAN	(FLINK_NOERROR + 5)		// Invalid channel
#define FLINK_ENULLPTR		(FLINK_NOERROR + 6)		// Null ptr as argument
#define FLINK_UNKNOWNIOCTL	(FLINK_NOERROR + 7)		// Unknown ioctl command
#define FLINK_WRONGSUBDEVT	(FLINK_NOERROR + 8)		// Wrong subdevice type

const char* flink_strerror(int e);
void flink_perror(const char* p);
void libc_error(void);
void flink_error(int e);

#endif // FLINKLIB_ERROR_H_
