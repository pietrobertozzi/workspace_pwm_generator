#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>

#if defined(ARDUINO)
#include "APP_config.h"
#endif /* ARDUINO */

#if defined (TARGET_ARCH_AVR)
#include <inttypes.h>
#elif defined(TARGET_ARCH_ARM7)
#include <stdint.h>
#elif defined(TARGET_ARCH_ARM)
#include <stdint.h>
#elif defined (TARGET_ARCH_HC08)
#include <mylibc.h>
#elif defined ( TARGET_ARCH_PIC32 )
#include <errno.h>
#elif defined ( TARGET_ARCH_MIPS )
#include <errno.h>
#elif defined(TARGET_ARCH_ESP32)
#include <stdint.h>
#endif

#include <maria_parser.h>
#include <maria.h>
#if defined(MARIA_INFO_TRACE)
	#if defined(DFWTRC)
		#include "general.h"
		void dfwTrc0(unsigned int ref, char *sMsg);
	#elif defined(TRCW)
		#define MYSELF 0
		int BASE_CODE=0xCA770000;
		#include "libtrc/modules.h"
		#include "libtrc/trc.h"
	#elif defined(TRCSTDOUT)
		#include <stdio.h>
	#endif
#endif
#if !defined(MARIA_INFOTRC_PREFIX)
#define MARIA_INFOTRC_PREFIX ""
#endif

#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000			\
					 + __GNUC_MINOR__ * 100		\
					 + __GNUC_PATCHLEVEL__)

/* esempio #if GCC_VERSION > 30200 significa > 3.2.0 */
#elif defined SDCC_hc08
/* do nothing */
#else
#error "Unsupported compiler"
#endif

#if defined MARIA_TUNNEL
extern int maria_tunnel(char *s);
#endif

#if !defined(NO_MARIA_INFO)
INFO_USE(Maria);
#endif

#if defined(MARIA_PARNUM_CHECK)
	#define WRONG_PAR_NUMBER_FMT_STR		"Command %s: Wrong par num %d, expected %d"
#endif

//#define PRINTF_DEBUG

MARIA_PARSED_LINE  s_maria_argv;
char               maria_sLine[MARIA_LINE_SIZE];
char               errmsg[64]="\n\rUnknown_symbol\n\r";

#ifdef MARIA_TABLE_ON_FLASH
MARIA_TABLE_RECORD s_maria_table_record_copy;
#endif

int g_active_fd_index = -1;
MARIA_DATA maria_data[MARIA_MAX_FDS];

char prompt[8]=">>";

#ifdef PRINTF_DEBUG
static void dumpArgv(void);
#endif
static void fillArgv(MARIA_USER_DATA *user_data);
static void executeArgv(int fd, MARIA_PARSED_LINE *pCmd, MARIA_USER_DATA *user_data);
static int findSymbolExact(char * name);
static int maria_handleVar(GENPTR pvar,MARIA_PARSED_LINE *pArgv);
static char *tokenizer(char *sin, char *delim, char **pnext);


void init_maria_table(void);

