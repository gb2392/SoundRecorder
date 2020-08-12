#include "reconstitution.h"

/*
 *reconstitute - converts encoded MSB and LSB to a 16-bit integer to return.
 *uint8_t msb_in : the most significant byte of the encoded data. 
 *uint8_t lsb_in : the least significant byte of the encoded data. 
 *
 *The encoded MSB contains the lower four bits of the original MSB, as well as
 *the upper two bits of the original LSB. The encoded LSB contains the lower 
 *six bits of the original LSB. This function gets those bits out, and repacks
 *them into a 16-bit integer which represents the original from which the 
 *encoded data were derived. 
 *
 *Return : the original 16-bit integer. 
 */
uint16_t reconstitute(uint8_t msb_in, uint8_t lsb_in)
{
	uint8_t msb_out = 0;
	uint8_t lsb_out = 0;

	//Get bits 5:2 of the MSB and write to 3:0 of msb_out
	msb_out = (((msb_in & BITS_5_2_MASK) >> 2) & 0x0f);

	//Get bits 1:0 of the MSB and write to 7:6 of lsb_out
	lsb_out = ((msb_in & BITS_1_0_MASK) << 6);

	//Get bits 5:0 of the LSB and OR to 5:0 of lsb_out 
	lsb_out &= ~BITS_5_0_MASK;
	lsb_out |= (lsb_in & BITS_5_0_MASK);

	return ((uint16_t)((msb_out << 8) | (lsb_out)));
}
