#define NO_PARSER_PROTOFUN
#define NO_PARSER_PROTODATA
#include <maria_parser.h>
#include <maria_info.h>

extern int GPM;
extern int MARIA_ADD_CR;
extern int Unknown_symbol(void);
extern int dump(void);
extern int dumplong(void);
extern int dumppkt(void);
extern int dumpulong(void);
extern int errmsg;
extern int fillMemory(void);
extern int g_active_fd_index;
extern int maria_data;
extern int maria_finish(void);
extern int maria_fprintf(void);
extern int maria_init(void);
extern int maria_max_fd(void);
extern int maria_parser(void);
extern int maria_printf(void);
extern int maria_prompt(void);
extern int maria_sLine;
extern int memfillbyte(void);
extern int memfilllong(void);
extern int memfillshort(void);
extern int printArgs(void);
extern int prompt;
extern int rem(void);
extern int retval(void);
extern int s_maria_argv;
extern int setPwm(void);
extern int showMaria(void);
extern int showStat(void);
extern int showSym(void);
extern int srecStore(void);


MARIA_TABLE_ATTR MARIA_TABLE_RECORD maria_table[] = {
	{"GPM",MARIA_SYMTYPE_DATA,(GENPTR)&GPM},
	{"MARIA_ADD_CR",MARIA_SYMTYPE_DATA,(GENPTR)&MARIA_ADD_CR},
	{"Unknown_symbol",MARIA_SYMTYPE_FUNCTION,(GENPTR)Unknown_symbol},
	{"dump",MARIA_SYMTYPE_FUNCTION,(GENPTR)dump},
	{"dumplong",MARIA_SYMTYPE_FUNCTION,(GENPTR)dumplong},
	{"dumppkt",MARIA_SYMTYPE_FUNCTION,(GENPTR)dumppkt},
	{"dumpulong",MARIA_SYMTYPE_FUNCTION,(GENPTR)dumpulong},
	{"errmsg",MARIA_SYMTYPE_DATA,(GENPTR)&errmsg},
	{"fillMemory",MARIA_SYMTYPE_FUNCTION,(GENPTR)fillMemory},
	{"g_active_fd_index",MARIA_SYMTYPE_DATA,(GENPTR)&g_active_fd_index},
	{"maria_data",MARIA_SYMTYPE_DATA,(GENPTR)&maria_data},
	{"maria_finish",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_finish},
	{"maria_fprintf",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_fprintf},
	{"maria_init",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_init},
	{"maria_max_fd",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_max_fd},
	{"maria_parser",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_parser},
	{"maria_printf",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_printf},
	{"maria_prompt",MARIA_SYMTYPE_FUNCTION,(GENPTR)maria_prompt},
	{"maria_sLine",MARIA_SYMTYPE_DATA,(GENPTR)&maria_sLine},
	{"memfillbyte",MARIA_SYMTYPE_FUNCTION,(GENPTR)memfillbyte},
	{"memfilllong",MARIA_SYMTYPE_FUNCTION,(GENPTR)memfilllong},
	{"memfillshort",MARIA_SYMTYPE_FUNCTION,(GENPTR)memfillshort},
	{"printArgs",MARIA_SYMTYPE_FUNCTION,(GENPTR)printArgs},
	{"prompt",MARIA_SYMTYPE_DATA,(GENPTR)&prompt},
	{"rem",MARIA_SYMTYPE_FUNCTION,(GENPTR)rem},
	{"retval",MARIA_SYMTYPE_FUNCTION,(GENPTR)retval},
	{"s_maria_argv",MARIA_SYMTYPE_DATA,(GENPTR)&s_maria_argv},
	{"setPwm",MARIA_SYMTYPE_FUNCTION,(GENPTR)setPwm},
	{"showMaria",MARIA_SYMTYPE_FUNCTION,(GENPTR)showMaria},
	{"showStat",MARIA_SYMTYPE_FUNCTION,(GENPTR)showStat},
	{"showSym",MARIA_SYMTYPE_FUNCTION,(GENPTR)showSym},
	{"srecStore",MARIA_SYMTYPE_FUNCTION,(GENPTR)srecStore},

	{"eol",MARIA_SYMTYPE_FUNCTION,(GENPTR)0L},
};

#ifndef NO_MARIA_TABLE_INIT /* maria_table is const */

void init_maria_table(void)
{
     int i=0;


}

#endif 

