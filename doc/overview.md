#Overview
All the basic functionality can be found in the file `base.c`.  The necessary types and prototypes are located 
in `include/flinklib.h` and `include/flinkioctl.h`. The high-level API offers for each availabe function such as 
"digital io" or "analog input" a specific interface which is implemented in an own driver, e.g. `dio.c` or `ain.c`.
The low-level API is implemented in `lowlevel.c`. 

##Data Structures
The information for a specific flink device will be stored in a structure `flink_dev`.
```
// ############ flink device ############
struct flink_dev {
	int            fd;
	uint8_t        nof_subdevices;
	flink_subdev*  subdevices;
};
```
All subdevices which are present in a device will be inserted in a linked list 'subdevices'.

A subdevices is represented by the structure `flink_subdev`. 
```
// ############ flink subdevice ############
struct flink_subdevice {
	uint8_t        id;
	uint16_t       type_id;
	uint8_t        sub_type_id;
	uint8_t        if_version;
	uint32_t       base_addr;
	uint32_t       mem_size;
	uint32_t       nof_channels;
	flink_dev*     parent;
};
```
##Operations for flink devices
This operation allow for opening and closing flink devices.
```
flink_dev* flink_open(const char* file_name);
int        flink_close(flink_dev* dev);
```

##Operations for flink subdevices
This operation allow for the general handling of flink subdevices.
```
int           flink_get_nof_subdevices(flink_dev* dev);
flink_subdev* flink_get_subdevice_by_id(flink_dev* dev, uint8_t subdev_id);
uint8_t       flink_subdevice_get_id(flink_subdev* subdev);
uint16_t      flink_subdevice_get_function(flink_subdev* subdev);
uint8_t       flink_subdevice_get_subfunction(flink_subdev* subdev);
uint8_t       flink_subdevice_get_function_version(flink_subdev* subdev);
uint32_t      flink_subdevice_get_baseaddr(flink_subdev* subdev);
uint32_t      flink_subdevice_get_memsize(flink_subdev* subdev);
uint32_t      flink_subdevice_get_nofchannels(flink_subdev* subdev);
int           flink_subdevice_select(flink_subdev* subdev, uint8_t exclusive);
int           flink_subdevice_reset(flink_subdev* subdev);
const char*   flink_subdevice_id2str(uint16_t subdev_id);
```
Every subdevice implementing a specific function offers its own set of methods, see in the corresponding API.

##Low-level operations
With these methods its possible to communicate with a subdevice implementing a user-defined function.
```
int     flink_ioctl(flink_dev* dev, int cmd, void* arg);
ssize_t flink_read(flink_subdev* subdev, uint32_t offset, uint8_t size, void* rdata);
ssize_t flink_write(flink_subdev* subdev, uint32_t offset, uint8_t size, void* wdata);
int     flink_read_bit(flink_subdev* subdev, uint32_t offset, uint8_t bit, void* rdata);
int     flink_write_bit(flink_subdev* subdev, uint32_t offset, uint8_t bit, void* wdata);
```
