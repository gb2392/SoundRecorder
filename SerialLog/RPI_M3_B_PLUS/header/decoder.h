#ifndef DECODER_H
#define DECODER_H
#include <stdint.h>

#define PAIR_CODE_MASK			(0xc0)
	
#define STATE_LSB			1
#define STATE_MSB			2

#define TEST_ENABLE			(1)
#define TEST_BUFFER_SIZE		5

void decoder(uint8_t input, void (*output_faculty)(uint16_t));
void test_decoder(void);

#endif