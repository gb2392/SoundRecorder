#include "stm32f302x8.h"
#include "clock_configuration.h"
#include "debug.h"
#include "time.h"
#include "serial.h"
#include "adc.h"
#include <stdint.h>

/*
 *main - calls initialization routines and implements an infinite loop. 
 *
 *This function calls routines to configure:
 *      -the system clock source and peripheral bus frequencies
 *      -the SysTick hardware to produce interrupts on a 1 ms basis 
 *      -the USART 1 peripheral to be used for data transmission 
 *
 *Otherwise, it is being used for tests and development. 
 */
void main(void)
{
        const uint32_t COUNT_MAX = 500000;
        uint8_t test_vector[] = { 0xab, 0xcd, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xcf, 0x1A , 0x32 };
        uint32_t count = 0;

        clock_setup();
        debug_setup();
        systick_setup();
        usart_1_setup();
        adc_setup();

        while(1);

        while(1)
        {      
                if(is_usart_1_ready())
                {
                        usart_1_transmit((void *)test_vector, 10);
                        count++;
                }

                if(count == COUNT_MAX)
                {
                        break;
                }
        }
}
