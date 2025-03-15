/**
* @file drv_maria.c
*/

#include <stdint.h>
#include <maria.h>
#include <drv_maria.h>

#include "brd_pwm_generator.h"

//======================================================================================================
// GLOBAL VARIABLES
//======================================================================================================

//======================================================================================================
// STATIC PROTOTYPES & STATIC VARIABLES
//======================================================================================================

//======================================================================================================
// IMPLEMENTATION
//======================================================================================================
int maria_read(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data)
{
	/* aggiorno il selettore di canale attivo */
	g_active_fd_index = user_data->fdidx;

	if(fd == (int)(&huart_CONSOLE))
	{
		/* Read one byte from the receive data register */
		ARD_memcpy(buf, g_brdStat.uart_CONSOLE_RxBuf, g_brdStat.uart_CONSOLE_RxBufLen);
		count = g_brdStat.uart_CONSOLE_RxBufLen;

		g_brdStat.uart_CONSOLE_RxBufLen = 0;
		g_brdStat.uart_CONSOLE_LineReady = 0;
	}

	return count;
}

int maria_write(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data)
{
	if(fd == (int)(&huart_CONSOLE))
	{
		HAL_UART_Transmit(&huart_CONSOLE, buf, count, HAL_MAX_DELAY);
	}


	return count;
}

/* nelle applicazioni FW tipicamente i parametri pdf_r e pdf_w sono NULL in quanto
   la gestione dei file descriptor va simulata */
void maria_open(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data)
{
	MARIA_ADD_CR = 0;

	int chan = user_data->fdidx;

	switch(chan)
	{
	case RS232_FD_IDX:
		maria_data[chan].r_fd = (int)(&huart_CONSOLE);
		maria_data[chan].w_fd = (int)(&huart_CONSOLE);
		maria_data[chan].ud = *user_data;
		maria_data[chan].ud.fdType = HANDLE_TYPE_COM;
		break;
	}
}

void maria_close(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data)
{
	int chan = user_data->fdidx;
	switch(chan)
	{
	case RS232_FD_IDX:
		// TODO:
		break;
	}
}

// =====================================================================================================
// STATIC FUNCTIONS
// =====================================================================================================

// =====================================================================================================
// DEBUG & PRINT FUNCTIONS
// =====================================================================================================

