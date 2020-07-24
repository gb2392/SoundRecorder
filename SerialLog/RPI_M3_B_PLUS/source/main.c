#include "main.h"
#include "serial.h"
#include <stdio.h>
#include <unistd.h>			
#include <termios.h>		

void main(void)
{	
	unsigned char buffer[32];
	int port = serial_setup("/dev/serial0", B921600);
	
	if(port == -1)
		return;
	
	while(1)
	{
		read(port, buffer, 1);
		printf("0x%x\n", buffer[0]);
	}
}