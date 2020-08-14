# SoundRecorder

## Overview

An effort to create a system capable of capturing audio, and storing it to a file. 

## Major Components

* STMicroelectronics Nucleo-64 development board based around the STM32F302 microcontroller
* Raspberry Pi Model 3 B+ running Raspberry Pi OS
* Adafruit Industries electret microphone with pre-amplifier evaluation board

## Approach

Analog-to-digital conversion of the microphone / pre-amp output will be facilitated by the STM32 microcontroller. As conversions occur, the samples will be staged 
and subsequently sent in batches to the Raspberry Pi via the microcontroller serial port. The Raspberry Pi will receive and store the audio samples in a file. 


![Figure 1 : System Overview](Diagrams/setup.JPG?raw=true)


## Current State

* STM32 microcontroller
  * Has an implementation of a one millisecond time base, using the SysTick hardware and corresponding handler.
  * Is using a hardware timer to trigger ADC conversions, which the DMA then automatically stores in memory, absent intervention by the processor, except for when a buffer is filled and the DMA "transfer complete" ISR runs.
    * Sample Rate : 10 kHz
    * Sample Resolution : 12 bits
  * As ADC conversions are taking place and buffered by the DMA, the microcontroller is transmitting available samples via the USART 1 peripheral.
    * Baud Rate : 576000 bits / second
    * Parity : None
    * Data Bits : 8
* Raspberry Pi
  * Software configures the PL011 UART (serial peripheral) to listen for incoming data, which are then buffered.
    * Because the platform is a Raspberry Pi Model 3 B+, it was necessary to remap the PL011 UART from the Bluetooth module back to GPIO header pins.
    * The software is started from the terminal; the Raspberry Pi listens and buffers data until user enters 'q' and presses return, at which point, buffered data are written to a file and the program terminates.  
  * Concern: data transmitted from the microcontroller don't always end up in the receive buffer on the Raspberry Pi. 
    * It is possible that the system misses incoming data because the data rate is too high, and the UART is not visited often enough to read all available data before new data arrives.
    * Appears to get worse with higher baud rates.
    * Given current serial configuration (baud rate 576000), tests showed around seven dropped bytes for every million bytes transferred. For audio purposes, not too bad overall, but...
    * Because the data constitute little endian 2-byte samples, dropped data can mess with interpretation of received data later on. 
 
 
    ![Figure 2 : Dropped Data Issue](Diagrams/byte_drop_illustration.JPG?raw=true)
 
 
    * An approach was developed to recover from dropped bytes. Since each sample is only 12-bits, but is contained in 2-byte, 16-bit integers, there are four unused bits for every sample sent. By moving the sample bits in such a way as to split up the four unused bits between both the most significant and least significant bytes, it becomes possible to allocate two bits at the top of each byte. Within these bits are stored a number (0 - 3), which will be the same for both the most and least significant bytes. Let this number be called a "pair code". This pair code also increments by one every time a complete sample is sent, and of course, rolls over after three back to zero. Software on the receiving end (the Raspberry Pi in this case) then has a means of regaining traction with data downstream of lost bytes. For the code responsible for creating the encoding, see the "injector_12_bit_enc" function in [injector.c](AudioCapture/STM32F302R8/source/injector.c). For the code responsible for recovery and extraction of samples on the receiving end, see the "decoder" function in [decoder.c](SerialLog/RPI_M3_B_PLUS/source/decoder.c). 
    
    
    ![Figure 3 : Encoding of Sample Data and Pair Codes for Transmission](Diagrams/pair_coding.JPG?raw=true)


