#ifndef _MARIA_PARSER_H_
#define _MARIA_PARSER_H_

#if defined(ARDUINO)
#include "APP_config.h"
#endif /* ARDUINO */

#include <drv_maria.h>

#if defined(USE_ARD_LIBC)
#include <ard_libc.h>
#else
#include <string.h>
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
#define ARD_memcmp    memcmp
#define ARD_min(a,b) ((a) < (b) ? (a) : (b))
#define ARD_max(a,b) ((a) > (b) ? (a) : (b))
#endif

#if defined TARGET_ARCH_AVR
#include <inttypes.h>
#include <avr/pgmspace.h>
#endif


#if defined (TARGET_ARCH_I386)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 16  /* must be multiple of 16 */
#define MARIA_LINE_SIZE  128 // needed for cgi commands
#elif defined( TARGET_ARCH_X64 )
#define MARIA_SYMBOL_SIZE 48
#define MARIA_MAX_ARGC 16  /* must be multiple of 16 */
#define MARIA_LINE_SIZE  128 // needed for cgi commands
#elif defined (TARGET_ARCH_AVR)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  64
#elif defined (TARGET_ARCH_ARM)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  128 // needed for cgi commands
#elif defined (TARGET_ARCH_ARM7)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  128  /* needed for srec download through  maria monitor */
#elif defined (TARGET_ARCH_HC08)
#define MARIA_SYMBOL_SIZE 16
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  64
#elif defined (TARGET_ARCH_PIC32)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  128
#elif defined(TARGET_ARCH_MIPS)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 16  /* must be multiple of 16 */
#define MARIA_LINE_SIZE  128 // needed for cgi commands
#elif defined (TARGET_ARCH_ESP32)
#define MARIA_SYMBOL_SIZE 32
#define MARIA_MAX_ARGC 8
#define MARIA_LINE_SIZE  128 // needed for cgi commands
#else

	   #error "Unknown targetarchitecture"
#endif

#define MARIA_TYPE_INT 0
#define MARIA_TYPE_STRING 1
#define MARIA_TYPE_CHAR 2
#define MARIA_TYPE_SHORT 3
#define MARIA_TYPE_SYMBOL 4

#if !defined(MARIA_MAX_FDS)     /* drv_maria.h is the right file to define this !!!! */
#error "MARIA_MAX_FDS not defined"
#endif

#define MARIA_DO_NOTHING 0
#define MARIA_PARSE_THIS 1

#if defined   (TARGET_ARCH_I386)
typedef void *GENPTR;
#elif defined( TARGET_ARCH_X64 )
typedef void *GENPTR;
#elif defined (TARGET_ARCH_AVR)
typedef char *GENPTR;
#elif defined (TARGET_ARCH_ARM)
typedef void *GENPTR;
#elif defined (TARGET_ARCH_ARM7)
typedef void *GENPTR;
#elif defined (TARGET_ARCH_HC08)
typedef const int (*GENPTR)(void);
#elif defined (TARGET_ARCH_PIC32)
typedef void *GENPTR;
#elif defined (TARGET_ARCH_MIPS)
typedef void *GENPTR;
#elif defined (TARGET_ARCH_ESP32)
typedef void *GENPTR;
#else
	  #error "Unknown target architecture"
#endif

