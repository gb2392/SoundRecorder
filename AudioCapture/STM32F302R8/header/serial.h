#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>

#define USART_1_STATE_NOT_CONFIGURED    0
#define USART_1_STATE_IDLE              1
#define USART_1_STATE_TX                2

/* 
 *ENABLE_INJECTION
 *
 *Enables injection of custom data just before the USART 1 transmit data  
 *register is written. The policies for injecting data are defined within the 
 *functions found in injector.c. A particular policy is selected by setting the
 *value of an INJECTOR_TYPE_X define to one (see below). The injected data
 *are transmitted in lieu of the data pointed to when making a call to the 
 *usart_1_transmit function. 
 */
#define ENABLE_INJECTION                (1)

/*
 *Note: only one type of data injection should be configured at any time. If
 *no injection type is enabled, injected data will always be zero in value. 
 *
 *INJECTION_TYPE_TEST
 *
 *Causes little endian (LSByte first) 16 bit integers (whose values increment 
 *by one until rolling over at 65535) to be transmitted in lieu of the actual 
 *data specified by call to function "usart_1_transmit". Useful for testing 
 *channel integrity. 
 *
 *INJECTION_TYPE_12_BIT_ENC 
 *
 *Encodes 12-bit data in a manner which includes a code that serves to 
 *associate the two bytes which contain each 12-bit data element so that 
 *if data are lost / missed during the transfer, the receiver can reconstitute
 *the received data in a manner that can be interpreted properly. 
 */
#define INJECTION_TYPE_TEST             (0)
#define INJECTION_TYPE_12_BIT_ENC       (1)

typedef struct
{
        volatile uint8_t state;
        uint32_t tx_length;
        volatile uint32_t tx_index;
        uint8_t * tx_data;
}USART_1_RUNTIME;

uint8_t is_usart_1_ready(void);
void usart_1_setup(void);
void usart_1_transmit(void * data, uint32_t length);

#endif