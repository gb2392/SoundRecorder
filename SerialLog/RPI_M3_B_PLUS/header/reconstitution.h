#ifndef RECONSTITUTION_H
#define RECONSTITUTION_H
#include <stdint.h>

#define BITS_1_0_MASK			(0x03)
#define BITS_5_0_MASK			(0x3f)
#define BITS_5_2_MASK			(0x3c)

uint16_t reconstitute(uint8_t msb_in, uint8_t lsb_in);

#endif