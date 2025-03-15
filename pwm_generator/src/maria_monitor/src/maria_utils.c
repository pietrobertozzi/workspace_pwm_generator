#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#if defined ( TARGET_ARCH_AVR )
/* todo: spostare quest' include in un file piu' specifico per avr */
#include <avr/pgmspace.h>
#endif

#if defined ( TARGET_ARCH_ARM7 )
#endif

#if defined ( TARGET_ARCH_HC08 )
#include <mylibc.h>
#endif

#include "maria.h"
#include "maria_parser.h"
#include "maria_utils.h"

#if !defined(NO_MARIA_INFO)
INFO_DEFINE(Maria);
#endif

#define MEMSPACE_RAM 0
#define MEMSPACE_FLASH 1
#define START_CODE 'S'

#define MAX_SREC_DATA  256
#define MAX_LINE_LEN   500

/* general purpose RAM buffer */
#if defined ( TARGET_ARCH_AVR )
#elif defined ( TARGET_ARCH_I386 )
volatile unsigned char GPM[1024];
#elif defined( TARGET_ARCH_X64 )
volatile unsigned char GPM[1024];
#elif defined ( TARGET_ARCH_ARM )
volatile unsigned char GPM[512];
#elif defined ( TARGET_ARCH_ARM7 )
volatile unsigned char GPM[256];
#elif defined ( TARGET_ARCH_HC08 )
volatile unsigned char GPM[128];
#elif defined ( TARGET_ARCH_PIC32 )
volatile unsigned char GPM[512];
#elif defined ( TARGET_ARCH_MIPS )
volatile unsigned char GPM[1536];
#elif defined ( TARGET_ARCH_ESP32 )
volatile unsigned char GPM[512];
#else
#error "Unknown target architecture"
#endif

#ifdef MARIA_TABLE_ON_FLASH
extern MARIA_TABLE_RECORD s_maria_table_record_copy;
#endif

#if defined MARIA_TUNNEL
// tunnel is open by default when MARIA_TUNNEL is defined
// application sw can close the tunnel setting maria_tunnel_on to 0 (zero)
unsigned char maria_tunnel_on = 1;
#endif

volatile int MARIA_ADD_CR = 0;
#if defined(MARIA_ANS)
int ANS = 0;
#endif

#if MSCR_SCRIPT_LINES > 0
char   *mscrBuf = NULL;
#endif


/* define NO_MARIA_DUMP in project
   to get rid of this implementation and save code space */
#if !defined(NO_MARIA_DUMP)

typedef enum SREC_ERR_TAG {
	SREC_NO_ERR     	= 0,
	SREC_CHK_ERR		,
	SREC_LEN_ERR		,
	SREC_WRONG_SREC_ERR	,
} SREC_ERR;

static int ISPRINT(int c)
{
	if(c<' ') return 0;
	if(c>'~') return 0;
	return 1;
}

static int hexnibble2bin(char c)
{
	if(c>'f') return 0;
	if(c>='a') return (c-'a'+10);
	if(c>='A') return (c-'A'+10);
	if(c>='0') return (c-'0');
	return 0;
}

static int hexbyte2bin(char *s)
{
	volatile int v;

	v=hexnibble2bin(s[0]);
	v<<=4;
	v|=hexnibble2bin(s[1]);
	return v;
}

