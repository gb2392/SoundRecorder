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

* STM32 microcontroller is successfully transmitting data via serial peripheral (USART) using interrupt driven software. 
* STM32 microcontroller has a successful implementation of a 1 ms time base, using the SysTick hardware and corresponding handler. 
* Raspberry Pi has a roughed-in implementation of software which can listen for incoming data on the serial interface and write it to file. There are issues here. 