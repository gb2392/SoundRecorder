#include "stm32f302x8.h"
#include "clock_configuration.h"
#include "debug.h"

void main(void)
{
        clock_setup();
        debug_setup();

        while(1)
        {
               
        }
}