static int maria_callApi(MARIA_API_POINTER api,MARIA_PARSED_LINE *pArgv)
{
	STACKSIMUL stackSimul;
	int i;

#if defined ( TARGET_ARCH_I386 )
	char *p=(char *)&stackSimul;
#elif defined( TARGET_ARCH_X64 )
	long *p=(long *)(&(stackSimul.buf[0]));
#elif defined ( TARGET_ARCH_ARM )
	char *p=(char *)&stackSimul;
#elif defined ( TARGET_ARCH_ARM7 )
	char *p=(char *)&stackSimul;
#elif defined ( TARGET_ARCH_AVR )
	/* avr fills function arguments in reverse order */
	char *p=((char *)&stackSimul) + sizeof(STACKSIMUL);
#elif defined ( TARGET_ARCH_HC08 )
	char *p = (char *)(&(stackSimul.buf[0]));
	int  *pStackSimul = (int *)&stackSimul;
#elif defined ( TARGET_ARCH_PIC32 )
	char *p=(char *)&stackSimul;
#elif defined ( TARGET_ARCH_MIPS )
	char *p=(char *)&stackSimul;
#elif defined ( TARGET_ARCH_ESP32 )
	char *p=(char *)(&(stackSimul.buf));
	long *plong = (long *)(&(stackSimul.buf));
#else
#error "Unknown target architecture"
#endif

	ARD_memset((char*)&stackSimul,0,sizeof(stackSimul));

	if(s_maria_argv.argc<=0) return 0;

	for(i=1;i<s_maria_argv.argc;i++)
	{
		switch(s_maria_argv.type[i])
		{
#if defined (__GNUC__) || defined (SDCC_hc08)
		case MARIA_TYPE_CHAR:
#if defined ( TARGET_ARCH_I386 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined( TARGET_ARCH_X64 )
			*p=s_maria_argv.lValue[i];
			p++;
#elif defined ( TARGET_ARCH_ARM )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_ARM7 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_AVR )
			p-=sizeof(uint16_t);
			*((uint16_t *)p)=s_maria_argv.lValue[i];
#elif defined ( TARGET_ARCH_HC08 )
			*((unsigned char *)p)=(char)s_maria_argv.lValue[i];
			p+=sizeof(char);
#elif defined ( TARGET_ARCH_PIC32 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_MIPS )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_ESP32 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#else
#error "Unknown target architecture"
#endif
			break;
		case MARIA_TYPE_INT:
		case MARIA_TYPE_SHORT:
		case MARIA_TYPE_SYMBOL:
#if defined ( TARGET_ARCH_I386 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined( TARGET_ARCH_X64 )
			*p=s_maria_argv.lValue[i];
			p++;
#elif defined ( TARGET_ARCH_ARM )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_ARM7 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_AVR )
			p-=sizeof(uint16_t);
			*((uint16_t *)p)=s_maria_argv.lValue[i];
#elif defined ( TARGET_ARCH_HC08 )
			*((unsigned short *)p)=s_maria_argv.lValue[i];
			p+=sizeof(unsigned short);
#elif defined ( TARGET_ARCH_PIC32 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_MIPS )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#elif defined ( TARGET_ARCH_ESP32 )
			*((long *)p)=s_maria_argv.lValue[i];
			p+=sizeof(long);
#else
#error "Unknown target architecture"
#endif
			break;
		case MARIA_TYPE_STRING:
#if defined ( TARGET_ARCH_I386 )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#elif defined( TARGET_ARCH_X64 )
			*((char **)p)=s_maria_argv.argv[i];
			p++;
#elif defined ( TARGET_ARCH_ARM )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#elif defined ( TARGET_ARCH_ARM7 )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#elif defined ( TARGET_ARCH_AVR )
			p-=sizeof(PTRSTORAGE);
			*((char **)p)=s_maria_argv.argv[i];
#elif defined ( TARGET_ARCH_HC08 )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(PTRSTORAGE);
#elif defined ( TARGET_ARCH_PIC32 )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#elif defined ( TARGET_ARCH_MIPS )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#elif defined ( TARGET_ARCH_ESP32 )
			*((char **)p)=s_maria_argv.argv[i];
			p+=sizeof(char *);
#else
#error "Unknown target architecture"
#endif
			break;
#endif /* __GNUC__ */
		default:
#if !defined(NO_MARIA_INFO)
			info0(Maria,"Errore unknown type");
#endif
			break;
		}
	}
	maria_printf("\n");
#if defined (__GNUC__)
#if !defined(NO_MARIA_INFO)
	info1(Maria,"prova info %d",sizeof(int));
#endif

#if defined ( TARGET_ARCH_I386 )
	return api(stackSimul);
#elif defined( TARGET_ARCH_X64 )
	return api(stackSimul.buf[0], stackSimul.buf[1], stackSimul.buf[2], stackSimul.buf[3],
			stackSimul.buf[4], stackSimul.buf[5], stackSimul.buf[6], stackSimul.buf[7],
			stackSimul.buf[8], stackSimul.buf[9], stackSimul.buf[10], stackSimul.buf[11],
			stackSimul.buf[12], stackSimul.buf[13], stackSimul.buf[14], stackSimul.buf[15]);
#elif defined ( TARGET_ARCH_ARM )
	return api(stackSimul);
#elif defined ( TARGET_ARCH_ARM7 )
	return api(stackSimul);
#elif defined ( TARGET_ARCH_AVR )
	return api(stackSimul);
#elif defined ( TARGET_ARCH_HC08 )
	return api(stackSimul);
#elif defined ( TARGET_ARCH_PIC32 )
	return api(stackSimul);
#elif defined ( TARGET_ARCH_MIPS )
	return api(stackSimul);
#elif defined( TARGET_ARCH_ESP32 )
	return api(plong[0], plong[1], plong[2], plong[3], plong[4], plong[5], plong[6], plong[7]);
#else
#error "Unknown target architecture"
#endif

#elif defined (SDCC_hc08)
#if !defined(NO_MARIA_INFO)
	info1(Maria,"prova info %d",sizeof(int));
#endif
	return api(
		pStackSimul[0],
		pStackSimul[1],
		pStackSimul[2],
		pStackSimul[3]
		);
#else
#error "Unknown compiler"
#endif
}

