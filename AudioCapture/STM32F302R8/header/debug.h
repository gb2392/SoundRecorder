#ifndef DEBUG_H
#define DEBUG_H

#define TOGGLE_PC6      GPIOC->ODR ^= GPIO_ODR_6;

void debug_setup(void);

#endif