int srecStore(char *s, unsigned long forced_address, SRECINFO *pSrecInfo )
{
	int           slen, i;
	char          *p=NULL;
	unsigned char *pDat=NULL;
	unsigned int  chk = 0;
	unsigned int  b;

	int            type;
	unsigned long  address;
	int            len;

	slen = ARD_strlen(s);

	p=s;
	if((*p)!=START_CODE) {
		maria_printf_0("srecParse SREC_WRONG_SREC_ERR\n");
		return SREC_WRONG_SREC_ERR;
	}

	p++;

	type=hexnibble2bin(*p);
	p++;

	len = hexbyte2bin(p);
	chk += len;
	p+=2;

	if(slen != (2 * len + 4)) {
		maria_printf_0("srecParse SREC_LEN_ERR\n");
		return SREC_LEN_ERR;
	}

	if(pSrecInfo != NULL)
	{
		pSrecInfo->dataSize = len;
	}

	address = 0;
	switch(type) {
		case 3: /* data sequence */
		case 7: /* end of block */
			b = hexbyte2bin(p);
			address |= b; chk += b; p += 2;
			address <<= 8;
			len --;
			/* no break here !!!! */
		case 2: /* data sequence */
		case 8: /* end of block */
			b = hexbyte2bin(p);
			address |= b; chk += b; p += 2;
			address <<= 8;
			len --;
			/* no break here !!!! */
		case 0: /* header */
		case 1: /* data sequence */
		case 5: /* record count */
		case 9: /* end of block */
			b = hexbyte2bin(p);
			address |= b; chk += b; p += 2;
			address <<= 8;
			len --;
			b = hexbyte2bin(p);
			address |= b; chk += b; p += 2;
			len --;
		break;

		default:
			maria_printf_0("srecParse SREC_WRONG_SREC_ERR\n");
			return SREC_WRONG_SREC_ERR;
		break;
	}

	if(pSrecInfo != NULL)
	{
		pSrecInfo->address = address;
	}

#if defined(TARGET_ARCH_X64)
	if(forced_address & 0xFFFFFFFFULL)
		pDat = (unsigned char *)(forced_address);
	else
	{
		/* forced_address contains the most significative 32 bits of the destination address */
		address &= 0xFFFFFFFFULL;
		address |= (forced_address & 0xFFFFFFFF00000000ULL);
		pDat = (unsigned char *)(address);
	}
#else
	if(forced_address)
		pDat = (unsigned char *)(forced_address);
	else
		pDat = (unsigned char *)(address);
#endif

	for(i=0; i < ( len - 1 ); i++) {
		b = hexbyte2bin(p);
		pDat[i] = b;
		chk += b;
		p += 2;
	}

	len --;
	b = hexbyte2bin(p);
	chk += b;

	if((chk & 0xFF) != 0xFF) {
		maria_printf_1("srecParse SREC_SREC_CHK_ERR: ignored\n");
	}

	return SREC_NO_ERR;
}

#if !defined TARGET_ARCH_HC08
int dump(unsigned long address, int size, int memory_space)
{
	unsigned char *p;
	unsigned char pval;
	int i, x = 0;
	char sCharDump[20];

#if !defined(NO_MARIA_INFO)
	info1(Maria,"address=0x%X size=%d",address,size);
#endif

	if(size==0) size=16*10;

	/* ensure address is multiple of 4 */
	p=(unsigned char *)(address & (~(0x3)));

	maria_printf_0("\n");

	for(i=0;i<size;i++)
	{
		switch(memory_space)
		{
		case MEMSPACE_FLASH:
#if defined TARGET_ARCH_AVR
			pval = pgm_read_byte_near((unsigned int)p);
#else
			pval = *p;
#endif
			break;
		default:
		case MEMSPACE_RAM:
			pval = *p;
			break;
		}

		if(!(i%16))
		{
			maria_printf_1("0x%04X:  ",p);
			x=0;
		}

		if(p < (unsigned char *)address)
		{
			/* skip filler bytes */
			maria_printf_0("  ");
		}
		else
		{
			maria_printf_1("%02x",pval);
		}

		if(ISPRINT(pval))
		{
			sCharDump[x]=pval;
		}
		else
		{
			sCharDump[x]='.';
		}
		sCharDump[x+1]=0;
		x++;

		if((i%2)==1)
		{
			maria_printf_0(" ");
		}

		if(((i%16)==15)||(i==(size-1)))
		{
			maria_printf_0(" * ");
			maria_printf_0(sCharDump);
			maria_printf_0("\n");
		}

		p++;
	}
	return 0;
}
#else /* #if !defined TARGET_ARCH_HC08 */

