/*************************************************************************
	> File Name: ts_queue.h
	> Author: LGmark
	> Mail: life_is_legend@163.com 
	> Created Time: Thu 21 Jul 2016 10:39:50 PM PDT
 ************************************************************************/

#ifndef _TS_QUEUE_H
#define _TS_QUEUE_H

#include <pthread.h>
#include "appGlobal.h"

struct ts_frame
{
	unsigned char	 emData[188];
	unsigned int emPTS[2];
	struct ts_frame *link;
};

struct TS_HEAD_NODE_T
{
	int count;
	int max_size;
	pthread_mutex_t mutex;
	struct ts_frame *front, *rear;
};

typedef struct TS_HEAD_NODE_T * TS_HEAD_NODE_T;
typedef struct ts_frame       * TS_FRAME_T;

extern TS_HEAD_NODE_T g_TsHeadNode[4];


int ts_queue_init(TS_HEAD_NODE_T *ts_queue, int size);
int ts_queue_size(TS_HEAD_NODE_T  ts_queue);
int ts_queue_clear(TS_HEAD_NODE_T ts_queue);
int ts_queue_push(TS_HEAD_NODE_T  ts_queue,TS_FRAME_T  ts_data);
int ts_queue_getfront(TS_HEAD_NODE_T ts_queue, TS_FRAME_T ts_data);
int ts_queue_pop(TS_HEAD_NODE_T   ts_queue);
int ts_queue_getrear(TS_HEAD_NODE_T ts_queue, TS_FRAME_T ts_data);
int ts_queue_destroy(TS_HEAD_NODE_T *ts_queue);


#endif
