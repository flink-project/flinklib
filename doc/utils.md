Helper utils for flink
======================

There are several small command line tools for working with a flink device.

lsflink
-------

Lists all subdevices of a flink device.

**Example:** `lsflink -d /dev/flink0 -v`

**Options:**

| Option  | Description         |
| ------- | ------------------- |
| -d file | specify device file |
| -v      | verbose mode        |


flinkinfo
-------

Prints the description string of the info subdevice.

**Example:** `flinkinfo -d /dev/flink0 -s 0`

**Options:**

| Option  | Description            |
| ------- | ---------------------- |
| -d file | specify device file    |
| -s id   | select subdevice by id |


flinkdio
--------

Control a flink digital I/O subdevice.

**Example:** `flinkdio -d /dev/flink0 -s 2 -c 0 -w -h`

**Options:**

| Option      | Description            |
| ----------- | ---------------------- |
| -d file     | specify device file    |
| -s id       | select subdevice by id |
| -c channel  | channel to use         |
| -r          | read from input        |
| -w          | write to output        |
| -h          | set output HIGH        |
| -l          | set output LOW         |


flinkpwm
--------

Control a flink PWM subdevice.

**Example:** `flinkpwm -d /dev/flink0 -s 3 -c 0 -f 1000 -h 50`

**Options:**

| Option        | Description            |
| ------------- | ---------------------- |
| -d file       | specify device file    |
| -s id         | select subdevice by id |
| -c channel    | channel to use         |
| -f frequency  | PWM frequency (in Hz)  |
| -h dutycycle  | PWM duty cycle in %    |
| -v            | verbose output         |


flinkppwa
---------

Read a flink PPWA subdevice.

**Example:** `flinkppwa -d /dev/flink0 -s 3 -c 0`

**Options:**

| Option        | Description            |
| ------------- | ---------------------- |
| -d file       | specify device file    |
| -s id         | select subdevice by id |
| -c channel    | channel to use         |
| -v            | verbose output         |


flinkcounter
------------

Read a flink counter subdevice.

**Example:** `flinkcounter -d /dev/flink0 -s 1 -c 0 -n 100 -t 100`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -c channel    | channel to use             |
| -n repeats    | number of repeats          |
| -t time       | time in ms between counter reads |


flinkanaloginput
------------

Read a flink analog input subdevice.

**Example:** `flinkanaloginput -d /dev/flink0 -s 4 -c 0`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -c channel    | channel to use             |
| -v            | verbose output             |


flinkanalogoutput
------------

Control a flink analog output subdevice.

**Example:** `flinkanalogoutput -d /dev/flink0 -s 4 -c 0 -o 38000`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -c channel    | channel to use             |
| -o value      | output value               |
| -v            | verbose output             |


flinkwatchdog
------------

Control a flink watchdog device. The watchdog is n times retriggered after 80% of the timeout is expired. 

**Example:** `flinkwd -d /dev/flink0 -s 2 -n 20 -t 200`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -n repeats    | number of repeats          |
| -t timeout    | watchdog timeout			     |
| -r            | real time                  |
| -v            | verbose output             |



flinksteppermotor
------------

Control a stepper motor.

**Defaults:**
File(/dev/flink0), Cannel(0), CW, Full Steps, Two Phase, Freerunning(30sec), Start Speed(10), Top Speed(100), Acceleration(10)

**Example:** `flinksteppermotor -s 1`

**Options:**

| Option          | Description                               |
| --------------- | ----------------------------------------- |
| -d file         | specify device file                       |
| -s id           | select subdevice by id                    |
| -c channel      | channel to use                            |
| -f direction    | direction: 1->CW 0->CCW                   |
| -h stepmode     | stepmode: 1->full 0->half                 |
| -p phasemode    | phasemode: 1->both 0->single              |
| -m runmode      | mode: 1->freerun 0->stepping              |
| -g start        | start speed [steps/s]                     |
| -t top          | top speed [steps/s]                       |
| -a acceleration | acceleration [stepps between start & top] |
| -n stepps       | Stepps to do in stepping mode             |
| -x time         | time for freerun mode                     |
| -v              | verbose output                            |


flinkreflectivesensoren
------------

Read a flink reflex light sensor subdevice.

**Example:** `flinkreflectivesensoren -d /dev/flink0 -s 4 -c 0`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -c channel    | channel to use             |
| -v            | verbose output             |


flinkinterrupthandler
------------

Provides IRQ sink. Print a string when an interrupt occurs.

**Example:** `flinkinterrupthandler -d /dev/flink0 -i 0`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -i IRQ        | System IRQ source Nr.      |
| -v            | verbose output             |


flinkinterruptmultiplexer
------------

Connects a flink IRQ signal coming from a flink device to a system IRQ.

**Example:** `flinkinterruptmultiplexer -d /dev/flink0 -s 4 -i 0 -f 10`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -i IRQ        | System IRQ source Nr.      |
| -f flink IRQ  | flink IRQ Nr.              |
| -v            | verbose output             |
