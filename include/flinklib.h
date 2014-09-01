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
 *  fLink userspace library, main include file                     *
 *                                                                 *
 *******************************************************************/

#ifndef FLINKLIB_H_
#define FLINKLIB_H_


#include <stdint.h>
#include <sys/types.h>
#include "flinkioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

// ############ Device structs ############

typedef struct _subdevice_t {
	uint8_t              id;
	uint16_t             type_id;
	uint8_t              sub_type_id;
	uint8_t              if_version;
	uint32_t             base_addr;
	uint32_t             mem_size;
	uint32_t             nof_channels;
//	void*                subheaderdata;
} subdevice_t;

typedef struct _flink_t {
	int                  fd;
	uint8_t              nof_subdevices;
	struct _subdevice_t* subdevices;
} flink_t;

// ############ Base operations ############

flink_t* flink_open(const char* file_name);
int flink_close(flink_t* dev);

// ############ Low level operations ############

int flink_ioctl(flink_t* dev, ioctl_cmd_t cmd, void* arg);
ssize_t flink_read(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t size, void* rdata);
ssize_t flink_write(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t size, void* wdata);
int flink_read_bit(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t bit, void* rdata);
int flink_write_bit(flink_t* dev, uint8_t subdev, uint32_t offset, uint8_t bit, void* wdata);

// ############ Validation functions ############

uint8_t valid_dev(flink_t* dev);
uint8_t valid_subdev(flink_t* dev, subdevice_t* subdev);

// ############ Subdevice operations ############

#define REGISTER_WITH			4		// byte
#define HEADER_SIZE			16		// byte
#define SUBHEADER_SIZE			16		// byte
#define STATUS_OFFSET			0x0010	// byte
#define CONFIG_OFFSET			0x0014	// byte
#define PWM_BASECLK_OFFSET		0x0000	// byte
#define PWM_FIRSTPWM_OFFSET		0x0004	// byte
#define ANALOG_INPUT_FIRST_VALUE_OFFSET	0x0004	// byte
#define WD_FIRST_COUNTER_OFFSET		0x0004	// byte
#define RESET_BIT			0

// General
uint8_t flink_get_nof_subdevices(flink_t* dev);
int flink_select_subdevice(flink_t* dev, uint8_t subdev_id, uint8_t exclusive);
int flink_subdevice_reset(flink_t* dev, uint8_t subdevice_id);

// Analog input
int flink_analog_in_get_resolution(flink_t* dev, uint8_t subdevice_id, uint32_t* resolution);
int flink_analog_in_get_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t* value);

// Analog output
// TODO

// Digital in-/output
#define FLINK_OUTPUT 1
#define FLINK_INPUT 0
int flink_dio_set_direction(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t output);
int flink_dio_set_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t high);
int flink_dio_get_value(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint8_t* value);

// Counter
int flink_counter_set_mode(flink_t* dev, uint8_t subdevice_id, uint8_t mode);
int flink_counter_get_count(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t* data);

// PWM
int flink_pwm_get_baseclock(flink_t* dev, uint8_t subdevice_id, uint32_t* frequency);
int flink_pwm_set_period(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t period);
int flink_pwm_set_hightime(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t hightime);

// Watchdog
int flink_wd_get_baseclock(flink_t* dev, uint8_t subdevice_id, uint32_t* base_clk);
int flink_wd_set_counter_reg(flink_t* dev, uint8_t subdevice_id, uint32_t channel, uint32_t value);
int flink_wd_reset_chanel(flink_t* dev, uint8_t subdevice_id, uint32_t channel);
int flink_wd_set_clk_pol(flink_t* dev, uint8_t subdevice_id, uint32_t channel,uint8_t clk_pol);

// ############ Logging/Errorhandling ############

#define LOGNOTHING	0
#define LOGERROR	1
#define LOGWARNING	2
#define LOGINFO		3
#define LOGDEBUG	4

#define FLINK_NOERROR		0x2000					// No error
#define FLINK_EUNKNOWN		(FLINK_NOERROR  + 1)	// Unknown error
#define FLINK_ENOTSUPPORTED	(FLINK_NOERROR + 2)		// Not supported
#define FLINK_EINVALDEV		(FLINK_NOERROR + 3)		// Invalid device
#define FLINK_EINVALSUBDEV	(FLINK_NOERROR + 4)		// Invalid subdevice
#define FLINK_EINVALCHAN	(FLINK_NOERROR + 5)		// Invalid channel
#define FLINK_ENULLPTR		(FLINK_NOERROR + 6)		// Null ptr as argument
#define FLINK_UNKNOWNIOCTL	(FLINK_NOERROR + 7)		// Unknown ioctl command
#define FLINK_WRONGSUBDEVT	(FLINK_NOERROR + 8)		// Wrong subdevice type

#define EXIT_SUCCESS	0
#define EXIT_ERROR		-1

extern __thread int flink_errno;

int flink_loglevel(int loglevel);
void flink_perror(const char* s);
const char* flink_strerror(int errno);
void flink_error(int errno);

// ############ Internal stuff ############

void libc_error(void);
int get_subdevices(flink_t* dev);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif // FLINKLIB_H_
