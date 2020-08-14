#include "main.h"
#include "serial.h"
#include "input_handling.h"
#include <stdio.h>
#include <termios.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include "decoder.h"

static FILE * writer_fp = NULL;

void writer(uint16_t value)
{
	if(writer_fp == NULL)
	{
		return;
	}
	
	fwrite(&value, sizeof(uint16_t), 1, writer_fp);
}

/*
 *main - sets up program data and implements an infinite loop to run. 
 *
 *This function establishes variables related to storage media and serial 
 *communication, calls initialization routines, and implements an infinite loop
 *for runtime operation. 
 *
 *Notes: B230400 appears to work well enough. B921600 doesn't. 
 */
void main(void)
{	
	FILE * media_fp_encoded = fopen("encoded", "w");
	FILE * media_fp_recovered = fopen("recovered", "w");
	
	int port_fd = serial_setup("/dev/serial0", B576000);
	uint32_t total_bytes_received; 
	uint8_t * receive_buffer = NULL;

	if(input_handling_setup(port_fd, media_fp_encoded) == 0)
	{
		printf("error\n");
		return;
	}

	while(1)
	{
		if(getchar() == 'q')
		{
			total_bytes_received = get_total_bytes_received();
			receive_buffer = get_buf();
			
			printf("received a total of %u bytes\n", total_bytes_received);
			printf("saving data\n");
			
			fwrite(receive_buffer, 1, total_bytes_received, media_fp_encoded);
			writer_fp = media_fp_recovered;
			
			for(uint32_t i = 0; i < total_bytes_received; i++)
			{
				decoder(receive_buffer[i], writer);
			}
			
			printf("closing files and exiting\n");
			fclose(media_fp_encoded);
			fclose(media_fp_recovered);
			return;
		}
	}
}