int maria_max_fd(void)
{
	int maxFd=0;
	int i;
	for(i=0;i<MARIA_MAX_FDS;i++)
	{
		maxFd=ARD_max(maxFd, maria_data[i].r_fd);
	}
	return maxFd;
}

/* --------------------------------------------------------------------- */
/* INTERFACE FUNCTION */
/* --------------------------------------------------------------------- */
void maria_init(void)
{
	int      i;

	//init_maria_table();
	ARD_memset(&s_maria_argv, 0, sizeof(s_maria_argv));
	g_active_fd_index = 0;

	for(i=0; i<MARIA_MAX_FDS; i++)
	{
		maria_data[i].r_fd = -1;
		maria_data[i].w_fd = -1;
		maria_data[i].ud.fdType = HANDLE_TYPE_SOCK;
	}
}

void maria_finish(void)
{
	int i;

	for(i=0;i<MARIA_MAX_FDS;i++)
	{
		maria_close(&maria_data[i].r_fd, &maria_data[i].w_fd, &maria_data[i].ud);
	}
}

int maria_parser(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data)
{
	int len,l,i;

	len=maria_read(*pfd_r, maria_sLine, MARIA_LINE_SIZE-1, user_data);
	maria_sLine[len]=0;

	if(len == 0)
	{
		maria_close(pfd_r, pfd_w, user_data);
		return -1;
	}

	l=ARD_strlen(s_maria_argv.sLine);

	for(i=0;(i<len && l<(sizeof(s_maria_argv.sLine)-1));i++)
	{
		if(maria_sLine[i]=='\n' || maria_sLine[i]=='\r')
		{
#if defined MARIA_TUNNEL
			/* if command starts with 'x', forward it to tulnneled parser */
			if(s_maria_argv.sLine[0] == 'x')
			{
				s_maria_argv.sLine[l]=maria_sLine[i];
				l++;
				s_maria_argv.sLine[l]=0;

				if(maria_tunnel_on)
				{
					maria_tunnel(&(s_maria_argv.sLine[1]));
				}

				s_maria_argv.sLine[0] = 0;
        break;
			}
			else
#endif
			{
				fillArgv(user_data);
#if defined(PRINTF_DEBUG)
				dumpArgv(); /* for debugging */
#endif
				executeArgv(*pfd_w,&s_maria_argv, user_data);
				ARD_memset(&s_maria_argv, 0, sizeof(s_maria_argv));
				maria_prompt(pfd_r,pfd_w,user_data);
				break; /* ignore rest of line */
			}
		}
		else
		{
#ifdef MARIA_LINE_EDIT
			// backspace 0x08
			// del 0x7f
			// simulfrecce
			// ^a 0x01 (<)
			// ^s 0x13 (>)
			// ^w 0x17 (^)
			// ^z 0x1a (V)
//			if(maria_sLine[i] == 0x01) {		//freccia sinistra
//			}
//			else if(maria_sLine[i] == 0x13) {	//freccia destra
//			}
//			else if(maria_sLine[i] == 0x17) {	//freccia su
//			}
//			else if(maria_sLine[i] == 0x1a) {	//freccia giu`
//			}
//			else
			if(maria_sLine[i] == 0x08) {	//backspace
				if(l > 0) {
					l--;
					s_maria_argv.sLine[l]=0;
				}
			}
//			else if(maria_sLine[i] == 0x7f) {	//del
//			}
			else {
#endif
				s_maria_argv.sLine[l]=maria_sLine[i];
				l++;
				s_maria_argv.sLine[l]=0;
#ifdef MARIA_LINE_EDIT
			}
#endif
		}
		l=ARD_strlen(s_maria_argv.sLine);
	}

	return *pfd_r;
}

