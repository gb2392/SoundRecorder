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

## Current State

* STM32 microcontroller
  * Has an implementation of a one millisecond time base, using the SysTick hardware and corresponding handler.
  * Is using a hardware timer to trigger ADC conversions, which the DMA then automatically stores in memory, absent intervention by the processor, except for when a buffer is filled and the DMA "transfer complete" ISR runs.
    * Sample Rate : 10 kHz
    * Sample Resolution : 12 bits
  * As ADC conversions are taking place and are buffered by the DMA, the microcontroller is transmitting available samples via the USART 1 peripheral.
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
    * An approach to recover from the presence of dropped bytes in received data is in development. See "decoder.c/h". 