/* thin dump() release for hc08 */
int dump(int address, int size, int memory_space)
{
	unsigned char *p;
	unsigned char pval;
	int i, x = 0;
	char sCharDump[20];

#if !defined(NO_MARIA_INFO)
	info1(Maria,"address=0x%X size=%d",address,size);
#endif

	if(size==0) size=16*4;
	p=(unsigned char *)(address);

	maria_printf_0("\n");

	for(i=0;i<size;i++)
	{
		pval = *p;

		if(!(i%16))
		{
			maria_printf_1("0x%04X:  ",p);
			x=0;
		}

		maria_printf_1("%02x",pval);

		if(isprint(pval))
		{
			sCharDump[x]=pval;
		}
		else
		{
			sCharDump[x]='.';
		}
		sCharDump[x+1]=0;
		x++;

		if((i%2)==1)
		{
			maria_printf_0(" ");
		}

		if(((i%16)==15)||(i==(size-1)))
		{
			maria_printf_0(" * ");
			maria_printf_0(sCharDump);
			maria_printf_0("\n");
		}

		p++;
	}
	return 0;
}
#endif

#endif /* NO_MARIA_DUMP */


#if !defined TARGET_ARCH_HC08

int dumpulong(unsigned long address, int size, int memory_space)
{
	int            i;
	unsigned long  *p;
	unsigned long  pval;

	if(size==0) size = 16;
	/* ensure address is multiple of 4 */
	p=(unsigned long *)(address & (~(0x3)));

	maria_printf_0("\n");

	for(i=0;i<size;i++)
	{
		switch(memory_space)
		{
		case MEMSPACE_FLASH:
#if defined TARGET_ARCH_AVR
			maria_printf_0("utility is not available in this architecture\n");
			return 0;
#else
			pval = *p;
#endif
			break;
		default:
		case MEMSPACE_RAM:
			pval = *p;
			break;
		}

		maria_printf_1("0x%04X:  0x%08X\n", p, pval);
		p++;
	}
	return 0;
}
int dumplong(unsigned long address, int size, int memory_space)
{
	int            		i;
	long int 			*p;
	long int 			pval;
	if(size==0) size = 16;
	p=(long int *)(address & (~(0x3)));
	maria_printf_0("\n");
	for(i=0;i<size;i++)
	{
		switch(memory_space)
		{
		case MEMSPACE_FLASH:
#if defined TARGET_ARCH_AVR
			maria_printf_0("utility is not available in this architecture\n");
			return 0;
#else
			pval = *p;
#endif
			break;
		default:
		case MEMSPACE_RAM:
			pval = *p;
			break;
		}
		maria_printf_1("0x%04X:  %ld\n", p, pval);
		p++;
	}
	return 0;
}
#endif

#if !defined TARGET_ARCH_HC08
int memfillbyte(unsigned char *p, int size, unsigned char v, int incr)
{
	int i;

	if(size == 0) size = 16;

	for (i = 0; i < size; i++)
	{
		*p = v;
		p++;
		v += incr;
	}
	return size;
}

int memfillshort(unsigned short *p, int size, unsigned short v, int incr)
{
	int i;

	if(size == 0) size = 16;

	for (i = 0; i < size; i++)
	{
		*p = v;
		p++;
		v += incr;
	}
	return size;
}

int memfilllong(unsigned long *p, int size, unsigned long v, int incr)
{
	int i;

	if(size == 0) size = 16;

	for (i = 0; i < size; i++)
	{
		*p = v;
		p++;
		v += incr;
	}
	return size;
}
#endif

#if defined TARGET_ARCH_AVR
int dumpFlash(int address, int size)
{
	return dump(address, size, MEMSPACE_FLASH);
}
#endif /* TARGET_ARCH_AVR */

