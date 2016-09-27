#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <unistd.h>
#include "appGlobal.h"

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <sysexits.h>


//#define LOG_BUFFER_FILE   "/var/log/message"
//#define LOG_BACKUP_FILE   "/var/log/message.0"

#define MAX_LINE_LENGTH     1024
#define LOG_STVER           0x1
   
#define LOG_BUFFER_FILE   "/var/log/messages"
#define LOG_BACKUP_FILE   "/var/log/messages.0"
#define LOG_SAVE_FILE     WELLAV_DATA"log/wellav.log"
#define LOG_SAVE_DIR      WELLAV_DATA"log"

//#define MAX_MODULE_NUM      27   
#define MAX_MODULE_NAME_LEN 50

typedef enum{
    LOG_EN_LEVEL_DEBUG      = 0,
    LOG_EN_LEVEL_ALARM      = 1, 
    LOG_EN_LEVEL_ERROR      = 2,
    LOG_EN_LEVEL_EMERGENCY  = 3,

    LOG_EN_LEVEL_CONSOLE    = 4,
    LOG_EN_LEVEL_SYSLOG     = 5,

    LOG_EN_MODULE_SYS       = 6,
    LOG_EN_MODULE_VIDEO     = 7,
    LOG_EN_MODULE_AUDIO     = 8,
}peLogControl;

/*log start*/
#define LOG_MAX_LENGTH          256
#define LOG_SLEEP               2
#define LOG_INTERVAL_DEFAULT    4
#define LOG_SWITCH              (0x1 << 31)

/* bit31 = 0 */
#define LOG_LEVEL_ALL               (0xF)
#define LOG_LEVEL_DEBUG             (0x1 << 0)
#define LOG_LEVEL_ALARM             (0x1 << 1)
#define LOG_LEVEL_ERROR             (0x1 << 2)
#define LOG_LEVEL_EMERGENCY         (0x1 << 3)

#define LOG_OUTPUT_ALL              (0x30)
#define LOG_OUTPUT_CONSOLE          (0x1 << 4)
#define LOG_OUTPUT_SYSLOG           (0x1 << 5)

#define LOG_MODULE_ALL              (0x7FFFFFC0)//(0x7FFFFE00)
#define LOG_MODULE_SYS              (0x1 << 6)
#define LOG_MODULE_VIDEO            (0x1 << 7)
#define LOG_MODULE_AUDIO            (0x1 << 8)
#define LOG_MODULE_STATE            (0x1 << 9)
#define LOG_MODULE_ENCODE           (0x1 << 10)
#define LOG_MODULE_COMM             (0x1 << 11)


/* bit31 = 1 */
#define LOG_LEVEL_ALL_OFF           ~(0xF)
#define LOG_LEVEL_DEBUG_OFF         ~(0x1 << 0)
#define LOG_LEVEL_ALARM_OFF         ~(0x1 << 1)
#define LOG_LEVEL_ERROR_OFF         ~(0x1 << 2)
#define LOG_LEVEL_EMERGENCY_OFF     ~(0x1 << 3)

#define LOG_OUTPUT_ALL_OFF          ~(0x30)
#define LOG_OUTPUT_CONSOLE_OFF      ~(0x1 << 4)
#define LOG_OUTPUT_SYSLOG_OFF       ~(0x1 << 5)

#define LOG_MODULE_ALL_OFF          ~(0x7FFFFFC0)
#define LOG_MODULE_SYS_OFF          ~(0x1 << 6)
#define LOG_MODULE_VIDEO_OFF        ~(0x1 << 7)
#define LOG_MODULE_AUDIO_OFF        ~(0x1 << 8)
#define LOG_MODULE_STATE_OFF        ~(0x1 << 9)
#define LOG_MODULE_ENCODE_OFF       ~(0x1 << 10)
#define LOG_MODULE_COMM_OFF         ~(0x1 << 11)




#define ASSERT_APP(isOk, comment, ...)\
	{\
		if(!(isOk)){\
			assert_failed(__FILE__, __LINE__, __FUNCTION__, comment, ## __VA_ARGS__);\
		}\
	}

#define ERR_MSG(fmt, ...)		fprintf (stderr, "(%s (%d))\n" "ERR: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#define INFO_MSG(fmt, ...)		fprintf (stderr, fmt, ## __VA_ARGS__)

#define MAX(a, b)		(((a) > (b)) ? (a) : (b))

/**
 *	@brief		Assert function
 *	@param[in]	file_p		file name that assertion failed
 *	@param[in]	line		line number that assertion failed
 *	@param[in]	function_p	function name that assertion failed
 *	@param[in]	comment_p	string of printing
 *	@attention	None
 */
void assert_failed ( const char* file_p, int line,
					 const char* function_p, const char* comment_p, ... );






struct he_log_ctrl
{
    U32  u32LogControl;
    U32  u32LogSaveInter;
};

/*log end*/
void log_InitCtrlHandler(void);
void log_printf( U32 Loglevel, U32 u32Modulemask, const char *format,...);
void log_Control(U32 u32Log);
void log_TimeMark(const char *format,...);
#endif


