#include "input_handling.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

static FILE * m_fp = NULL;
static int p_fd = -1;
static uint8_t staging_buffer[STAGING_BUFFER_SIZE];
static uint16_t num_bytes_received;
static uint32_t total_bytes_received = 0;

/*
 *input_handler - handler for servicing SIGIO. 
 *int x		: receives a signal number. 
 * 
 *Main purpose is to read available data from the serial port and then write it
 *to a file. 
 */
static void input_handler(int x)
{
	num_bytes_received = read(p_fd, staging_buffer, STAGING_BUFFER_SIZE);
	fwrite((void *)staging_buffer, 1, num_bytes_received, m_fp);
	total_bytes_received += num_bytes_received;
}

/*
 *get_total_bytes_received - accessor for the total bytes received.  
 *
 *Return : the total number of bytes received thus far. 
 */
uint32_t get_total_bytes_received(void)
{
	return total_bytes_received;
}

/*
 *input_handling_setup - registers a handler with the SIGIO signal. 
 *int port_fd		: file descriptor for serial port. 
 *FILE * media_fp	: pointer to file where received data are to be stored.
 *
 *In addition to associating a handler with the SIGIO signal, this function 
 *initializes the serial port file descriptor and storage media file pointer 
 *variables, and as well performs additional config. of the serial port. 
 *
 *Return : status on whether or not the setup was successful. A value of 1
 *         indicates success; a value of 0 indicates failure. 
 */
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