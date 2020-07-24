#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>	

int serial_setup(char * port_name, unsigned int baud_rate)
{
	struct termios cfg;
	int fd = -1;

	fd = open(port_name, O_RDWR | O_NOCTTY );
	
	if(fd == -1)
	{
		printf("Failed to open port.\n");
	}
	else
	{
		tcgetattr(fd, &cfg);
		cfg.c_cflag = baud_rate | CS8 | CLOCAL | CREAD;		
		cfg.c_iflag = IGNPAR;
		cfg.c_oflag = 0;
		cfg.c_lflag = 0;
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &cfg);
		printf("Serial port ready.\n");
	}
	
	return fd;
}