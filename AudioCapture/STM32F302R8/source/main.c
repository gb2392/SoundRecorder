#include "stm32f302x8.h"
#include "clock_configuration.h"
#include "debug.h"
#include "time.h"
#include "serial.h"

void main(void)
{
        uint8_t test_vector[] = {0xab, 0xcd, 0xef, 0xde, 0xad, 0xbe, 0xef};

        clock_setup();
        debug_setup();
        systick_setup();
        usart_1_setup();

        while(1)
        {      
                if(is_usart_1_ready())
                {
                        usart_1_transmit((void *)test_vector, 7);
                }
        }
}
