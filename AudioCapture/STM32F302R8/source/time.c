#include "stm32f302x8.h"
#include "time.h"
#include "debug.h"
#include "serial.h"
#include "adc.h"
#include <stdint.h>

static volatile uint32_t delay_down_counter_ms = 0;

/*
 *systick_setup - configures the SysTick hardware for use as a time base. 
 *
 *This function assumes an advanced high-performance bus (AHB) frequency
 *of 24 MHz, and accordingly attempts to configure the SysTick hardware to 
 *generate an interrupt every millisecond. The SysTick_Handler should run every
 *time the interrupt is generated, thereby providing a consistent time base 
 *for use elsewhere. 
 */
void systick_setup(void)
{
        // Set the reload value
        SysTick->LOAD = 23999;

        // Clear the current value
        SysTick->VAL = 0;

        // Program the status and control register

        // Set clock source for the SysTick timer as the AHB clock
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

        // Enable the generation of an interrupt on reaching zero
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
        
        // Enable the counter
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
 *SysTick_Handler - the handler for the SysTick interrupt (exception).
 *
 *This function should run when the SysTick hardware generates an interrupt.
 *Assuming an interrupt is being generated every millisecond, this function
 *should also run every millisecond. Keep user code within this function short.
 */
void SysTick_Handler(void)
{
        // Verify that the SysTick handler is running by toggling an output.
        TOGGLE_PC6;

        if(delay_down_counter_ms)
        {
                delay_down_counter_ms--;
        }
        
        if(adc_can_start())
        {
                //adc_set_ADSTART();
        }
}

/*
 *delay_ms - stops the program for the argued time. 
 *uint32_t delay_time_us        : the amount of time to wait, in milliseconds.
 */
void delay_ms(uint32_t delay_time_ms)
{
        delay_down_counter_ms = delay_time_ms;
        while(delay_down_counter_ms);
}