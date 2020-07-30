#include "input_handling.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

static FILE * m_fp = NULL;
static int p_fd = -1;

static void input_handler(int x)
{
	uint16_t num_bytes_received;
	static uint8_t staging_buffer[STAGING_BUFFER_SIZE];
	
	num_bytes_received = read(p_fd, staging_buffer, STAGING_BUFFER_SIZE);
	fwrite((void *)staging_buffer, 1, num_bytes_received, m_fp);
	printf("Got data\n");
}

uint8_t input_handling_setup(int port_fd, FILE * media_fp)
{
	const int INIT_FAILED = 0;
	const int INIT_SUCCEEDED = 1;
	struct sigaction s;
	int status;

	if((port_fd == -1) || (media_fp == NULL))
	{
		status = INIT_FAILED;
	}
	else
	{
		m_fp = media_fp;
		p_fd = port_fd;
		s.sa_handler = input_handler;
		s.sa_flags = 0;
		sigaction(SIGIO, &s, NULL);
		fcntl(port_fd, F_SETFL, O_NDELAY | O_ASYNC);
		status = INIT_SUCCEEDED;
	}

	return status;
}