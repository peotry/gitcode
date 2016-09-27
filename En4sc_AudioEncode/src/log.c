#include <semaphore.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include "log.h"

struct he_log_ctrl  s_LogCtrl;

char log_NameArrayName[][MAX_MODULE_NAME_LEN] = {
	"DEBUG",
	"ALARM",
	"ERROR",   
	"EMERGENCY",   
	"CONSOLE",   
	"LOG",   
	"SYS",   
	"VIDEO",   
	"AUDIO",   
	"STATE",   
	"ENCODE",   
	"COMM",   
	"USB",   
	"LCD",   
	"WEB",   
	"NET",   
	"PLAYLIST",   
	"FIFO",
	"RTC",  
	"TABEL",  
	"TSIP",  
	"DMA",  
	"SIPROCESS",  
	"USER",
	"IPINPUT",
	"WIFI",
	"LICENSE",
	"INIT",
	"RTMP",
	"NULL"     /* 结束标志 */
};

/****************************************************************************
 *
 * NAME
 *      void log_InitCtrlHandler()
 *
 * PURPOSE:
 *      Initialize log-ctrol handler
 *
 * USAGE
 *      This routine is C-callable and can be called as:
 *
 *              void he_log_ctrl()
 *
 *
 * RETURN VALUE
 *      - Node
 *
 * REFERENCE
 *      - None
 *
 * AUTHOR:
 *      - dadi.zeng
 *
 ****************************************************************************/
void  log_InitCtrlHandler()
{
	log_Control(LOG_LEVEL_ALL | LOG_OUTPUT_ALL | LOG_MODULE_ALL);
}



void assert_failed ( const char* file_p, int line,
		const char* function_p, const char* comment_p, ... )
{
	int cur_errno;
	va_list args;

	cur_errno = errno;

	fprintf ( stderr, "\n##################################################\n" );
	fprintf ( stderr, "### ASSERT FAILED %s(%i):%s\n", file_p, line, function_p );

	fprintf ( stderr, "### " );
	va_start ( args, comment_p );
	vfprintf ( stderr, comment_p, args );
	va_end ( args );
	fprintf ( stderr, "\n" );

	if ( cur_errno != 0 ) {
		fprintf ( stderr, "### errno:%d(%s)\n", cur_errno, strerror ( cur_errno ) );
	}
	fprintf ( stderr, "##################################################\n\n" );

	fflush ( stderr );

	//exit ( EXIT_FAILURE );
}



/****************************************************************************
 *
 * NAME
 *      void he_log_ctrl()
 *
 * PURPOSE:
 *     Get log ctrol handler
 *
 * USAGE
 *      This routine is C-callable and can be called as:
 *
 *              void he_log_ctrl()
 *
 *
 * RETURN VALUE
 *      - Log ctrol handler
 *
 * REFERENCE
 *      - None
 *
 * AUTHOR:
 *      - dadi.zeng
 *
 ****************************************************************************/
struct he_log_ctrl   *   log_GetCtrlHandler()
{
	return &(s_LogCtrl);
}


/**************************************************************************** 
 *
 * NAME  
 *      void log_Control(U32 u32Log)
 *
 * PURPOSE:
 *      Control print behavior.
 *
 * USAGE
 *      This routine is C-callable and can be called as:
 *
 *              void log_Control(U32 u32Log)
 *      
 *      u32Log               - (i)   control mask                  
 *
 * RETURN VALUE
 *      - None
 *
 * REFERENCE 
 *      - None
 *
 * AUTHOR:
 *      - dadi.zeng
 *              
 ****************************************************************************/ 
void log_Control(U32 u32Log)
{
	struct he_log_ctrl * pstCtrl;
	pstCtrl = log_GetCtrlHandler();

	// bit31 = 0 表明为打开 否则为关闭
	if(0 == (LOG_SWITCH & u32Log))
	{
		pstCtrl->u32LogControl |= u32Log;
	}
	else
	{
		pstCtrl->u32LogControl  &=  u32Log;
	}
}


