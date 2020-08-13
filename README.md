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
** Has a successful implementation of a 1 ms time base, using the SysTick hardware and corresponding handler. 
** Is using a hardware timer to trigger ADC conversions, which the DMA then automatically stores in memory, absent intervention by the processor, except for when a buffer is filled and the DMA conversion complete interrupt service routine runs. 
*** Sample Rate : 10 kHz
*** Resolution : 12-bit
** In real time, as ADC conversions are taking place and buffered by the DMA, the microcontroller is transmitting available samples via USART 1 peripheral. 
*** Baud Rate : 576000 bits / second
*** Parity : None
*** Data Bits : 8
* Raspberry Pi has a roughed-in implementation of software which can listen for incoming data on the serial interface and write it to file. There are issues here. 
* Raspberry Pi 
** Software now configures the PL011 UART (serial perihperal) to listen for incoming data, which are then buffered. 
*** Because the platform is a Raspberry Pi Model 3 B+, it was necessary to remap the PL011 UART from Bluetooth module back to GPIO header pins. 
** User runs a console app which invokes configuration of UART and buffering effort. Entering 'q', then return while in the terminal ends the buffering, and causes the buffered data to be written to file. Program then exits. 
** Concern: data transmitted from the microcontroller don't always end up in the receive buffer on the Raspberry Pi. 
*** It is possible that the RPI OS "misses" incoming data because the data rate is too high, and the UART is visited often enough by the application to get available data. 
*** Appears to get worse with higher baud rates. 
*** Given current serial configuration (baud rate 576000), tests showed around seven dropped bytes for every million bytes transferred. Not too bad overall, but...
*** Because the data constitute little endian 2-byte samples, dropped data can mess with interpretation of received data later on. 
**** An approach to recover from the presence of dropped bytes in a received data is in development. See "decoder.c/h". 
