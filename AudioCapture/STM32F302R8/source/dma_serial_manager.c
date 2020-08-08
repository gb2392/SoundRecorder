#include "dma_serial_manager.h"
#include "serial.h"
#include "stm32f302x8.h"
#include <debugio.h>

static uint16_t sample_buf_0[SAMPLE_BUF_SIZE];
static uint16_t sample_buf_1[SAMPLE_BUF_SIZE];

void dma_serial_manager_setup(void)
{
        // Will be using channel 1 for ADC

        // Enable the DMA peripheral clock
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;

        // Set the peripheral address
        DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));

        // Set the memory address 
        DMA1_Channel1->CMAR = (uint32_t)sample_buf_0;

        // Set the total number of elements to be transferred
        DMA1_Channel1->CNDTR = SAMPLE_BUF_SIZE;

        // Configure the channel priority to very high
        DMA1_Channel1->CCR |= DMA_CCR_PL;

        // Set the data transfer direction to read from peripheral
        DMA1_Channel1->CCR &= ~DMA_CCR_DIR;

        // Set circular mode disabled.
        DMA1_Channel1->CCR &= ~DMA_CCR_CIRC;

        // Set memory pointer to increment after each operation
        DMA1_Channel1->CCR |= DMA_CCR_MINC;

        // Set peripheral pointer to NOT increment after each operation
        DMA1_Channel1->CCR &= ~DMA_CCR_PINC;

        // Set the memory data size to 16 bits
        DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE;
        DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;

        // Set the peripheral data size to 16 bits
        DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE;
        DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

        // Set interrupt after full transfer
        DMA1_Channel1->CCR |= DMA_CCR_TCIE;

        // Activate DMA by setting enable bit
        DMA1_Channel1->CCR |= DMA_CCR_EN;

        NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}       

void DMA1_Channel1_IRQHandler(void)
{
        // Deactivate DMA before changing settings
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;

        // Clear the transfer complete interrupt flag for channel 1
        DMA1->IFCR |= DMA_IFCR_CTCIF1;

        // Clear the global interrupt flag

        // Start transmitting buffered data
        usart_1_transmit((void *)(DMA1_Channel1->CMAR), SAMPLE_BUF_SIZE);

        if(DMA1_Channel1->CMAR == (uint32_t)sample_buf_0)
        {
                DMA1_Channel1->CMAR = (uint32_t)sample_buf_1;
                debug_printf("switching to buffer 1\n");
        }
        else
        {
                DMA1_Channel1->CMAR = (uint32_t)sample_buf_0;
                debug_printf("switching to buffer 0\n");
        }

        // Reload the count register
        DMA1_Channel1->CNDTR = SAMPLE_BUF_SIZE;

        // Reactivate DMA channel 1
        DMA1_Channel1->CCR |= DMA_CCR_EN;

        ADC1->CR |= ADC_CR_ADSTART;
}