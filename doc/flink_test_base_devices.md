# Test Program for Basic Devices

## Hardware Setup 
To run this test you need a [MPC5200I-O Board](http://phytec.com/products/system-on-modules/phycore/mpc5200b-io/) or any other Processor which connects a Cyclone II over a local plus bus.

## VHDL Desing 
The first step to run this test is to compile and download the test design which can be found [here](https://github.com/flink-project/flinkvhdl/tree/master/axiComponents/testing/testdesigns/lpb/baseDevTesting)

The design contains the following devices: 

     ________________
    |                |
    |  Info Device   | 
    |________________|
    
     ________________
    |                |
    |  GPIO Device   | <------|
    |________________|        |
                              |
     ________________         |
    |                |        |
    |  GPIO Device   | -------|
    |________________|
    
     ________________
    |                | <----------|
    |  FQD Device    |            |
    |________________| <------|   |
                              |   |
     ________________         |   |
    |                |        |   |
    |  OUT IO Device | -------|   |
    |________________|            |
     ________________             |
    |                |            |
    |  OUT IO Device | -----------|
    |________________|          
    
     ________________
    |                | 
    |  PWM Device    | -------|         
    |________________|        |
                              |
     ________________         | 
    |                |        | 
    |  IN IO Device  | <------| 
    |________________|          

## Testcases
The following testcases are implemented in the test program:
-Test if the number of devices is right
-For every subdevice it reads the unique id and tests if it matchs to the coresponding function
-For every subdevice if there is a subfunction this will also be tested
-For every subdevice it tests the number of channels
-The info device descriptor register is rode 
-Reset the fqd device and test if the counter register resets to zero
-Bitbang the out_io devices to simulate an encoder. Than read the counter register and check if it has the right value. 
-Configure the two gpio devices to one only inputs and one only outputs. Than set the outputs and reade the inputs of the other device and check if the values match. The setting and reading is done in various orders.
-Set the period and the hightime of the pwm device to a random value and use the in_io device to measure the times.  

## Run the Test
The program can be run with the -d parameter to specify which flink device should be tested. For example: ./flink_test_base_devices  -d /dev/flink0 

## TODO
The following list shows the additional tests which should be implemented:
     -Add Watchdog Device to this test



