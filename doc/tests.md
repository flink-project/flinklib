#Test Programs for the flinklib

You can find two small test applications in directory 'test'.
- open_close: Opens a flink device file and closes it again. The program arguments allow for selecting the device file.
- read_write: Opens a flink device file. Selects a subdevice therein followed by a read or write. Program arguments specify the device, the subdevice id, the read or write offset and a value in case of write. It's up to the user to select meaningful parameter values.  
