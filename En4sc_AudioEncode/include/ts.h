#ifndef _TS_H
#define _TS_H

#include "ts_queue.h"

#define TS_MAX_BUFFER_NUM 4000


int  	Ts_init(void);
int 	Ts_init_frame(int port);
int     Ts_save_frame(int port, TS_FRAME_T  pstTsFrame);
int     Ts_get_one_frame(int port, TS_FRAME_T  pstTsFrame);

int		Ts_del_one_frame(int port );

#endif