static int
maria_printf_buffer_flush(int fd_index, char *s)
{
	int ret = 0;
	int   len;

	if((fd_index < 0) || (fd_index >= MARIA_MAX_FDS)) return 0;

	if(maria_data[fd_index].w_fd >= 0)
	{
		len = ARD_strlen(s);
		if(MARIA_ADD_CR)
		{
			if(s[len-1] == '\n')
			{
				s[len-1] = '\r';
				s[len]='\n';
				len ++;
				s[len]=0; /* terminate string */
			}
		}
		ret=maria_write(maria_data[fd_index].w_fd,
						s,len, &maria_data[fd_index].ud);
	}
	return ret;
}

int maria_printf(const char *format, ...)
{
	static char maria_printf_buffer[128];
	int ret = 0;

	va_list ap;

	va_start(ap, format);

#if defined(__GNUC__)
	ARD_vsnprintf(maria_printf_buffer,sizeof(maria_printf_buffer),format,ap);
#elif defined(SDCC_hc08)
	ARD_vsprintf(maria_printf_buffer,format,ap);
#else
#error "Unsupported compiler"
#endif
	maria_printf_buffer[sizeof(maria_printf_buffer)-1]=0;
	ret = maria_printf_buffer_flush(g_active_fd_index, maria_printf_buffer);

	va_end(ap);
	return ret;
}

int maria_fprintf(int fd_idx,const char *format, ...)
{
	static char maria_printf_buffer[128];
	int ret = 0;

	va_list ap;

	va_start(ap, format);

#if defined(__GNUC__)
	ARD_vsnprintf(maria_printf_buffer,sizeof(maria_printf_buffer),format,ap);
#elif defined(SDCC_hc08)
	ARD_vsprintf(maria_printf_buffer,format,ap);
#else
#error "Unsupported compiler"
#endif
	maria_printf_buffer[sizeof(maria_printf_buffer)-1]=0;
	ret = maria_printf_buffer_flush(fd_idx, maria_printf_buffer);

	va_end(ap);
	return ret;
}

#if defined(__gnu_linux__)
char *maria_timestamp(void)
{
	static char sss[64];
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	ARD_sprintf(sss, "%ld.%03ld ", now.tv_sec%3600, now.tv_nsec/1000000);
	return sss;
}
#endif

