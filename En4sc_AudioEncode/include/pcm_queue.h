/*************************************************************************
	> File Name: pcm_queue.h
	> Author: LGmark
	> Mail: life_is_legend@163.com 
	> Created Time: Thu 21 Jul 2016 10:39:50 PM PDT
 ************************************************************************/

#ifndef _PCM_QUEUE_H
#define _PCM_QUEUE_H
#include <pthread.h>

struct pcm_frame
{
	unsigned int emDataLen;
	unsigned char	*emData;
	unsigned int emPTS[2];
	struct pcm_frame *link;
};

struct PCM_HEAD_NODE_T
{
	int count;
	int max_size;
	pthread_mutex_t mutex;
	struct pcm_frame *front, *rear;
};

typedef struct PCM_HEAD_NODE_T * PCM_HEAD_NODE_T;
typedef struct pcm_frame       * PCM_FRAME_T;

extern PCM_HEAD_NODE_T g_PcmHeadNode[4];


int pcm_queue_init(PCM_HEAD_NODE_T *pcm_queue, int size);
int pcm_queue_size(PCM_HEAD_NODE_T  pcm_queue);
int pcm_queue_clear(PCM_HEAD_NODE_T pcm_queue);
int pcm_queue_push(PCM_HEAD_NODE_T  pcm_queue,PCM_FRAME_T  pcm_data);
int pcm_queue_getfront(PCM_HEAD_NODE_T pcm_queue, PCM_FRAME_T pcm_data);
int pcm_queue_pop(PCM_HEAD_NODE_T   pcm_queue);
int pcm_queue_getrear(PCM_HEAD_NODE_T pcm_queue, PCM_FRAME_T pcm_data);
int pcm_queue_destroy(PCM_HEAD_NODE_T *pcm_queue);


#endif
