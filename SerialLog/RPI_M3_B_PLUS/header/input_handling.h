#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H
#include <stdint.h>
#include <stdio.h>

#define BUF_SIZE			10000000UL

uint8_t * get_buf(void);
uint32_t get_total_bytes_received(void);
uint8_t input_handling_setup(int port_fd, FILE * media_fp);

#endif