/* --------------------------------------------------------------------- */
/* STATIC FUNCTION */
/* --------------------------------------------------------------------- */
#if !defined(NO_MARIA_INFO)
int infoLog(unsigned short mask, unsigned short enabledMask, char *file, int line, char *fmtstring, ...)
{
	va_list ap;
	char msg[MAX_INFO_LEN];
	char msg1[MAX_INFO_LEN];
#if defined(MARIA_INFO_ON_ALL_FD)
	int i;
#endif
	int ret=0;
	static char *strtokptr;
	va_start(ap, fmtstring);

	if( (mask & ((1<<0xE) | (1<<0xD))) || (infoGlobOn && (mask & enabledMask)) )
	{
#if defined(__gnu_linux__)
			sprintf(msg,"%s:%-15s:%-4d: ",maria_timestamp(), file,line);
#else
			strcpy(msg, MARIA_INFOTRC_PREFIX);
#endif
			vsprintf(msg1,fmtstring,ap);
			strcat(msg, msg1);
			tokenizer(msg, "\n\r", &strtokptr);
			strcat(msg, "\n");
	}
#if defined(MARIA_INFO_TRACE)
	// messaggio destinato a trace comunque
	if(mask & ((1<<0xE) | (1<<0xD)))
	{
#if defined(DFWTRC)
		dfwTrc0(TRC_REF, msg);
#elif defined(TRCW)
		if(mask & (1<<0xD))
		{
			trcW(_DEBUG, msg);
		}
		else if(mask & (1<<0xE))
		{
			trcW(_EINVAL, msg);
		}
#elif defined(TRCSTDOUT)
		printf(msg);
#endif /* TRC */
	}
#endif
	if((infoGlobOn && (mask & enabledMask)) || (mask & (1<<0xE)))
	{
#if defined(__gnu_linux__)
		ARD_sprintf(msg,"%s:%-15s:%-4d: ",maria_timestamp(), file,line);
#else
		msg[0]=0;
#endif
		ARD_vsprintf(msg1,fmtstring,ap);
		ARD_strcat(msg, msg1);

		tokenizer(msg, "\n\r", &strtokptr);
		if(!MARIA_ADD_CR) ARD_strcat(msg, "\n");
		else ARD_strcat(msg, "\r\n");

#if defined(MARIA_INFO_DUPLICATE_ON_STDOUT)
		printf(msg);
#endif

#if defined(MARIA_INFO_ON_ALL_FD)
		for(i=0;i<MARIA_MAX_FDS;i++)
		{
			if(maria_data[i].w_fd >= 0)
			{
				ret = maria_write(maria_data[i].w_fd, msg,ARD_strlen(msg), &(maria_data[i].ud));
			}
		}
#else
		if((g_active_fd_index >=0) && (maria_data[g_active_fd_index].w_fd >= 0))
		{
			ret = maria_write(maria_data[g_active_fd_index].w_fd, msg,ARD_strlen(msg), &(maria_data[g_active_fd_index].ud));
		}
#endif
	}

	va_end(ap);
	return ret;
}

#endif

static int isDelim(char c, char *delim)
{
	int      ndelim = ARD_strlen(delim);
	int      i;

	for(i=0; i<ndelim; i++)
	{
		if(c == delim[i]) return 1;
	}
	return 0;
}

static char *tokenizer(char *sin, char *delim, char **pnext)
{
	char             *pTok = NULL;
	int              ignoreDelim = 0;
	int              delimFound = 0;
	char             prevChar=0;

	if((delim == NULL)||(pnext == NULL)) return NULL;
	if(delim[0] == 0) return NULL;

	if(sin != NULL) *pnext = sin;

	if((*(*pnext)) == 0) return NULL;

	while(isDelim(*(*pnext), delim))
	{
		*(*pnext) = 0;
		(*pnext)++;
	}

	pTok = *pnext;
	while((*(*pnext)) != 0)
	{
		if((*(*pnext)) ==  '"')
		{
			if(prevChar != '\\') ignoreDelim = (ignoreDelim ? 0 : 1);
		}

		if((!ignoreDelim) && isDelim(*(*pnext), delim))
		{
			delimFound = 1;
			*(*pnext) = 0;
		}
		else if(delimFound && (!isDelim(*(*pnext), delim)))
		{
			return pTok;
		}
		prevChar = *(*pnext);
		(*pnext)++;
	}
	return pTok;
}

static uint8_t hex2nibble(char h)
{
	if(h < '0') return 0;
	if(h <= '9') return (h - '0');
	if(h < 'A') return 0;
	if(h <= 'F') return (h -'A' + 10);
	if(h < 'a') return 0;
	if(h <= 'f') return (h -'a' + 10);
	return 0;
}

static uint8_t hex2byte(char *h)
{
	 uint8_t   val;

	 val = hex2nibble(h[0]);
	 val <<= 4;
	 val |= hex2nibble(h[1]);

	return val;
}

