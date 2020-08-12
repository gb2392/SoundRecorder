#include "decoder.h"
#include "reconstitution.h"
#include <stdio.h>

/*
 *decoder - takes encoded input data, and writes original data out. 
 *uint8_t input	: the next byte to examine. 
 *void (*output_write)(uint16_t) : for a function that writes data to memory.
 *
 *This function takes encoded input data as an 8-bit integer, and attempts 
 *to reconstruct the original 16-bit data from which it was derived. It does
 *this by keeping track of both the current input value, as well as the 
 *one which occurred immediately before, and performing a check to verify 
 *integrity. If verification fails, this function omits the corrupted data,
 *and recovers so that downstream data can still be interpreted correctly.  
 */
void decoder(uint8_t input, void (*output_write)(uint16_t))
{
	static uint8_t state = STATE_LSB;
	static uint8_t lsb_in;
	uint8_t msb_in;

	if(state == STATE_LSB)
	{
		lsb_in = input;
		state = STATE_MSB;
	}
	else
	{
		msb_in = input;

		if((lsb_in & PAIR_CODE_MASK) == (msb_in & PAIR_CODE_MASK))
		{
			output_write(reconstitute(msb_in, lsb_in));
			state = STATE_LSB;
		}
		else
		{
			lsb_in = msb_in;
		}
	}
}

#if (TEST_ENABLE > 0)

static uint16_t test_output_buffer[TEST_BUFFER_SIZE];

/*
 *test_write - writes 16-bit values to a buffer and increments the index.
 *uint16_t value	: The data to write. 
 *
 *This function is a simple implementation of what is required for the 
 *second argument of the function "decoder". It emulates a data "sink". 
 */
void test_write(uint16_t value)
{
	static uint8_t buffer_pos = 0;
	
	if(buffer_pos < TEST_BUFFER_SIZE)
	{
		test_output_buffer[buffer_pos++] = value;
	}
}

/*
 *test_print_output - print contents of buffer with decoded results. 
 */
void test_print_output(void)
{
	for(uint8_t i = 0; i < TEST_BUFFER_SIZE; i++)
	{
		printf("0x%04X\n", test_output_buffer[i]);
	}
}

/*
 *test_read - emulates a data "source"; provides the input data for the test. 
 */
uint8_t test_read(uint8_t * value)
{
	/*
	 *This array contains data with pair codes and the original content of
	 *the source data set. Here is the source data (in terms of 16-bit
	 *integers) from which the "read_buffer" array contents were derived.
	 *
	 *{ 0x0F52, 0x039B, 0x0780, 0x0CF2, 0x01B7 }
	 */
	static uint8_t read_buffer[] = 
	{ 0x12, 0x3d, 0x5b, /*0x4e,*/ 0x80, 0x9e, 0xf2, 0xf3, 0x37, 0x06 };
	
	static uint8_t index = 0;
	
	if(index < sizeof(read_buffer))
	{
		*value = read_buffer[index++];
		return 1;
	}
	
	return 0;
}

/*
 *test_decoder - runs the test. 
 *
 *Calls the "test_read" function to get the next value from the test data set, 
 *and then submits the value to the decoder, along with the function which 
 *facilitates storing the results of the decoding effort. 
 */
void test_decoder(void)
{
	printf("Running decoder test...\n");
	uint8_t current_value;
	
	while(test_read(&current_value))
	{
		decoder(current_value, test_write);
	}
	
	test_print_output();
}

#endif