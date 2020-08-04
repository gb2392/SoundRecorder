#include "input_handling.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

static FILE * m_fp = NULL;			/*media file pointer*/
static int p_fd = -1;				/*port file descriptor*/
static uint8_t buf[BUF_SIZE];			/*rx buffer*/
static uint32_t buf_index = 0;			/*rx buffer write index*/
static uint32_t buf_remaining = BUF_SIZE;	/*space left in rx buffer*/
static uint32_t num_read = 0;			/*bytes acquired with a read*/

/*
 *input_handler - handler for servicing SIGIO.
 *int x		: receives a signal number.
 * 
 *Reads available data into the rx buffer. Based on the number of data read,
 *it increments the buffer write index and updates the variable which keeps 
 *track of the space remaining in the buffer. 
 *
 *Note: buffer overruns are prevented by virtue of the "buf_remaining" variable
 *settling at zero once the buffer capacity has been reached. Because 
 *"buf_remaining" is used as the last argument to the "read" function call, 
 *when it is zero, the read function will move no data and return zero. 
 */
static void input_handler(int x)
{
	num_read = read(p_fd, &(buf[buf_index]), buf_remaining);
	buf_index += num_read;
	buf_remaining = BUF_SIZE - buf_index;
}

/*
 *get_buf - returns a pointer to the buffer used for storing received data. 
 *
 *Return : pointer to received data. 
 */
uint8_t * get_buf(void)
{
	return buf;
}

/*
 *get_total_bytes_received - accessor for the total bytes received.  
 *
 *Return : the total number of bytes received thus far. 
 */
uint32_t get_total_bytes_received(void)
{
	return buf_index;
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