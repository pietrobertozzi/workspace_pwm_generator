#include <stdlib.h>

#ifndef _MARIA_H_
#define _MARIA_H_

#if defined(ARDUINO)
#include "APP_config.h"
#endif /* ARDUINO */

#include "maria_info.h"
#include "maria_parser.h"
#include "maria_utils.h"

#if defined(SDCC_hc08)
#   ifndef SDCC_STACK_AUTO
#       error "to compile maria_monitor with SDCC, you must enable --stack-auto option !!"
#   endif
#endif

#if defined ( TARGET_ARCH_I386 )
typedef unsigned long PTRSTORAGE;
#elif defined( TARGET_ARCH_X64 )
typedef unsigned long PTRSTORAGE;
#elif defined ( TARGET_ARCH_AVR )
typedef unsigned short PTRSTORAGE;
#elif defined ( TARGET_ARCH_ARM )
typedef unsigned long PTRSTORAGE;
#elif defined ( TARGET_ARCH_ARM7 )
typedef unsigned long PTRSTORAGE;
#elif defined ( TARGET_ARCH_HC08 )
typedef unsigned short PTRSTORAGE;
#elif defined ( TARGET_ARCH_PIC32 )
typedef unsigned long PTRSTORAGE;
#elif defined ( TARGET_ARCH_MIPS )
typedef unsigned long PTRSTORAGE;
#elif defined ( TARGET_ARCH_ESP32 )
typedef unsigned long PTRSTORAGE;
#else
#error "Undefined Target architecture"
#endif

extern volatile int MARIA_ADD_CR;
#if defined(MARIA_ANS)
extern int ANS;
#endif
#if defined MARIA_TUNNEL
// tunnel is open by default when MARIA_TUNNEL is defined
// application sw can close the tunnel setting maria_tunnel_on to 0 (zero)
extern unsigned char maria_tunnel_on;
#endif

/* --------------------------------------------------------------------- */
/* API FUNCTION */
/* --------------------------------------------------------------------- */
void maria_init(void);
void maria_finish(void);
int maria_parser(int *fd_r, int *fd_w, MARIA_USER_DATA *user_data);

int maria_printf(const char *format, ...);
int maria_fprintf(int fd_idx,const char *format, ...);
#define maria_printf_0 maria_printf
#define maria_printf_1 maria_printf
#define maria_printf_2 maria_printf
#define maria_printf_3 maria_printf
#define maria_printf_4 maria_printf
#define maria_printf_5 maria_printf
#define maria_printf_6 maria_printf
#define maria_printf_7 maria_printf
#define maria_printf_8 maria_printf

/* --------------------------------------------------------------------- */
/* FUNCTIONS for memory stored script                                    */
/* --------------------------------------------------------------------- */
#if MSCR_SCRIPT_LINES > 0
int mscrInit(void);
int mscrRun(int startline);
int mscrGoto(int label);
extern char   *mscrBuf;
extern char MSCR_BUF[MSCR_SCRIPT_LINES*32];
#endif

/* --------------------------------------------------------------------- */
/* FUNCTIONS DRIVER MUST PROVIDE for monitor                             */
/* --------------------------------------------------------------------- */
int maria_read(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data);
int maria_write(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data);
void maria_close(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data);
void maria_open(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data);

#endif /* _MARIA_H_ */
