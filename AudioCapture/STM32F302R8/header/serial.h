#ifndef SERIAL_H
#define SERIAL_H

#define USART_1_STATE_NOT_CONFIGURED    0
#define USART_1_STATE_IDLE              1
#define USART_1_STATE_TX                2

typedef struct
{
        volatile uint8_t state;
        uint16_t tx_length;
        volatile uint16_t tx_index;
        uint8_t * tx_data;
}USART_1_RUNTIME;

uint8_t is_usart_1_ready(void);
void usart_1_setup(void);
void usart_1_transmit(void * data, uint32_t length);

#endif