/**************************************************************************** 
 *
 * NAME  
 *      void log_printf(U32 u32Loglevel, U32 u32Modulemask, const char *format,...)
 *
 * PURPOSE:
 *      Printf information according to setting
 *
 * USAGE
 *      This routine is C-callable and can be called as:
 *
 *              void log_printf(U32 u32Loglevel, U32 u32Modulemask, const char *format,...)
 *      
 *      u32Loglevel               - (i)   print level
 *      u32Modulemask             - (i)   module mask
 *      format                    - (i)   the content will print
 *
 * RETURN VALUE
 *      - None
 *
 * REFERENCE 
 *      - LogPrint (IRD160AD) 
 *
 * AUTHOR:
 *      - dadi.zeng
 *              
 ****************************************************************************/  
void log_printf(U32 u32Loglevel, U32 u32Modulemask, const char *format,...)
{
	va_list     Argument;  
	static U8   u8StringBuff[LOG_MAX_LENGTH];
	U8          u8PrintBuff[LOG_MAX_LENGTH];
	struct he_log_ctrl * pstCtrl;
	pstCtrl = log_GetCtrlHandler();

	memset(u8StringBuff,0,sizeof(u8StringBuff));

	/*  获取待打印的内容 */
	va_start(Argument, format);
	vsprintf((char *)u8StringBuff, format, Argument);
	va_end(Argument);


	//(char *)log_NameArrayName[(U32)((log(u32Modulemask) / log(2)))],

	snprintf((char *)u8PrintBuff, ARRAY_SIZE(u8PrintBuff), "[%s][%s]%s",
			(char *)log_NameArrayName[(U32)((log(u32Loglevel) / log(2)))],
			(char *)log_NameArrayName[(U32)((log(u32Modulemask) / log(2)))],
			(char *)u8StringBuff);

	/*  校验打印控制 */
	if((u32Loglevel & pstCtrl->u32LogControl) && (u32Modulemask & pstCtrl->u32LogControl))
	{
		//控制台是否属打开
		if(LOG_OUTPUT_CONSOLE & pstCtrl->u32LogControl)
		{
			printf("%s",u8PrintBuff);
		}

		//是否输入到日志
		if(LOG_OUTPUT_SYSLOG & pstCtrl->u32LogControl)
		{
			syslog(LOG_INFO,"%s",u8PrintBuff);
		}
	}
}


/**************************************************************************** 
 *
 * NAME  
 *      void log_TimeMark(const char *format,...)
 *
 * PURPOSE:
 *      Record time mark
 *
 * USAGE
 *      This routine is C-callable and can be called as:
 *
 *              void log_TimeMark(const char *format,...)
 *      
 *      format                    - (i)   the content will print
 *
 * RETURN VALUE
 *      - None
 *
 * REFERENCE 
 *      - LogPrint (IRD160AD) 
 *
 * AUTHOR:
 *      - dadi.zeng
 *              
 ****************************************************************************/  
void log_TimeMark(const char *format,...)
{
	va_list     Argument;  
	static U8   u8StringBuff[LOG_MAX_LENGTH];
	U8          u8PrintBuff[LOG_MAX_LENGTH];
	FILE * fp  = NULL;
	char time[MAX_LINE_LENGTH] = {0};

	memset(u8StringBuff,0,sizeof(u8StringBuff));

	/* 获取时间 */
	fp = popen("date", "r");
	fgets(time, MAX_LINE_LENGTH, fp);
	fclose(fp);

	/*  获取待打印的内容 */
	va_start(Argument, format);
	vsprintf((char *)u8StringBuff, format, Argument);
	va_end(Argument);

	/* 输出内容 */
	snprintf((char *)u8PrintBuff, ARRAY_SIZE(u8PrintBuff), "\n%s%s", time,  (char *)u8StringBuff);

	//控制台
	printf("%s",u8PrintBuff);

	//日志
	syslog(LOG_INFO,"%s",u8PrintBuff);
}