typedef struct MARIA_PARSED_LINEtag {
	char sLine[MARIA_LINE_SIZE];
	int argc;
	char *argv[MARIA_MAX_ARGC];
	union {
#if defined (TARGET_ARCH_I386)
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined( TARGET_ARCH_X64 )
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_AVR
		int16_t lValue[MARIA_MAX_ARGC];
		uint16_t ulValue[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_ARM
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_ARM7
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_HC08
		int lValue[MARIA_MAX_ARGC];
		unsigned int[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_PIC32
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined (TARGET_ARCH_MIPS)
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#elif defined TARGET_ARCH_ESP32
		long lValue[MARIA_MAX_ARGC];
		unsigned long ulValue[MARIA_MAX_ARGC];
#else
	 #error "Unknown target architecture"
#endif
	};
	char type[MARIA_MAX_ARGC];
} MARIA_PARSED_LINE;

#define HANDLE_TYPE_COM  		0
#define HANDLE_TYPE_SOCK 		1  /* for connectionless (DGRAM) socket or accepted STREAM socket*/
#define HANDLE_TYPE_SOCK_LISTEN	2  /* for listening common stream socket */
#define HANDLE_TYPE_FILE		3  /* file stored script executing */
#define HANDLE_TYPE_MEMORY		4  /* memory stored script executing */

#define MARIA_SYMTYPE_FUNCTION 0
#define MARIA_SYMTYPE_DATA 1

typedef struct MARIA_TABLE_RECORDtag {
	char symbol[MARIA_SYMBOL_SIZE];
	int symtype;
	GENPTR symaddr;
#if defined(MARIA_PARNUM_CHECK)
	int len;
#endif
} MARIA_TABLE_RECORD;

#if defined (TARGET_ARCH_I386)
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR
#  endif
#elif defined( TARGET_ARCH_X64 )
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR
#  endif
#elif defined TARGET_ARCH_AVR
#  ifdef MARIA_TABLE_ON_FLASH
#        define MARIA_TABLE_ATTR const PROGMEM
#  else
#        define MARIA_TABLE_ATTR
#  endif
#elif defined TARGET_ARCH_ARM
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR
#  endif
#elif defined TARGET_ARCH_ARM7
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR const
#        define NO_MARIA_TABLE_INIT
#  endif
#elif defined TARGET_ARCH_HC08
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR const
#  endif
#  define NO_MARIA_TABLE_INIT
#elif defined TARGET_ARCH_PIC32
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR const
#        define NO_MARIA_TABLE_INIT
#  endif
#elif defined (TARGET_ARCH_MIPS)
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR
#  endif
#elif defined TARGET_ARCH_ESP32
#  ifdef MARIA_TABLE_ON_FLASH
#        error "Wrong Architecture for maria_table on flash"
#  else
#        define MARIA_TABLE_ATTR
#  endif
#else
	 #error "Unknown target architecture"
#endif

extern MARIA_TABLE_ATTR MARIA_TABLE_RECORD maria_table[];

#if defined ( TARGET_ARCH_I386 )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#elif defined( TARGET_ARCH_X64 )
#elif defined ( TARGET_ARCH_AVR )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(int16_t))
#elif defined ( TARGET_ARCH_ARM )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#elif defined ( TARGET_ARCH_ARM7 )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#elif defined ( TARGET_ARCH_HC08 )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(char))
#elif defined ( TARGET_ARCH_PIC32 )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#elif defined ( TARGET_ARCH_MIPS )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#elif defined ( TARGET_ARCH_ESP32 )
#define MARIA_MAX_CALLSTACK (MARIA_MAX_ARGC * sizeof(long))
#else
	 #error "Unknown target architecture"
#endif

#if defined(__cplusplus)
#define MARIA_CDECL extern "C"
#else
#define MARIA_CDECL
#endif

#if defined ( TARGET_ARCH_I386 )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_X64 )
#pragma pack(1)
typedef	struct STACKSIMULtag {
	volatile long buf[MARIA_MAX_ARGC];
} STACKSIMUL;
#pragma pack()
typedef int (*MARIA_API_POINTER)(long p0, long p1, long p2, long p3,
		long p4, long p5, long p6, long p7,
		long p8, long p9, long p10, long p11,
		long p12, long p13, long p14, long p15
		);
#elif defined ( TARGET_ARCH_AVR )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_ARM )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_ARM7 )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_HC08 )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(int p1, int p2, int p3, int p4);
#elif defined ( TARGET_ARCH_PIC32 )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_MIPS )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(STACKSIMUL s);
#elif defined ( TARGET_ARCH_ESP32 )
typedef	struct STACKSIMULtag {
	volatile char buf[MARIA_MAX_CALLSTACK];
} STACKSIMUL;
MARIA_CDECL typedef int (*MARIA_API_POINTER)(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7);
#else
	 #error "Unknown target architecture"
#endif

int mariaOnlyLoop(void);
#if 0
void maria_prompt0(void); /* normal prompt */
void maria_prompt1(void); /* while breaked */
void maria_user_prompt(char *user_prompt);
#endif

#if !defined(NO_PARSER_PROTOFUN)
int maria_max_fd(void);
void maria_prompt(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data);
#endif /* NO_PARSER_PROTOFUN */

typedef struct MARIA_DATAtag {
	int  			r_fd;
	int  			w_fd;
	MARIA_USER_DATA ud;
} MARIA_DATA;


#if !defined(NO_PARSER_PROTODATA)
extern int returnFromBreakpoint;
extern int g_active_fd_index;
extern MARIA_DATA maria_data[MARIA_MAX_FDS];
#endif /* NO_PARSER_PROTODATA */

#define MARIA_FLAGS_NOPROMPT  0x1
#define MARIA_FLAGS_NORETVAL  0x2
#define MARIA_FLAGS_DUALPROTOCOL  (MARIA_FLAGS_NOPROMPT | MARIA_FLAGS_NORETVAL)

#if !defined(NO_PARSER_PROTOFUN)
int maria_parser(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data);
void maria_finish(void);
void maria_init(void);
#endif /* NO_PARSER_PROTOFUN */

#endif /* _MARIA_PARSER_H_ */
