#include "stm32f302x8.h"
#include "clock_configuration.h"
#include "dma_serial_manager.h"
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
 *Otherwise, it starts ADC conversions and loops endlessly. 
 */
void main(void)
{
        clock_setup();
        debug_setup();
        systick_setup();
        usart_1_setup();
        dma_serial_manager_setup();

        /*ADC should be configured before its driving timer.*/
        adc_setup();            
        adc_drive_timer_setup();
       
        if(adc_can_start())
        {
                // Get things going...
                adc_set_ADSTART();
        }

        // Loop endlessly. Timer, DMA and USART take care of everything else.
        while(1);
}
