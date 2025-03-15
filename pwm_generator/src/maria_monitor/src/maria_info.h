#ifndef _MARIA_INFO_H_
#define _MARIA_INFO_H_

#if !defined(NO_MARIA_INFO)

#include <stdarg.h>

#if defined(ARDUINO)
#include "APP_config.h"
#endif /* ARDUINO */

extern int infoGlobOn;

#ifdef __cplusplus
#define INFO_EXTERN extern "C"
extern "C" {
#else
#define INFO_EXTERN
#endif

int infoLog(unsigned short mask, unsigned short enabledMask, char *file, int line, char *fmtstring, ...);

#if defined ( TARGET_ARCH_AVR )
#define MAX_INFO_LEN 80
#elif defined ( TARGET_ARCH_I386 )
#define MAX_INFO_LEN 240
#elif defined( TARGET_ARCH_X64 )
#define MAX_INFO_LEN 240
#elif defined ( TARGET_ARCH_ARM )
#define MAX_INFO_LEN 128
#elif defined ( TARGET_ARCH_ARM7 )
#define MAX_INFO_LEN 80
#elif defined ( TARGET_ARCH_HC08 )
#define MAX_INFO_LEN 80
#elif defined ( TARGET_ARCH_PIC32 )
#define MAX_INFO_LEN 80
#elif defined ( TARGET_ARCH_MIPS )
#define MAX_INFO_LEN 80
#elif defined ( TARGET_ARCH_ESP32 )
#define MAX_INFO_LEN 128
#else
	 #error "Unknown target architecture"
#endif


#define INFO_VAR(MODULE) info ## MODULE ## On

#define INFO_USE(MODULE)						      		\
    INFO_EXTERN int infoOn ## MODULE(void);				\
    INFO_EXTERN int infoOff ## MODULE(void);				\
    INFO_EXTERN int infoSet ## MODULE(int newValue);		\
    extern int								                \
    INFO_VAR (MODULE)

#define INFO_DEFINE(MODULE)						      \
    INFO_USE(MODULE);							      \
    INFO_EXTERN int infoOn ## MODULE (void)          \
    {									              \
		INFO_VAR(MODULE) = 0xffff;					  \
		return INFO_VAR(MODULE);					  \
    }									              \
    INFO_EXTERN int infoOff ## MODULE (void)         \
	{											  \
		INFO_VAR(MODULE) = (1<<0xE);			  \
		return INFO_VAR(MODULE);				  \
    }									              \
    INFO_EXTERN int infoSet ## MODULE (int newValue) \
    {									              \
		 INFO_VAR(MODULE) = newValue ;	  \
		return INFO_VAR(MODULE);					  \
    }									              \
	int INFO_VAR(MODULE) = 0


#define infoX(MODULE, X, FORMAT...)					  \
    do {								              \
		if ((info ## MODULE ## On & (1 << (X))) || (X == 0xE) || (X == 0xD)) { \
			 infoLog((1 << X), info ## MODULE ## On, (char *)__func__, __LINE__, FORMAT); \
		}								              \
    } while (0)

#define info0(MODULE, FORMAT...) infoX(MODULE, 0x0, FORMAT)
#define info1(MODULE, FORMAT...) infoX(MODULE, 0x1, FORMAT)
#define info2(MODULE, FORMAT...) infoX(MODULE, 0x2, FORMAT)
#define info3(MODULE, FORMAT...) infoX(MODULE, 0x3, FORMAT)
#define info4(MODULE, FORMAT...) infoX(MODULE, 0x4, FORMAT)
#define info5(MODULE, FORMAT...) infoX(MODULE, 0x5, FORMAT)
#define info6(MODULE, FORMAT...) infoX(MODULE, 0x6, FORMAT)
#define info7(MODULE, FORMAT...) infoX(MODULE, 0x7, FORMAT)
#define info8(MODULE, FORMAT...) infoX(MODULE, 0x8, FORMAT)
#define info9(MODULE, FORMAT...) infoX(MODULE, 0x9, FORMAT)
#define infoA(MODULE, FORMAT...) infoX(MODULE, 0xA, FORMAT)
#define infoB(MODULE, FORMAT...) infoX(MODULE, 0xB, FORMAT)
#define infoC(MODULE, FORMAT...) infoX(MODULE, 0xC, FORMAT)
#define infoD(MODULE, FORMAT...) infoX(MODULE, 0xD, FORMAT)
#define infoE(MODULE, FORMAT...) infoX(MODULE, 0xE, FORMAT)
#define infoF(MODULE, FORMAT...) infoX(MODULE, 0xF, FORMAT)

#ifdef __cplusplus
}
#endif

#endif              /* !defined(NO_MARIA_INFO) */
#endif				/* ifndef maria_info_h */
