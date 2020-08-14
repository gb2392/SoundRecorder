#include "injector.h"
#include "serial.h"

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
 *Definitions for use in the "injector_12_bit_enc" function.
 */

#define STATE_LSB               1
#define STATE_MSB               2

#define BITS_7_6_MASK           0xc0
#define BITS_5_0_MASK           0x3f
#define BITS_1_0_MASK           0x03

/*
 *injector_12_bit_enc - returns bytes that encode 12-bit data with pair codes.
 *
 *This function acquires data (one byte at a time) from the write buffer 
 *of the serial driver, at the current index maintained by the serial driver.
 *It is only intended to be used when the serial driver is attempting to send
 *12-bit samples stored in two bytes, with little endian ordering. Example:
 *
 *Original 12-bit Sample:
 *byte 0 (LSB): [7:0 are the lower 8 bits of the sample]
 *byte 1 (MSB): [7:4 unused | 3:0 are the upper 4 bits of the sample]
 *
 *Encoded Pair (what ends up actually being transmitted):
 *byte 0 (LSB): [7:6 pair code | 5:0 lower 6 bits of sample]
 *byte 1 (MSB): [7:6 pair code | 5:2 upper 4 bits of sample | 1:0 are bits 7:6 of sample]
 *
 *Return : uint8_t with encoded pair code and sample bits. 
 */
uint8_t injector_12_bit_enc(void)
{
        static uint8_t state = STATE_LSB;
        static uint8_t pair_code = 0;

        static uint8_t sample_lsb;
        uint8_t sample_msb;
        uint8_t encoded_lsb;
        uint8_t encoded_msb;
        
        uint8_t result; 

        // Only want values 0 - 3, so mask out bits 7:2. 
        pair_code &= BITS_1_0_MASK;

        if(state == STATE_LSB)
        {
                // Need to hold on to the sample LSB for next cycle, as its
                // top two bits are going to be packed into the lower two bits 
                // of the encoded MSB. 
                sample_lsb = usart_1_runtime.tx_data[usart_1_runtime.tx_index];

                // The encoded LSB consists of the lower six bits of the sample
                // LSB at 5:0, and the two bit pair code, at 7:6
                encoded_lsb = (sample_lsb & BITS_5_0_MASK);
                encoded_lsb |= ((pair_code << 6) & BITS_7_6_MASK);
                
                result = encoded_lsb;
                state = STATE_MSB;
        }
        else
        {
                // State MSB
                sample_msb = usart_1_runtime.tx_data[usart_1_runtime.tx_index];
                
                // Pack bits 3:0 of sample MSB to bits 5:2 of encoded MSB
                encoded_msb = (sample_msb << 2);
                
                // Pack bits 7:6 of sample LSB into bits 1:0 of encoded MSB
                encoded_msb &= ~BITS_1_0_MASK;
                encoded_msb |= ((sample_lsb >> 6) & BITS_1_0_MASK);
                
                // Pack the pair code
                encoded_msb &= ~BITS_7_6_MASK;
                encoded_msb |= ((pair_code << 6) & BITS_7_6_MASK);
                
                state = STATE_LSB;
                result = encoded_msb;
                pair_code++;
                
        }

        return result;
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