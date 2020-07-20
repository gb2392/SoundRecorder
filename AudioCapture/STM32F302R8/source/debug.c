#include "stm32f302x8.h"
#include "debug.h"

/*
 *debug_setup - configures hardware for use in debugging efforts. 
 *
 *Resource use:
 *      -PC6 as a push-pull output
 */
void debug_setup(void)
{
        // Enable the GPIOC peripheral clock
        RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

        // Set PC6 mode to general purpose output
        GPIOC->MODER &= ~GPIO_MODER_MODER6;
        GPIOC->MODER |= GPIO_MODER_MODER6_0;

        // Set PC6 output type to push-pull
        GPIOC->OTYPER &= ~GPIO_OTYPER_OT_6;

        // Set PC6 output to high-speed
        GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;

        // Disable PC6 pull up/down resistors
        GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR6;
}