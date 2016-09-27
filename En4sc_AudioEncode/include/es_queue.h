/*************************************************************************
	> File Name: pcm_queue.h
	> Author: LGmark
	> Mail: life_is_legend@163.com 
	> Created Time: Thu 21 Jul 2016 10:39:50 PM PDT
 ************************************************************************/

#ifndef _ES_QUEUE_H
#define _ES_QUEUE_H
#include <pthread.h>

struct es_frame
{
	unsigned int emDataLen;
	unsigned char	*emData;
	unsigned int emPTS[2];
	struct es_frame *link;
};

struct ES_HEAD_NODE_T
{
	int count;
	int max_size;
	pthread_mutex_t mutex;
	struct es_frame *front, *rear;
};

typedef struct ES_HEAD_NODE_T * ES_HEAD_NODE_T;
typedef struct es_frame       * ES_FRAME_T;

#define SUCCESS (0)
#define ERROR	(-1)

extern ES_HEAD_NODE_T g_EsHeadNode[4];


int es_queue_init(ES_HEAD_NODE_T *es_queue, int size);
int es_queue_size(ES_HEAD_NODE_T  es_queue);
int es_queue_clear(ES_HEAD_NODE_T es_queue);
int es_queue_push(ES_HEAD_NODE_T  es_queue,ES_FRAME_T  es_data);
int es_queue_getfront(ES_HEAD_NODE_T es_queue, ES_FRAME_T es_data);
int es_queue_pop(ES_HEAD_NODE_T   es_queue);
int es_queue_getrear(ES_HEAD_NODE_T es_queue, ES_FRAME_T es_data);
int es_queue_destroy(ES_HEAD_NODE_T *es_queue);


#endif
