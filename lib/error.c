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

#include "error.h"
#include <errno.h>
#include "log.h"
#include <string.h>

__thread int flink_errno = 0;

const char* flinklib_error_strings[] = {
	"No error",
	"Unknown error",
	"Not supported"
	"Invalid device",
	"Invalid subdevice",
	"Invalid channel",
	"Null ptr as argument",
	"Unknown ioctl command",
	"Wrong subdevice type",
};
#define NOF_ERRORS (sizeof(flinklib_error_strings) / sizeof(char*))


const char* flink_strerror(int e) {
	if(e < FLINK_NOERROR || e >= FLINK_NOERROR + NOF_ERRORS) { // not a flink error
		return strerror(e);
	}
	return flinklib_error_strings[e - FLINK_NOERROR];
}


void flink_perror(const char* p) {
	if(!p) p = "flinklib";
	fprintf(stderr, "%s: %s\n", p, flink_strerror(flink_errno));
}


void libc_error(void) {
	flink_errno = errno;
	if(PRINT_ERRORS_TO_STDERR){
		flink_perror("libc error");
	}
}


void flink_error(int e) {
	flink_errno = e;
	if(PRINT_ERRORS_TO_STDERR){
		flink_perror("flink error");
	}
}
