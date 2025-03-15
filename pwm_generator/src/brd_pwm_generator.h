/**
* @file brd_pwm_generator.h
*/

#ifndef _BRD_PWM_GENERATOR_H_
#define _BRD_PWM_GENERATOR_H_

#include "main.h"

#if defined(LINK_MARIA)
#include <maria.h>
#else
#if defined(USE_ARD_LIBC)
#include <ard_libc.h>
#else
#define ARD_memset    memset
#define ARD_memcpy    memcpy
#define ARD_memcpy_P  memcpy_P
#define ARD_sprintf   sprintf
#define ARD_strcat    strcat
#define ARD_strcmp    strcmp
#define ARD_strcpy    strcpy
#define ARD_strlen    strlen
#define ARD_strncpy	  strncpy
#define ARD_strstr    strstr
#define ARD_strtol    strtol
#define ARD_strtoul   strtoul
#define ARD_vsprintf  vsprintf
#define ARD_vsnprintf vsnprintf
#endif /* USE_ARD_LIBC */
#endif /* LINK_MARIA */

// see main.c automatically generated code
enum {
	MAINTASK_INIT_PHASE_1,
	MAINTASK_INIT_PHASE_INIT,
	MAINTASK_INIT_PHASE_SYSINIT,
	MAINTASK_INIT_PHASE_2,
};

#define MAIN_TASK_TIME_MS							1

//=================================================================================================
// TYPEDEFS & DEFINES
//=================================================================================================

#include "symbols.h"

#if defined(LINK_MARIA) || defined(MARIA_PRINTF_IS_PRINTF)
#if defined(BOARD_PWM_GENERATOR_1_0)
#define BP(fmt, ...) { sprintf(g_Msg, "BREAKP %s:%d " fmt, __func__, __LINE__, ##__VA_ARGS__ ); maria_printf(g_Msg); }
#define LOG(fmt, ...) { if(!frcLogOff) { sprintf(g_Msg, "%s " fmt, __func__, ##__VA_ARGS__ ); maria_printf(g_Msg); }; }
#else
#define BP(fmt, ...) { sprintf(g_Msg, "BREAKP %s:%d " fmt, __func__, __LINE__, ##__VA_ARGS__ ); HAL_UART_Transmit(&huart1, (uint8_t *)g_Msg, ARD_strlen(g_Msg), HAL_MAX_DELAY); }
#define LOG(fmt, ...) { sprintf(g_Msg, "%s " fmt, __func__, ##__VA_ARGS__ ); HAL_UART_Transmit(&huart1, (uint8_t *)g_Msg, ARD_strlen(g_Msg), HAL_MAX_DELAY); }
#warning "Unknown board!"
#endif	// BOARD_XX
#endif /* LINK_MARIA */

static inline void BPLED(int x)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static inline void BPLEDTOGGLE(void)
{
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
}

#define TOT_PWM           2

typedef struct BOARD_STATEtag {
#if defined(BOARD_PWM_GENERATOR_1_0)
	uint32_t             					uart_CONSOLE_RxIntCounter;	// contatori che vengono incrementati con gli interrupt

	// VARIABILI D'APPOGGIO 
	uint8_t                     			uart_CONSOLE_RxChar;
	uint8_t              					uart_CONSOLE_RxBuf[128];
	uint8_t                     			uart_CONSOLE_RxBufLen;
	int                         			uart_CONSOLE_LineReady;

	uint32_t                                tempoDaStartup;

    int                                     PWM[TOT_PWM];
#endif /* BOARD_XX */

} BOARD_STATE;

//=================================================================================================
// FUNCTION PROTOTYPES
//=================================================================================================
int mainTaskInit(int initPhase);
void mainTaskLoopIteration(void);
int mainTaskEnd(void);

void initBoardStat(void);

//=================================================================================================
// EXPORTED SYMBOLS
//=================================================================================================
extern BOARD_STATE  				g_brdStat;

#define ARDGMSG_SIZE				128
extern char        					g_Msg[ARDGMSG_SIZE];

extern UART_HandleTypeDef 			huart_CONSOLE;
extern TIM_HandleTypeDef            htim_PWM;


#endif /* _BRD_PWM_GENERATOR_H_ */

