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
    |  OUT IO Device | <------| 
    |________________|          

