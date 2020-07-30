#include "main.h"
#include "serial.h"
#include "input_handling.h"
#include <stdio.h>
#include <unistd.h>			
#include <termios.h>	
#include <signal.h>	
#include <fcntl.h>

unsigned char buf[1028];
int global_port_fd;
volatile int rx_bytes;
volatile int i;

void io_handler(int x)
{
	printf("Hello from signal handler.\n");
	rx_bytes = read(global_port_fd, buf, 100);

	for(i = 0; i < rx_bytes; i++)
		printf("%02X\n", buf[i]);

	printf("--------\n");
}

void input_handler(int x)
{
	
}

void main(void)
{
	/*
	FILE * file;
	int run = 1;

	//record = fopen("record_pcm", "w");
	//fwrite(bin_data, 1, 5, file);

	unsigned char buffer[32];
	int port = serial_setup("/dev/serial0", B921600);

	if(port == -1)
		return;

	while(run)
	{
		read(port, buffer, 1);
		printf("%d\n", buffer[0]);
		//fwrite(buffer, 1, 1, record);
	}

	//fclose(record);
	
	//////////////////////////////////////////////////////

	int pfd = serial_setup("/dev/serial0", B921600);
	unsigned char buffer[512];
	if(pfd == -1) return;

	int run = 1;
	int number_of_bytes_read = 0;

	while(run)
	{
		// Whatever happens to be read, write out to something. 
		number_of_bytes_read = read(pfd, buffer, 1);
		
		if(number_of_bytes_read == 1)
		{
			printf("%d\n", buffer[0]);
		}



	}

	

	/////////////////

	struct sigaction s;
	int port_fd = serial_setup("/dev/serial0", B921600);
	if(port_fd == -1) return;
	global_port_fd = port_fd;

	s.sa_handler = io_handler;
	s.sa_flags = 0;
	//s.sa_mask = 0;
	sigaction(SIGIO, &s, NULL);

	printf("Other\n");
	//fcntl(port_fd, F_SETOWN, getpid());
	fcntl(port_fd, F_SETFL, O_NDELAY | O_ASYNC);
*/

	FILE * media_fp = fopen("data_file", "w");
	int port_fd = serial_setup("/dev/serial0", B921600);
	
	if(input_handling_setup(port_fd, media_fp) == 0)
	{
		printf("Error\n");
		return;
	}
	
	while(1)
	{
		if(getchar() == 'q')
		{
			return;
		}
		else
		{
			printf("input not recognized\n");
		}
	}
}