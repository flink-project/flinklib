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


flinkcounter
------------

Control a flink counter subdevice.

**Example:** `flinkcounter -d /dev/flink0 -s 1 -c 0 -n 100 -t 100`

**Options:**

| Option        | Description                |
| ------------- | -------------------------- |
| -d file       | specify device file        |
| -s id         | select subdevice by id     |
| -c channel    | channel to use             |
| -n repeats    | number of repeats          |
| -t time       | time between counter reads |

