#include "adc.h"
#include "time.h"
#include "debugio.h"

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

        // Wait for ADRDY
        while((ADC1->ISR & ADC_ISR_ADRDY) == 0);

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

        // Select the external trigger for the ADC (TIM1_CC1).
        ADC1->CFGR &= ~ADC_CFGR_EXTSEL;

        // Select the trigger polarity (falling edge). 
        ADC1->CFGR &= ~ADC_CFGR_EXTEN;
        ADC1->CFGR |= ADC_CFGR_EXTEN_1;

        // Enable the end-of-conversion (EOC) interrupt. 
        //ADC1->IER |= ADC_IER_EOCIE;

        // Enable DMA ops for ADC conversions
        ADC1->CFGR |= ADC_CFGR_DMAEN;

        // Set DMA one shot mode
        ADC1->CFGR &= ~ADC_CFGR_DMACFG;

        // Enable the interrupt request
        NVIC_EnableIRQ(ADC1_2_IRQn);

        adc_state = ADC_STATE_CONFIGURED;
}

void ADC1_IRQHandler(void)
{
        static uint32_t index = 0;
        debug_printf("Got: %u at index: %d\n", ADC1->DR, index++);
}

/*
 *adc_drive_timer_setup - configures TIM1 to drive ADC1 conversions. 
 *
 *This function assumes an APB2 frequency of 24 MHz. In development.
 */
void adc_drive_timer_setup(void)
{
        // Configure PC0 as an output to verify output compare signal

        // Enable the port C peripheral clock
        RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

        // Set PC0 to alternate function mode
        GPIOC->MODER &= ~GPIO_MODER_MODER0;
        GPIOC->MODER |= GPIO_MODER_MODER0_1;

        // Set output type to push-pull
        GPIOC->OTYPER &= ~GPIO_OTYPER_OT_0;

        // Set the output speed to high
        GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;

        // No pull up/down resistors
        GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR0;

        // Configure PC0 for alternate function 2 (TIM1_CH1)
        GPIOC->AFR[0] &= ~GPIO_AFRL_AFRL0;
        GPIOC->AFR[0] |= (2U << GPIO_AFRL_AFRL0_Pos);

        // Now, configure TIM1

        // Enable the TIM1 peripheral clock
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

        // Set the counting direction to up 
        TIM1->CR1 &= ~TIM_CR1_DIR;

        // Select the counter clock prescale factor
        TIM1->PSC = 239;

        // Set the auto-reload register
        TIM1->ARR = 65535;

        // Set the capture compare register
        TIM1->CCR1 = 32000;

        // Set the output to toggle on match
        TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
        TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0);

        // Disable the preload register
        TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE;

        // Configure polarity to active high
        TIM1->CCER &= ~TIM_CCER_CC1P;

        // Enable the output
        TIM1->CCER |= TIM_CCER_CC1E;

        // Configure capture compare selection as output
        TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;

        // Set the main output enable bit
        TIM1->BDTR |= TIM_BDTR_MOE;

        // Enable the counter
        TIM1->CR1 |= TIM_CR1_CEN;
}

/*
 *adc_set_ADSTART - wrapper for starting an ADC conversion.
 */
void adc_set_ADSTART(void)
{
        ADC1->CR |= ADC_CR_ADSTART;
}

/*
 *adc_can_start - checks if a call to adc_set_ADSTART may be made. 
 *
 *Return : 0 means ADSTART should not be set; 1 means okay to set ADSTART. 
 */
uint8_t adc_can_start(void)
{
        return (adc_state == ADC_STATE_CONFIGURED);
}