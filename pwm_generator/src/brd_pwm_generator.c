/**
* @file brd_pwm_generator.c
*/

#include "brd_pwm_generator.h"

//======================================================================================================
// GLOBAL VARIABLES
//======================================================================================================
BOARD_STATE         		g_brdStat;
char        				g_Msg[ARDGMSG_SIZE];

//======================================================================================================
// STATIC PROTOTYPES & STATIC VARIABLES
//======================================================================================================
static inline void mainTaskIdle(void);

// ##############################################################################################
// INTERRUPT HANDLERS
// ##############################################################################################
/**
 * @name HAL_UART_RxCpltCallback:
 * @brief interrupt handler su evento di ricezione UART
 * 		la routine gestisce le ricezioni di ogni UART impiegata dal sistema, i dati ricevuti vengono inseriti nelle opportune
 *      macchine a stati che realizzano il livello di trasporto del protocollo associato al canale
 * @param huart: descrittore della periferica su cui e' avvenuta la ricezione
 * @return void
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart_CONSOLE.Instance)
	{
		g_brdStat.uart_CONSOLE_RxIntCounter ++;

		// store new data in buffer
		g_brdStat.uart_CONSOLE_RxBuf[g_brdStat.uart_CONSOLE_RxBufLen] = g_brdStat.uart_CONSOLE_RxChar;
		g_brdStat.uart_CONSOLE_RxBufLen ++;
		g_brdStat.uart_CONSOLE_RxBufLen %= sizeof(g_brdStat.uart_CONSOLE_RxBuf);

		if((g_brdStat.uart_CONSOLE_RxChar == '\n') || (g_brdStat.uart_CONSOLE_RxChar == '\r'))
		{
			g_brdStat.uart_CONSOLE_LineReady = 1;
		}

		// arm new receipt
		HAL_UART_Receive_IT(&huart_CONSOLE, &g_brdStat.uart_CONSOLE_RxChar, sizeof(g_brdStat.uart_CONSOLE_RxChar));
	}
}


//======================================================================================================
// IMPLEMENTATION
//======================================================================================================

/*
 * @name mainTaskInit
 * @brief Funzione di inizializzazione, la funzione mainTaskInit() viene chiamata direttamente da main() in ognuna delle 4 fasi della procedura proposta dal framework HAL
 * 			MAINTASK_INIT_PHASE_1
 *			MAINTASK_INIT_PHASE_INIT: inizializzazione console
 *			MAINTASK_INIT_PHASE_SYSINIT: esecuzione di eventuale script di inizializzazione
 * 			MAINTASK_INIT_PHASE_2: variabili globali e periferiche HW
 * @param initPhase codice della fase in esecuzione da main
 * @return il valore di ritorno e' sempre 0
 */
int mainTaskInit(int initPhase)
{
#if defined(LINK_MARIA)
	MARIA_USER_DATA   maria_user_data;
#endif /* LINK_MARIA */
	switch(initPhase)
	{
	case MAINTASK_INIT_PHASE_1:
		//***********************************************************************************************************
		break;
	case MAINTASK_INIT_PHASE_INIT:
		//***********************************************************************************************************
#if defined(LINK_MARIA)
		maria_init();
		ARD_memset(&maria_user_data, 0, sizeof(maria_user_data));
		maria_user_data.fdType = HANDLE_TYPE_COM;
		maria_user_data.fdidx = RS232_FD_IDX;
		maria_user_data.mariaFlags = 0;
		maria_open(NULL, NULL, (void *)(&maria_user_data));
#endif /* LINK_MARIA */

		break;
	case MAINTASK_INIT_PHASE_SYSINIT:
		//***********************************************************************************************************

		break;
	case MAINTASK_INIT_PHASE_2:

		initBoardStat();

		//***********************************************************************************************************
		// attiva la ricezione delle seriali
		HAL_UART_Receive_IT(&huart_CONSOLE, &g_brdStat.uart_CONSOLE_RxChar, sizeof(g_brdStat.uart_CONSOLE_RxChar));

		break;
    }
    
    return 0;
}

/**
 * @name mainTaskLoopIteration:
 * @brief funzione che realizza il singolo ciclo del main().
 * 		- tramite l'analisi del contatore di tick di sistema, la funzione chiama i task periodici con la cadenza desiderata
 *      quando nessuno dei task e' pronto per essere esguito la funzione passa il controllo a mainTaskIdle()
 * @param void:
 * @return void
 */
void mainTaskLoopIteration(void)
{
	static uint32_t 	lastServedTick = 0;		// last tick served in one of the periodic tasks
	static uint32_t 	lastTick = 0;			// last tick served in Idle Task
	uint32_t 			nowTick = HAL_GetTick();
    int                 i;                 

	if (nowTick == lastTick)
	{
		mainTaskIdle();
		return;
	}
	lastTick = nowTick;

	if ((nowTick - lastServedTick) >= MAIN_TASK_TIME_MS)
	{
		if((g_brdStat.tempoDaStartup % 500) == 0)
		{
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		}

		g_brdStat.tempoDaStartup += MAIN_TASK_TIME_MS;
		lastServedTick = nowTick;
	}

	// **************************************************************************
	// task 1 mS

	// **************************************************************************

	return;
}

/**
 * @name mainTaskEnd:
 * @brief funzione di fine applicazione, non viene mai chiamata in quanto il mainLoop non puo' terminare mai
 * 		la funzione chiude la console di debug
 * @param void
 * @return 0 se la funziona ha successo, codice di errore negativo se termina con errore
 */
int mainTaskEnd(void)
{
#if defined(LINK_MARIA)
	MARIA_USER_DATA   maria_user_data;

	ARD_memset(&maria_user_data, 0, sizeof(maria_user_data));
	maria_user_data.fdidx = RS232_FD_IDX;

	maria_close(NULL, NULL, (void *)(&maria_user_data));
#endif /* LINK_MARIA */

	return 0;
}

/**
 * @name todo:
 * @brief todo:
 * @param todo:
 * @param todo:
 * @return todo:
 */
void initBoardStat(void)
{
	ARD_memset(&g_brdStat, 0, sizeof(g_brdStat));
}

// =====================================================================================================
// STATIC FUNCTIONS
// =====================================================================================================

/**
 * @name mainTaskIdle:
 * @brief funzione di idle di sistema. questa funzione viene chiamata quando il sistema e' lòibero dall' esecuzione di task periodici
 * 			serve quindi per eseguire le attivita' che richiedono una risposta pronta a controllo di programma.
 * 			contiene quindi le chiamate agli strati dei protocolli di comunicazione superiori a quello di trasporto.
 *  		in sostanza compito del mainTaskIdle e' quello di elaborare frames gia' assemblate e processate
 * 			dallo strato di trasporto per ogni canale di comunicazione.
 * @param void:
 * @return void:
 */
static inline void mainTaskIdle(void)
{
	if(g_brdStat.uart_CONSOLE_LineReady)
	{
#if defined(LINK_MARIA)
		if(g_brdStat.uart_CONSOLE_RxBufLen)
		{
			maria_parser(&(maria_data[RS232_FD_IDX].r_fd), &(maria_data[RS232_FD_IDX].w_fd), &(maria_data[RS232_FD_IDX].ud));
		}
#endif /* LINK_MARIA */
	}
}

// =====================================================================================================
// DEBUG & PRINT FUNCTIONS
// =====================================================================================================
