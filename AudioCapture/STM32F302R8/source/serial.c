#include "stm32f302x8.h"
#include "serial.h"
#include "injector.h"
#include <stdint.h>

static USART_1_RUNTIME usart_1_runtime = { .state = USART_1_STATE_NOT_CONFIGURED };

/*
 *usart_1_setup - configures USART 1 peripheral
 *
 *This function attempts to configure the USART1 peripheral hardware for 
 *the following:
 *      -baud rate : 921600
 *      -parity : none
 *      -number of data bits : 8
 *
 *This function must be called before making calls to other functions which
 *depend on the USART 1 hardware (such as usart_1_transmit). 
 */
void usart_1_setup(void)
{
        const uint32_t USART_1_CLOCK_FREQUENCY = 24000000;
        const uint32_t USART_1_BAUD_RATE = 576000;

        // Configure PC4 for use as USART 1 Tx
        
        // Enable GPIOC peripheral clock
        RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

        // Set the mode of PC4 to alternate function
        GPIOC->MODER &= ~GPIO_MODER_MODER4;
        GPIOC->MODER |= GPIO_MODER_MODER4_1;

        // Set the output type of PC4 to push-pull
        GPIOC->OTYPER &= ~GPIO_OTYPER_OT_4;

        // Set the PC4 speed to high
        GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4;

        // Set PC4 to have no pull up/down resistors
        GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR4;

        // Set the alternate function for PC4 to 7 (for USART1 TX)
        GPIOC->AFR[0] &= ~GPIO_AFRL_AFRL4;
        GPIOC->AFR[0] |= (7U << GPIO_AFRL_AFRL4_Pos);

        // Configure USART 1 peripheral 

        // Set the USART 1 clock source to SYSCLK
        RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
        RCC->CFGR3 |= RCC_CFGR3_USART1SW_0;

        // Enable the USART 1 peripheral clock
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 

        // Set the word length to 8 bits
        USART1->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);

        // Select for oversampling by 16
        USART1->CR1 &= ~USART_CR1_OVER8;

        // Set the baud rate
        USART1->BRR = USART_1_CLOCK_FREQUENCY / USART_1_BAUD_RATE;

        // Set the number of stop bits to 1
        USART1->CR2 &= ~USART_CR2_STOP;

        // Enable the usart
        USART1->CR1 |= USART_CR1_UE;

        // Set the TE bit to send an idle frame
        USART1->CR1 |= USART_CR1_TE;

        // Enable interrupt 
        NVIC_EnableIRQ(USART1_IRQn);

        // Wait until the idle frame is finished clocking out onto the TX line
        while((USART1->ISR & USART_ISR_TC) == 0);

        // USART1 configured and ready for use
        usart_1_runtime.state = USART_1_STATE_IDLE;
}

/*
 *is_usart_1_ready - checks to see if USART1 can be used to transmit.
 *
 *Return : value of 0 if not ready; value of 1 if ready. 
 */
uint8_t is_usart_1_ready(void)
{
        return (usart_1_runtime.state == USART_1_STATE_IDLE);
}

/*
 *usart_1_transmit - employs USART1 to transmit a vector of data. 
 *void * data           : pointer to the start of the data to send
 *uint32_t length       : number of bytes to send
 *
 *Attempts to use USART1 to transmit a vector of data. Prior to calling this 
 *function, the function "is_usart_1_ready" may need to be called in order
 *to check whether USART1 is available to execute a new transmission. As well, 
 *the function "usart_1_setup" should have been called prior to any call to 
 *"usart_1_transmit". 
 *
 *Data is transmitted via pin PC4 (USART1 TX). 
 */
void usart_1_transmit(void * data, uint32_t length)
{
        if(length == 0)
        {
                return;
        }

        usart_1_runtime.state = USART_1_STATE_TX;
        usart_1_runtime.tx_length = length;
        usart_1_runtime.tx_data = (uint8_t *)data;
        usart_1_runtime.tx_index = 0;
        
        // Send the zeroth byte
#if (ENABLE_INJECTION > 0)
        usart_1_runtime.tx_index++;
        USART1->TDR = injected_value();
#else
        USART1->TDR = usart_1_runtime.tx_data[usart_1_runtime.tx_index++];
#endif

        // Enable the TX complete (TC) interrupt
        USART1->CR1 |= USART_CR1_TCIE;

        if(length != 1)
        {
                // Enable the TX empty (TXE) interrupt
                USART1->CR1 |= USART_CR1_TXEIE;
        }
}

/*
 *USART1_IRQHandler - handler for USART1 generated interrupts. 
 *
 *Currently only supports transmission related activity. Code here should be 
 *kept short. 
 */
void USART1_IRQHandler(void)
{
        if(usart_1_runtime.state == USART_1_STATE_TX)
        {
                if(usart_1_runtime.tx_index == usart_1_runtime.tx_length)
                {
                        // When there are no more bytes to send
                        USART1->CR1 &= ~USART_CR1_TCIE;
                        usart_1_runtime.state = USART_1_STATE_IDLE;
                }
                else if(usart_1_runtime.tx_index == (usart_1_runtime.tx_length - 1))
                {
                        // When there is just one more byte to send
                        USART1->CR1 &= ~USART_CR1_TXEIE;
#if (ENABLE_INJECTION > 0)
                        usart_1_runtime.tx_index++;
                        USART1->TDR = injected_value();
#else
                        USART1->TDR = usart_1_runtime.tx_data[usart_1_runtime.tx_index++];
#endif
                }
                else
                {
                        // When there are multiple bytes left to send
#if (ENABLE_INJECTION > 0)
                        usart_1_runtime.tx_index++;
                        USART1->TDR = injected_value();
#else
                        USART1->TDR = usart_1_runtime.tx_data[usart_1_runtime.tx_index++];
#endif
                        
                }
        }
}