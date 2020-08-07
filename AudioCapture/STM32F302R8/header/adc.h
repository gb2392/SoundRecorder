#ifndef ADC_H
#define ADC_H
#include <stm32f302x8.h>
#include <stdint.h>

typedef enum 
{
        ADC_STATE_NOT_CONFIGURED = 0,
        ADC_STATE_CONFIGURED = 1
}ADC_STATE;

void adc_setup(void);
void adc_set_ADSTART(void);
uint8_t adc_can_start(void);
void adc_drive_timer_setup(void);

#endif