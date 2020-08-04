#include "main.h"
#include "serial.h"
#include "input_handling.h"
#include <stdio.h>
#include <termios.h>

/*
 *main - sets up program data and implements an infinite loop to run. 
 *
 *This function establishes variables related to storage media and serial 
 *communication, calls initialization routines, and implements an infinite loop
 *for runtime operation. 
 *       
 */
void main(void)
{
	FILE * media_fp = fopen("dat", "w");
	int port_fd = serial_setup("/dev/serial0", B921600);

	if(input_handling_setup(port_fd, media_fp) == 0)
	{
		printf("error\n");
		return;
	}

	while(1)
	{
		if(getchar() == 'q')
		{
			printf("received a total of %u bytes\n", get_total_bytes_received());
			printf("saving data\n");
			fwrite(get_buf(), 1, get_total_bytes_received(), media_fp);
			printf("closing file and exiting\n");
			fclose(media_fp);
			return;
		}
	}
}