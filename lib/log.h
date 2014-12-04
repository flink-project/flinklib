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
 *  fLink userspace library, logging                               *
 *                                                                 *
 *******************************************************************/

/** @file log.h
 *  @brief Debug utilities.
 *
 *  @author Martin Züger
 */

#ifndef FLINKLIB_LOGGING_H_
#define FLINKLIB_LOGGING_H_

#include <stdio.h>

#define PRINT_ERRORS_TO_STDERR 1
#define DEBUG 0

#define dbg_print(fmt, ...)      do { if(DEBUG) { printf("[flinklib] DEBUG: "); printf(fmt, ##__VA_ARGS__); } } while(0)

#endif // FLINKLIB_LOGGING_H_