static void unquoteToken(char *tok)
{
	int 	len = ARD_strlen(tok);
	int     i, j;

	for(i=0; i<(len-1); i++)
	{
		if(tok[i] == '\\')
		{
			switch(tok[i+1])
			{
				case 't':
					tok[i] = '\t';
					for(j=i+1; j<len-1; j++) tok[j] = tok[j+1];
					tok[j] = 0;
				break;
				case 'n':
					tok[i] = '\n';
					for(j=i+1; j<len-1; j++) tok[j] = tok[j+1];
					tok[j] = 0;
				break;
				case 'r':
					tok[i] = '\r';
					for(j=i+1; j<len-1; j++) tok[j] = tok[j+1];
					tok[j] = 0;
				break;
				case '\\':
					tok[i] = '\\';
					for(j=i+1; j<len-1; j++) tok[j] = tok[j+1];
					tok[j] = 0;
				break;
				case '\"':
					tok[i] = '\"';
					for(j=i+1; j<len-1; j++) tok[j] = tok[j+1];
					tok[j] = 0;
				break;
				case 'x':
					tok[i] = hex2byte(&(tok[i+2]));
					for(j=i+1; j<len-3; j++) tok[j] = tok[j+3];
					tok[j] = 0;
				break;
			}
		}
	}
}


static void fillArgv(MARIA_USER_DATA *user_data)
{
	char  *p=NULL;
	char  delims[]=" \t\n\r=(),:><";
	register int   i, idx;
	int len;
	static char *strtokptr;

	s_maria_argv.argc=0;
	p=tokenizer(s_maria_argv.sLine,delims,&strtokptr);
	while(p)
	{
		unquoteToken(p);
		idx=findSymbolExact(p);
		if(idx >= 0)
		{
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_SYMBOL;
#ifdef MARIA_TABLE_ON_FLASH

			memcpy_P(&s_maria_table_record_copy, &(maria_table[idx]), sizeof(s_maria_table_record_copy));
			s_maria_argv.lValue[s_maria_argv.argc]=(PTRSTORAGE)s_maria_table_record_copy.symaddr;
#else
			s_maria_argv.lValue[s_maria_argv.argc]=(PTRSTORAGE)maria_table[idx].symaddr;
#endif
#if defined(MARIA_ANS)
			if(!ARD_strcmp(p, "ANS"))
			{
				/* gestione separata per la variabile ANS: normalmente quando su una linea di comando
				 * viene incontrato un simbolo, si sostituisce a questo il suo indirizzo.
				 * la Variabile ANS viene invece sostituita con il suo valore */
				s_maria_argv.type[s_maria_argv.argc] = MARIA_TYPE_INT;
				s_maria_argv.lValue[s_maria_argv.argc] = ANS;
			}
#endif
		}
		else if(p[0]=='\"')
		{
			/* it is a string */
			p++; /* skip leading \"*/
			len=ARD_strlen(p);
			for(i=len-1;i>=0;i--)
			{
				if(p[i]=='\"')
				{
					p[i]=0;
					break;
				}
			}
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_STRING;
		}
		else if((p[0]=='c')||(p[0]=='C'))
		{
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_CHAR;
			s_maria_argv.lValue[s_maria_argv.argc]=ARD_strtol(p+1,NULL,0);
		}
		else if((p[0]=='s')||(p[0]=='S'))
		{
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_SHORT;
			s_maria_argv.lValue[s_maria_argv.argc]=ARD_strtol(p+1,NULL,0);
		}
		else if((p[0]=='0')&&(p[1]=='x'))
		{
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_INT;
			s_maria_argv.ulValue[s_maria_argv.argc]=ARD_strtoul(p,NULL,0);
		}
		else
		{
			s_maria_argv.type[s_maria_argv.argc]=MARIA_TYPE_INT;
#if defined ( TARGET_ARCH_PIC32 )
			errno = 0;
			s_maria_argv.lValue[s_maria_argv.argc]= ARD_strtoll(p,NULL,0);
#else
			s_maria_argv.lValue[s_maria_argv.argc]= ARD_strtol(p,NULL,0);
#endif
		}
		s_maria_argv.argv[s_maria_argv.argc]=p;
		s_maria_argv.argc++;
		if(s_maria_argv.argc>=MARIA_MAX_ARGC) break;

		p=tokenizer(NULL,delims,&strtokptr);
	}
}

