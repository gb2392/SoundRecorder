#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H
#include <stdint.h>
#include <stdio.h>

#define STAGING_BUFFER_SIZE			5000000UL

uint32_t get_total_bytes_received(void);
uint8_t input_handling_setup(int port_fd, FILE * media_fp);

#endif