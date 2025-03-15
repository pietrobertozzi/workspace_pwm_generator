/**
* @file drv_maria.h
*/

#ifndef COMMON_DRIVERS_DRV_MARIA_H_
#define COMMON_DRIVERS_DRV_MARIA_H_

#include <stdint.h>

//=================================================================================================
// TYPEDEFS & DEFINES
//=================================================================================================
#define RS232_FD_IDX 0  /* this must be 0 !! */

#define MARIA_MAX_FDS 1

#pragma pack(1)
typedef struct MARIA_USER_DATAtag {
	/* ##################################### common part of MARIA_USER_DATA */
	int  					fdType;				/* defines which type of FD we are handling: SOCKET, FILE .... */
	int         			fdTag, fdInstance;  /* epoll() parameters */
	uint16_t                mariaFlags;			/* parser mode flags see MARIA_FLAGS_... */

	/* ##################################### user part of MARIA_USER_DATA */
	int              		fdidx;
	char                    *pWriteBuffer;
	char                    *pReadBuffer;
} MARIA_USER_DATA;
#pragma pack()

//=================================================================================================
// FUNCTION PROTOTYPES
//=================================================================================================

//=================================================================================================
// EXPORTED SYMBOLS
//=================================================================================================

#endif /* COMMON_DRIVERS_DRV_MARIA_H_ */
