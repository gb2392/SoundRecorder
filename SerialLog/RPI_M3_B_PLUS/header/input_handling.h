#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H
#include <stdint.h>
#include <stdio.h>

#define STAGING_BUFFER_SIZE			100

uint8_t input_handling_setup(int port_fd, FILE * media_fp);

#endif