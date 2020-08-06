#include "adc.h"
#include "time.h"

static uint8_t adc_state = ADC_STATE_NOT_CONFIGURED;

/*
 *adc_setup - configures analog to digital converter. 
 *
 *In development / not tested. 
 */
void adc_setup(void)
{
        // Configure PA1 for use as an analog input

        RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

        // Set the mode of PA1 to analog
        GPIOA->MODER |= GPIO_MODER_MODER1;

        // No pull up/down resistors
        GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1;

        RCC->AHBENR |= RCC_AHBENR_ADC1EN;

        // ADC configuration 

        // Actions when ADC is disabled (ADEN = 0)

        // ADC clock source HCLK / 4. HCLK is 24 MHz, giving ADC CLK 6 MHz.
        ADC1_COMMON->CCR |= ADC1_CCR_CKMODE;

        // ADC voltage regulator startup
        ADC1->CR &= ~ADC_CR_ADVREGEN;
        ADC1->CR |= ADC_CR_ADVREGEN_0;

        // Give the regulator time to stabilize
        delay_ms(2);

        // Select single ended input
        ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_2;

        // ADC calibration procedure

        // Select for single ended calibration
        ADC1->CR &= ~ADC_CR_ADCALDIF;

        // Initiate calibration
        ADC1->CR |= ADC_CR_ADCAL;

        // Wait until complete (ADCAL cleared by hardware once cal. complete).
        while(ADC1->CR & ADC_CR_ADCAL);

        // Enable ADC
        ADC1->CR |= ADC_CR_ADEN;

        // Actions when ADC is enabled (ADEN = 1)

        // Set the regular conversion length to one
        ADC1->SQR1 &= ~ADC_SQR1_L;
        
        // Set the zeroth conversion channel as channel 2
        ADC1->SQR1 &= ~ADC_SQR1_SQ1;
        ADC1->SQR1 |= (2 << ADC_SQR1_SQ1_Pos);

        // Set the channel 2 sampling time to 19.5 ADC clock cycles
        ADC1->SMPR1 &= ~ADC_SMPR1_SMP2;
        ADC1->SMPR1 |= ADC_SMPR1_SMP2_2;

        // Single conversion mode
        ADC1->CFGR &= ~ADC_CFGR_CONT;

        adc_state = ADC_STATE_CONFIGURED;
}

void adc_set_ADSTART(void)
{
        ADC1->CR |= ADC_CR_ADSTART;
}

uint8_t adc_can_start(void)
{
        return (adc_state == ADC_STATE_CONFIGURED);
}