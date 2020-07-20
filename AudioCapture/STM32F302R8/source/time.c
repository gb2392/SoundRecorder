#include "stm32f302x8.h"
#include "time.h"

void SysTick_Handler(void)
{

}

void systick_setup(void)
{
        // Set the reload value
        SysTick->LOAD = 0xffff;

        // Clear the current value
        SysTick->VAL = 0;

        // Program status and control register

        // Select the clock source for the SysTick timer

        // Enable the generation of an interrupt on roll-under

        // Enable the counter

}