#ifdef PRINTF_DEBUG

static void dumpArgv(void)
{
	int i;

	printf("argc=%d\n",s_maria_argv.argc);
	for(i=0;i<s_maria_argv.argc;i++)
	{
		if(s_maria_argv.type[i] == MARIA_TYPE_STRING)
		{
			printf("argv[%d]=%s\n",i,s_maria_argv.argv[i]);
		}
		else
		{
#if defined ( TARGET_ARCH_I386 )
			printf("argv[%d]=%s (%ld)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined( TARGET_ARCH_X64 )
			printf("argv[%d]=%s (%ld)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined ( TARGET_ARCH_ARM )
			printf("argv[%d]=%s (%ld)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined ( TARGET_ARCH_ARM7 )
			printf("argv[%d]=%s (%ld)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined ( TARGET_ARCH_AVR )
			printf("argv[%d]=%s (%d)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined ( TARGET_ARCH_HC08 )
			printf("argv[%d]=%s (%d)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#elif defined ( TARGET_ARCH_ESP32 )
			printf("argv[%d]=%s (%ld)\n",i,s_maria_argv.argv[i],s_maria_argv.lValue[i]);
#else
#error "Unknown target architecture"
#endif
		}
	}
	printf("\n");
}
#endif /* PRINTF_DEBUG */


static void executeArgv(int fd, MARIA_PARSED_LINE *pCmd, MARIA_USER_DATA *user_data)
{
	MARIA_TABLE_RECORD *p;
	int retval = 0;

#ifdef MARIA_TABLE_ON_FLASH
	uint16_t   idxP = 0;
	memcpy_P(&(s_maria_table_record_copy), &(maria_table[idxP]), sizeof(s_maria_table_record_copy));
	p= &s_maria_table_record_copy;
#else
	p=(MARIA_TABLE_RECORD *) maria_table;
#endif

	if(pCmd->argc <= 0) return;

	if(MARIA_ADD_CR) ARD_sprintf(errmsg,"\n\rUnknown_symbol(\"%s\");\n\r", pCmd->argv[0]);
	else ARD_sprintf(errmsg,"\nUnknown_symbol(\"%s\");\n", pCmd->argv[0]);
	while(p->symaddr)
	{
		if(!ARD_strcmp(p->symbol,pCmd->argv[0]))
		{
#if defined(MARIA_PARNUM_CHECK)
			if(p->len != -1)	// EDL
			{
				if(pCmd->argc != (p->len + 1)) {
					if(MARIA_ADD_CR) sprintf(errmsg,"\r\n" WRONG_PAR_NUMBER_FMT_STR "\r\n", pCmd->argv[0], pCmd->argc - 1, p->len);
					else sprintf(errmsg,"\n" WRONG_PAR_NUMBER_FMT_STR "\n", pCmd->argv[0], pCmd->argc - 1, p->len);
					break;
				}
			}
#endif

			if(p->symtype == MARIA_SYMTYPE_FUNCTION)
			{
				retval = maria_callApi((MARIA_API_POINTER)(p->symaddr),pCmd);
			}
			if(p->symtype == MARIA_SYMTYPE_DATA)
			{
				retval = maria_handleVar(p->symaddr,pCmd);
			}
#if defined(MARIA_ANS)
			ANS = retval;
#endif

			if((user_data->mariaFlags & MARIA_FLAGS_NORETVAL) == 0)
			{
#if defined (__GNUC__)
	#if defined (MARIA_HEXONLY_RETVAL_FORMAT)
				if(MARIA_ADD_CR) ARD_sprintf(errmsg,"\r\n0x%08X\r\n",retval);
				else sprintf(errmsg,"\n0x%08X\n",retval);
	#else
				if(MARIA_ADD_CR) ARD_sprintf(errmsg,"\r\nretval %d (0x%X);\r\n",retval,retval);
				else ARD_sprintf(errmsg,"\nretval %d (0x%X);\n",retval,retval);
	#endif
#elif defined (SDCC_hc08)
				if(MARIA_ADD_CR) sprintf(errmsg,"\r\naddr: 0x%04X val: %d (0x%X)\r\n", (unsigned short)(p->symaddr), retval, retval);
				else ARD_sprintf(errmsg,"\naddr: 0x%04X val: %d (0x%X)\n", (unsigned short)(p->symaddr), retval, retval);
#else
#error "Undefined compiler"
#endif
			}
			else
			{
				// MARIA_FLAGS_NORETVAL is active
				errmsg[0] = 0;
 			}
			break; // symbol is found in table
		}

#ifdef MARIA_TABLE_ON_FLASH
		idxP++;
		memcpy_P(&s_maria_table_record_copy, &(maria_table[idxP]), sizeof(s_maria_table_record_copy));
		p = &s_maria_table_record_copy;
#else
		p++;
#endif
	}

	retval = ARD_strlen(errmsg);
	if(retval > 0) maria_write(fd,errmsg,retval,(MARIA_USER_DATA *)user_data);
}

static int findSymbolExact(char * name)
{
	int idx=0;

#ifdef MARIA_TABLE_ON_FLASH
	memcpy_P(&s_maria_table_record_copy, &(maria_table[idx]), sizeof(s_maria_table_record_copy));
	while(s_maria_table_record_copy.symaddr)
	{
		if(!ARD_strcmp(name,s_maria_table_record_copy.symbol))
		{
			return idx;
		}

		idx++;
		memcpy_P(&s_maria_table_record_copy, &(maria_table[idx]), sizeof(s_maria_table_record_copy));
	}
#else
	while(maria_table[idx].symaddr)
	{
		if(!ARD_strcmp(name,maria_table[idx].symbol))
		{
			return idx;
		}

		idx++;
	}
#endif
	return -1;
}

void maria_prompt(int *pfd_r, int *pfd_w, MARIA_USER_DATA *user_data)
{
	char add_cr[2] = {'\r',0};

	if(user_data->mariaFlags & MARIA_FLAGS_NOPROMPT) return;

	if(MARIA_ADD_CR)
	{
		maria_write(*pfd_w,(void *)add_cr,ARD_strlen(add_cr),user_data);
	}
	add_cr[0]='\n';
	maria_write(*pfd_w,(void *)add_cr,ARD_strlen(add_cr),user_data);
	maria_write(*pfd_w,(void *)prompt,ARD_strlen(prompt),user_data);

#if defined (MARIA_TX_K)
	add_cr[0]=MARIA_TX_K;
	maria_write(*pfd_w,(void *)add_cr, 1,user_data);
#endif
}

#if 0
void maria_prompt0(void)
{
	ARD_strcpy(prompt,"->");
}

void maria_prompt1(void)
{
	ARD_strcpy(prompt,">>");
}

void maria_user_prompt(char *user_prompt)
{
	ARD_strncpy(prompt,user_prompt,7);
	prompt[7] = 0;
}
#endif

static int maria_handleVar(GENPTR pvar,MARIA_PARSED_LINE *pArgv)
{
	int retval=0;

	switch(pArgv->argc)
	{
	case 1:
		/* just return value */
		break;
	case 2:
	default:
		/* assignements */
		/* example: var=0x100 */
		/* example: var = 0x100 */
		switch(pArgv->type[1])
		{
		default:
		case MARIA_TYPE_INT:
			*((int *)pvar) = pArgv->lValue[1];
			break;
		case MARIA_TYPE_CHAR:
			*((char *)pvar) = pArgv->lValue[1];
			break;
		case MARIA_TYPE_SHORT:
			*((short *)pvar) = pArgv->lValue[1];
			break;
		case MARIA_TYPE_SYMBOL:
		case MARIA_TYPE_STRING:
			break;
		}
		break;
	}

	switch(pArgv->type[1])
	{
	default:
	case MARIA_TYPE_INT:
		retval = *((int *)pvar);
		break;
	case MARIA_TYPE_CHAR:
		retval = *((char *)pvar);
		break;
	case MARIA_TYPE_SHORT:
		retval = *((short *)pvar);
		break;
	case MARIA_TYPE_SYMBOL:
	case MARIA_TYPE_STRING:
		break;
	}
	return retval;
}

