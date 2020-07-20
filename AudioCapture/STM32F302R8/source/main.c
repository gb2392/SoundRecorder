#include "stm32f302x8.h"
#include "clock_configuration.h"
#include "debug.h"
#include "time.h"

void main(void)
{
        clock_setup();
        debug_setup();
        systick_setup();

        while(1)
        {
               
        }
}
