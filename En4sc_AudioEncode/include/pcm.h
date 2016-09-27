#ifndef _PCM_H_
#define _PCM_H_

#include "pcm_queue.h"

#define  FIFOMODE       (O_CREAT | O_RDWR | O_NONBLOCK)
#define  OPENMODE       (O_RDWR | O_NONBLOCK)

#define PCM_MAX_FRAME_LEN   0x4000

#define PCM_MAX_BUFFER_NUM  100
#define READ_PCM_TIMEOUT     4
#define PCM_FRAME_NUM        1  /* 每个PCM包，包含的音频帧个数 */


int 	Pcm_create_task(void);

void * 	Pcm_capture_task(void *arg);

int 	Pcm_configure(int port);
int 	Pcm_get_data(int port);
int  	Pcm_init(void);
int 	Pcm_init_frame(int port);
int		Pcm_get_one_frame(int port, PCM_FRAME_T  pstPCMFrame, int pcmlen);
int     Pcm_save_frame(int port, int framelen);

#endif