#if defined( TARGET_ARCH_I386 ) || defined( TARGET_ARCH_X64 )
int resolveAddr(GENPTR addr, char *symString)
{
	int symbolIndex=0;

	#define MAX_DISTANCE 10000000

	int symbolDistance=MAX_DISTANCE;
	int d;
	int i=0;
	char sTmp[32];
	MARIA_TABLE_RECORD    *p;

#if defined MARIA_TABLE_ON_FLASH
	memcpy_P(&s_maria_table_record_copy, &(maria_table[i]), sizeof(s_maria_table_record_copy));
	p = &s_maria_table_record_copy;
#else
	p=maria_table;
#endif
	if(p->symaddr)
	{
		symbolDistance=(addr - (p->symaddr));
		if(symbolDistance<0) symbolDistance=MAX_DISTANCE;
	}

	while(p->symaddr)
	{
		if(addr >= p->symaddr)
		{
			d=(addr-p->symaddr);
			if(d<symbolDistance)
			{
				symbolDistance=d;
				symbolIndex=i;
			}
		}

		i++;
#if defined MARIA_TABLE_ON_FLASH
		memcpy_P(&s_maria_table_record_copy, &(maria_table[i]), sizeof(s_maria_table_record_copy));
		p = &s_maria_table_record_copy;
#else
		p++;
#endif
	}

#if defined MARIA_TABLE_ON_FLASH
	memcpy_P(&s_maria_table_record_copy, &(maria_table[symbolIndex]),
			 sizeof(s_maria_table_record_copy));
	p = &s_maria_table_record_copy;
#else
	p = maria_table + symbolIndex;
#endif
	if(symString)
	{
		ARD_sprintf(symString,"%s",p->symbol);
		if(symbolDistance)
		{
			ARD_sprintf(sTmp,"+0x%X",symbolDistance);
			ARD_strcat(symString,sTmp);
		}
	}
	else
	{
		/* if symString is null output message to stdout */
		maria_printf_1("%s",p->symbol);
		if(symbolDistance)
		{
			maria_printf_1("+0x%X",symbolDistance);
		}
		maria_printf_0("\n");
	}

	return 0;
}
#endif /* TARGET_ARCH_I386 || TARGET_ARCH_X64 */

int rem(void)
{
	return 0;
}

int retval(int i, int j)
{
	return 0;
}

int Unknown_symbol(void)
{
	return 0;
}

int showSym(char *symbol)
{
	MARIA_TABLE_ATTR MARIA_TABLE_RECORD   *pRecord;
	char *pTypeSym;
	char *p;

#if defined MARIA_TABLE_ON_FLASH
	uint16_t         idx = 0;
	memcpy_P(&s_maria_table_record_copy, &(maria_table[idx]), sizeof(s_maria_table_record_copy));
	pRecord = &(s_maria_table_record_copy);
#else
	pRecord = maria_table;
#endif


	while(pRecord->symaddr)
	{
		pTypeSym="";
		if(pRecord->symtype==MARIA_SYMTYPE_FUNCTION) pTypeSym="()";

		if(symbol==NULL)
		{
#if defined (__GNUC__)
			/* show every symbol */
			maria_printf_3("%p %s%s\n",pRecord->symaddr,pRecord->symbol,pTypeSym);
#elif defined (SDCC_hc08)
			/* show every symbol */
			maria_printf_3("0x%04X %s%s\n",(unsigned short)(pRecord->symaddr),pRecord->symbol,pTypeSym);
#else
#error "Unknown compiler"
#endif
		}
		else
		{
			p=ARD_strstr(pRecord->symbol,symbol);
			if(p!=NULL)
			{
#if defined (__GNUC__)
				maria_printf_3("%p %s%s\n",pRecord->symaddr,pRecord->symbol,pTypeSym);
#elif defined (SDCC_hc08)
				maria_printf_3("0x%04X %s%s\n",(unsigned short)(pRecord->symaddr),pRecord->symbol,pTypeSym);
#else
#error "Unknown compiler"
#endif
			}
		}
#if defined MARIA_TABLE_ON_FLASH
		idx++;
		ARD_memcpy_P(&s_maria_table_record_copy, &(maria_table[idx]), sizeof(s_maria_table_record_copy));
		pRecord = &(s_maria_table_record_copy);
#else
		pRecord++;
#endif
	}
	return 0;

}

