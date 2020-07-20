#include "stm32f302x8.h"
#include "serial.h"
#include <stdint.h>

void usart_1_setup(void)
{
        // Set the word length 

        // Set the baud rate

        // Set the number of stop bits

        // Enable the usart

        // Set the TE bit to send an idle frame

        // Write the data register

        // Wait until 

        // Tx config

}

void usart_1_transmit(void * data, uint32_t length)
{
        // Set the TXEIE bit to enable interrupts
}

void USART1_IRQHandler(void)
{

}