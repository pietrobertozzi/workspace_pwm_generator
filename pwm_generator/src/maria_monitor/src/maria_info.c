#include "maria_info.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if !defined(NO_MARIA_INFO)

int infoGlobOn=0;

int infoOn(void)
{
	infoGlobOn = TRUE;
	return infoGlobOn;
}

int infoOff(void)
{
	infoGlobOn = FALSE;
	return infoGlobOn;
}

#endif