#if !defined( TARGET_ARCH_HC08 )
int fillMemory(unsigned char *address, char *hexstring)
{
	unsigned char *p = address;
	char          sTmp[3];
	int           i, l = ARD_strlen(hexstring);

	for(i=0; i<l; i+=2)
	{
		sTmp[0]=hexstring[i];
		sTmp[1]=hexstring[i+1];
		sTmp[2]=0; /* terminate substring */
		*p = ARD_strtol(sTmp,NULL,16);
		p++;
	}
	return 0;
}
#endif

int showMaria(void)
{
	int     	i, j;
	uint8_t 	*p;

	maria_printf("g_active_fd_index = %d\n\n", g_active_fd_index);
	for(i=0; i<MARIA_MAX_FDS; i++)
	{
		maria_printf("maria_data[%d].r_fd = %d\n", i, maria_data[i].r_fd);
		maria_printf("maria_data[%d].w_fd = %d\n", i, maria_data[i].w_fd);
		maria_printf("maria_data[%d].ud:\n", i);
#if !defined(ARDUINO)
		maria_printf("    .fdInstance = %d\n", maria_data[i].ud.fdInstance);
		maria_printf("    .fdTag = %d\n", maria_data[i].ud.fdTag);
#endif /* ARDUINO */
		maria_printf("    .fdType = %d\n", maria_data[i].ud.fdType);
#if !defined(ARDUINO)
		j = sizeof(maria_data[i].ud.fdInstance) + sizeof(maria_data[i].ud.fdTag) + sizeof(maria_data[i].ud.fdType);
#else
		j = sizeof(maria_data[i].ud.fdType);
#endif /* ARDUINO */
		maria_printf("    ");
		for(; j<sizeof(maria_data[i]); j++)
		{
			p = (uint8_t *)(&maria_data[i]);
			maria_printf("%02X", p[j]);
		}
		maria_printf("\n\n");
	}
	return MARIA_MAX_FDS;
}

/* prints a packet dump in hexadecimal */
/* sprefix string (optional) is printed before the hexadecimal dump as a user defined title */
int dumppkt(unsigned long address, int len, char *sprefix)
{
	int    		i;
	uint8_t     *p = (uint8_t *)address;

	if(sprefix != NULL)
	{
		maria_printf("%s", sprefix);
	}

	if(p != NULL)
	{
		for(i=0; i<len; i++)
		{
			maria_printf("%02X ", p[i]);
		}
	}
	maria_printf("\n");

	return len;
}

int printArgs(unsigned long ptr, char *ptr2)
{
	maria_printf("ptr=%ld  0x%lX\n", ptr, ptr);
	maria_printf("ptr2=%p\n", ptr2);
	return 55;
}

#if MSCR_SCRIPT_LINES > 0
static char *MSCR[MSCR_SCRIPT_LINES];
static int  MSCR_LBL[MSCR_SCRIPT_LINES];
static int    mscrNextLine = 0;

