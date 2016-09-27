#ifndef _ES_H
#define _ES_H

#include "es_queue.h"

#define ES_MAX_BUFFER_NUM 100
//#define FRAME_NUM_PER_PES 1

int  	Es_init(void);
int 	Es_init_frame(int port);
int		Es_get_one_frame(int port, ES_FRAME_T  pstESFrame, int pcmlen);
int     Es_save_frame(int port, ES_FRAME_T  pstESFrame);

#endif
