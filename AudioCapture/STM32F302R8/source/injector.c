#include "injector.h"

#if (INJECTION_TYPE_TEST > 0)

/*
 *injector_test - returns bytes which are the LSB and MSB of 16-bit integers.
 *
 *Calls to this function return bytes which constitute the LSB and MSB of an
 *incrementing unsigned 16-bit integer. For example, the first call to this 
 *function returns the LSB of 0x0000, the second call returns the MSB of 
 *0x0000, the third call returns the LSB of 0x0001, the fourth call returns
 *the MSB of 0x0001, and so on as subsequent calls are made. The 16-bit 
 *integer value rolls over to 0x0000 after it reaches 0xFFFF.
 *
 *The purpose of this method is to evaluate gaps in the actual data buffered
 *by the receiving entity. Knowing before hand what the data should have
 *been, it is easier to discern what may have been missed, and when. 
 *
 *Return : uint8_t value which is either the MSB or LSB of a 16-bit integer.
 */
uint8_t injector_test(void)
{
        volatile static uint8_t ret = 0;
        volatile static uint8_t msb = 0;
        volatile static uint16_t value = 0;

        if(msb)
        {
                // return the msbyte
                ret = (uint8_t)((value >> 8) & 0xff);
                value++;
                msb = 0;
        }
        else
        {
                // return the lsbyte
                ret = (uint8_t)(value & 0xff);
                msb = 1;
        }

        return ret; 
}

DATA_INJECTOR injected_value = injector_test;

#elif (INJECTION_TYPE_12_BIT_ENC > 0)

/*
 *injector_12_bit_enc - returns bytes that encode 12-bit data with known codes.
 *
 *In development. 
 *
 *Return : uint8_t of fixed value (0x0f).
 */
uint8_t injector_12_bit_enc(void)
{
        return 0x0f;
}

DATA_INJECTOR injected_value = injector_12_bit_enc;

#else
/*
 *injector_default - the default injector when injection enabled. 
 *
 *Return : always returns zero. 
 */
uint8_t injector_default(void)
{
        return 0; 
}

DATA_INJECTOR injected_value = injector_default;

#endif