static void mscrPreParse(void)
{
	int      i;
	int      lblCode = 0;
	char     lblName[8];
	int      lblNameIdx = 0;
	int      mscrLine = 0;

	lblName[0] = 0;
	for(i=0; mscrBuf[i]!=0; i++)
	{
		if(mscrBuf[i] == '@')
		{
			lblNameIdx = 0;
			lblName[lblNameIdx] = mscrBuf[i];
			lblNameIdx++;
		}
		else if(mscrBuf[i] == ';')
		{
			if(lblNameIdx)
			{
				// e' la fine del nome della label
				lblName[lblNameIdx] = 0;
				lblCode = ARD_strtol(lblName+1, NULL, 0);
				MSCR_LBL[lblCode%MSCR_SCRIPT_LINES] = mscrLine;
				lblNameIdx = 0;
				lblName[lblNameIdx] = 0;
			}
			else
			{
				// e' la fine di una riga di script
				mscrLine ++;
				mscrLine %= MSCR_SCRIPT_LINES;
			}
		}
		else
		{
			// e' un carattere qualunque
			if(lblNameIdx)
			{
				lblName[lblNameIdx] = mscrBuf[i];
				lblNameIdx++;
			}
			else if(MSCR[mscrLine] == NULL)
			{
				MSCR[mscrLine] = &(mscrBuf[i]);
			}
		}
	}
}

int mscrInit(void)
{
	int          i;

	mscrBuf = MSCR_BUF;
	for(i=0; i<MSCR_SCRIPT_LINES; i++)
	{
		MSCR[i] = NULL;
		MSCR_LBL[i] = 0;
	}

	mscrNextLine = 0;
	mscrPreParse();

	return MSCR_SCRIPT_LINES;
}


ssize_t maria_mscr_read(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data)
{
	int 				l, i, ret;
	char                *p = buf;
	char   				*mscrPtr = NULL;

	if(user_data->fdType != HANDLE_TYPE_MEMORY) return -1;  // fd is not correct type
	mscrPtr = MSCR[mscrNextLine];
	if(mscrPtr == NULL)
	{
		return 0; /* script was completed or empty */
	}
	if(*mscrPtr == 0)
	{
		return 0; /* script was completed or empty */
	}

	ret = 0;
	for(i=0; i<count; i++)
	{
		ret++;
		if( (*mscrPtr == '\n') || (*mscrPtr == ';'))
		{
			*p = '\n';
			mscrNextLine ++;
			mscrNextLine %= MSCR_SCRIPT_LINES;
			break;
		}
		else if(*mscrPtr == '\r')
		{
			/* ignore \r */
			ret --;
		}
		else
		{
			*p = *mscrPtr;
		}
		p++;
		mscrPtr++;
	}
	return ret;
}

ssize_t maria_mscr_write(int fd, void *buf, size_t count, MARIA_USER_DATA *user_data)
{
	if(user_data->fdType != HANDLE_TYPE_MEMORY) return 0;

	return count;
}


int showMscr(void)
{
	int       i;
	char      *p;
	maria_printf("MSCR ============================================== \n");
	for(i=0; i<MSCR_SCRIPT_LINES; i++)
	{
		p = MSCR[i];

		maria_printf("[%02d]%c ", i, (mscrNextLine == i) ? '*' : ' ');

		while(p!=NULL)
		{
			if((*p) == ';')
			{
				break;
			}
			else
			{
				maria_printf("%c", *p);
				p++;
			}
		}
		maria_printf("\n");
	}
	maria_printf("\n");
	maria_printf("MSCR labels ========================================== \n");
	for(i=0; i<MSCR_SCRIPT_LINES; i++)
	{
		maria_printf("LBL%02d -> line %d\n", i, MSCR_LBL[i]);
	}
	return 0;
}

int mscrRun(int startline)
{
	int     ret;

	mscrNextLine = startline;

	return 0;
}

int mscrGoto(int label)
{
	int          i;

	if(label < 0) return 0;
	if(label >= MSCR_SCRIPT_LINES) return 0;

	mscrNextLine = MSCR_LBL[label];

	return mscrNextLine;
}

int mscrTest(int number)
{
	maria_printf("executing %s(%d)\n", __func__, number);
	printf("executing %s(%d)\n", __func__, number);
	return number;
}
#endif /* MSCR_SCRIPT_LINES > 0 */
