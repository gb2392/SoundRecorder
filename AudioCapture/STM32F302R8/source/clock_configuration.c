#include "stm32f302x8.h"
#include "clock_configuration.h"

/*
 *clock_setup - configures system and peripheral clock settings.
 *
 *This function assumes an external clock signal with a frequency of 8 MHz is
 *being delivered to the OSC_IN input of the high speed external (HSE) 
 *oscillator. Based on this assumption, this function attempts to configure the
 *controller for the following:
 *      -system clock (SYSCLK) frequency: 24 MHz
 *      -advanced high-performance bus (AHB) frequency: 24 MHz
 *      -advanced peripheral bus 1 (APB1) frequency: 24 MHz
 *      -advanced peripheral bus 2 (APB2) frequency: 24 MHz
 *      -wait states for flash access: 0
 *      -SYSCLK presented on MCO/PA8 for verification
 */
void clock_setup(void)
{
        // Set the AHB prescale value to 1
        RCC->CFGR &= ~RCC_CFGR_HPRE;

        // Set the APB1 prescale value to 1
        RCC->CFGR &= ~RCC_CFGR_PPRE1;
        
        // Set the APB2 prescale value to 1
        RCC->CFGR &= ~RCC_CFGR_PPRE2;

        // Turn on the HSE
        RCC->CR |= RCC_CR_HSEON;
        
        // Wait until HSE is ready
        while((RCC->CR & RCC_CR_HSERDY) == 0);
        
        // Set the PLL pre-division factor to 1
        RCC->CFGR2 &= ~RCC_CFGR2_PREDIV;
        
        // Set PLL Source (PLLSRC) to HSE
        RCC->CFGR |= RCC_CFGR_PLLSRC;
        
        // Set PLL multiplication factor to 3
        RCC->CFGR &= ~RCC_CFGR_PLLMUL;
        RCC->CFGR |= RCC_CFGR_PLLMUL_0;
        
        // Turn on the PLL
        RCC->CR |= RCC_CR_PLLON;

        // Wait until PLL ready
        while((RCC->CR & RCC_CR_PLLRDY) == 0);

        // Set the SYSCLK source to PLL
        RCC->CFGR &= ~RCC_CFGR_SW;
        RCC->CFGR |= RCC_CFGR_SW_1;

        // Verify that the PLL output is being used for SYSCLK
        if((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_1)
        {
                // Turn off the HSI to conserve energy
                RCC->CR &= ~RCC_CR_HSION;
        }
        
        // Enable the GPIOA peripheral clock
        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        
        // Configure PA8 for alternate function mode
        GPIOA->MODER &= ~GPIO_MODER_MODER8;
        GPIOA->MODER |= GPIO_MODER_MODER8_1;
                
        // Set MCO bits in RCC_CFGR to select SYSCLK as the output signal
        RCC->CFGR &= ~RCC_CFGR_MCO;
        RCC->CFGR |= RCC_CFGR_MCO_2;
}