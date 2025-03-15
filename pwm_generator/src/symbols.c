/**
* @file symbols.c
*/

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

/**
 * @name setPwm
 * @brief controlla il duty cycle del canale PWM indicato
 * @param chan: numero di canale del PWM [0..TOT_PWM-1]
 * @param rate: valore efficace del PWM [0..999]
 * @return rate:
 */
int setPwm(int chan, int rate)
{
	int ch[TOT_PWM] = { TIM_CHANNEL_1, TIM_CHANNEL_2};

    if(chan < 0) chan = -chan;
    chan %= TOT_PWM;
    if(rate < 0) rate = -rate;

    if (chan == 2)
    {
        // funzione di enable per gli n_channel
        HAL_TIMEx_PWMN_Start(&htim_PWM, ch[chan]);
    }
    else
    {
        HAL_TIM_PWM_Start(&htim_PWM, ch[chan]);
    }

	rate %= (__HAL_TIM_GET_AUTORELOAD(&htim_PWM));

	__HAL_TIM_SET_COMPARE(&htim_PWM, ch[chan], rate);
    g_brdStat.PWM[chan] = rate;

	return rate;
}

// =====================================================================================================
// STATIC FUNCTIONS
// =====================================================================================================

// =====================================================================================================
// DEBUG & PRINT FUNCTIONS
// =====================================================================================================

/**
 * @name showStat
 * @brief mostra il valore delle variabili di stato del FW
 * @param menu: indice di menu che identifica la visualizzazione, che puo' essere parziale
 * @param submenu: indice del sottomenu (se presente) che identifica la visualizzazione
 * @param prefix0: stringa di prefisso per ogni linea visualizzata, puo' essere usata per il controllo dell'indetazione
 * @return il codice numerico della visualizzazione selezionata
 */
int showStat(int menu, int submenu, char *prefix0)
{
    char           		*p0 = prefix0;
	char           		p1[8];
	int                 i;

    if(prefix0 == NULL) p0 = "";
	ARD_sprintf(p1, "%s     ", p0);

	submenu = (submenu < 0) ? 0 : submenu;

	if(menu == 0)
	{
		maria_printf("%s showStat 0   => show this menu\n", p0);
		maria_printf("%s showStat 1   => show SYSTEM status\n", p0);
	}
    
    if(menu == 1)
    {
   		maria_printf("%s IRQ counters:\n", p0);
		maria_printf("%s uart_CONSOLE_RxIntCounter:      %lu\n", p1, g_brdStat.uart_CONSOLE_RxIntCounter);
		maria_printf("%s sysTick:                        %lu\n", p1, HAL_GetTick());
        maria_printf("%s tempoDaStartup:                 %lu\n", p1, g_brdStat.tempoDaStartup);
        maria_printf("\n");
        maria_printf("%s PWM:\n", p0);
        for(i=0; i<TOT_PWM; i++)
        {
            maria_printf("%s PWM[%d] %d\n", p1, i, g_brdStat.PWM[i]);
        }
        maria_printf("\n");
    }

    return menu;
}
