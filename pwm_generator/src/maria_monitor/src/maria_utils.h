#ifndef MARIA_UTILS_H
#define MARIA_UTILS_H

#if defined(ARDUINO)
#include "APP_config.h"
#endif /* ARDUINO */

typedef struct SRECINFOtag {
    uint32_t   address;
    int        dataSize;
} SRECINFO;

int infoOn(void);
int infoOff(void);

#if !defined TARGET_ARCH_HC08
int dump(unsigned long address, int size, int memory_space);
int dumplong(unsigned long address, int len, int memory_space);
int dumpulong(unsigned long address, int size, int memory_space);
int dumppkt(unsigned long address, int len, char *sprefix);
#else
int dump(int address, int size, int memory_space);
#endif
int dumpStack(int stackEntity);
int fillMemory(unsigned char *address, char *hexstring);
int memfillbyte(unsigned char *p, int size, unsigned char v, int incr);
int memfillshort(unsigned short *p, int size, unsigned short v, int incr);
int memfilllong(unsigned long *p, int size, unsigned long v, int incr);

int retval(int i0, int i1);
int Unknown_symbol(void);

#if defined TARGET_ARCH_AVR
int dumpFlash(int address, int size);
#endif

int resolveAddr(GENPTR addr, char *symString);
int showSym(char *symbol);

int logout(void);
int cont(void);

int testAlive(void);

#endif /* MARIA_UTILS_H */
