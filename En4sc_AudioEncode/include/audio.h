#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "ts_queue.h"

#define AUDIO_ENCODER_NUM_4     4


#define MP2_BUF_SIZE		(0x8000)
#define AUDIO_BUF_SIZE		(0x8000)

extern int g_AudioFd[4];
extern int g_AudioDelay[4];

extern pthread_mutex_t pcm_mutex;


int create_audio_encode_task(int *data);

void * Audio_encoder_task(void *arg);


int Audio_send_ts_packet(TS_FRAME_T pstTsFrame, int port);
int Audio_send_data_logic(unsigned char *data);



int  Audio_init_encoder_status(void);
int  Audio_init_ffmpeg(void);


int Audio_init_eventfd(void);

int Audio_rcv_pcm(int port);
int Audio_wait_pcm(int port);

int Audio_clear_buffer(int port);

int Audio_get_sample_rate(int port, int *s32SampleRate);

int Audio_get_bit_rate(int port, int *s32BitRate);

int Audio_muxer(int port);

int Audio_wr_pcr_pid(int port);

int Audio_mutex_init(void);
int Audio_buffer_init(void);

int Audio_get_delay(int port);

int Audio_get_mpeg2_delay(int audio_bit_rate, int video_encode_rate);
int Audio_get_h264_delay(int audio_bit_rate, int video_encode_rate);
#endif



