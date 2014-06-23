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
 *  fLink userspace library                                        *
 *                                                                 *
 *******************************************************************/

#include "flinklib.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

__thread int flink_errno = 0;
uint8_t loglevel = LOGERROR;

char* flinklib_error_strings[] = {
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
	if(flink_loglevel > LOGNOTHING){
		flink_perror("libc error");
	}
}

void flink_error(int e) {
	flink_errno = e;
	if(flink_loglevel > LOGNOTHING){
		flink_perror("flink error");
	}
}

