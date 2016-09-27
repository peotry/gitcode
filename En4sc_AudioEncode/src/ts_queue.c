/*************************************************************************
  > File Name: ts_queue.c
  > Author: LGmark
  > Mail: life_is_legend@163.com 
  > Created Time: Thu 21 Jul 2016 11:01:06 PM PDT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "ts_queue.h"
#include "log.h"

#define SUCCESS (0)
#define ERROR	(-1)

TS_HEAD_NODE_T g_TsHeadNode[4];

int ts_queue_init(TS_HEAD_NODE_T *ts_queue, int size)
{
	//assert(size > 0);
	if(size <= 0)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_init: size <=0\n");
		exit(-1);
	}
	*ts_queue = (TS_HEAD_NODE_T) malloc(sizeof(struct TS_HEAD_NODE_T));
	//assert(NULL != *ts_queue);
	if(NULL == *ts_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_init call malloc\n");
		exit(-1);
	}

	(*ts_queue)->count = 0;
	(*ts_queue)->max_size = size;
	(*ts_queue)->front = NULL;
	(*ts_queue)->rear = NULL;

	int ret = pthread_mutex_init(&(*ts_queue)->mutex, NULL);
	//assert(0 == ret);
	if(0 != ret)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_init call pthread_mutex_init\n");
		exit(-1);
	}

	return SUCCESS;
}


int ts_queue_size(TS_HEAD_NODE_T ts_queue)
{
	//assert(ts_queue);
	if(NULL == ts_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_size\n");
		exit(-1);
	}
	

	return ts_queue->count;
}


int ts_queue_clear(TS_HEAD_NODE_T ts_queue)
{
	//assert(ts_queue);
	if(NULL == ts_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_clear\n");
		exit(-1);
	}

	TS_FRAME_T  tmp = ts_queue->front;

	pthread_mutex_lock(&(ts_queue->mutex));

	while(ts_queue->count > 0)
	{
		ts_queue->front = ts_queue->front->link;
		free(tmp);
		tmp = NULL;
		tmp = ts_queue->front;
		ts_queue->count--;
	}

	ts_queue->front = ts_queue->rear = NULL;
	ts_queue->count = 0;

	pthread_mutex_unlock(&(ts_queue->mutex));

	return SUCCESS;
}


int ts_queue_push(TS_HEAD_NODE_T ts_queue, TS_FRAME_T ts_data)
{
	//assert(ts_queue && ts_data);
	if(NULL == ts_queue || NULL == ts_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_push\n");
		exit(-1);
	}

	if(ts_queue->count >=  ts_queue->max_size)
	{
		return ERROR;
	}

	TS_FRAME_T tmp;
	tmp = (TS_FRAME_T)malloc(sizeof(*ts_data));
	//assert(tmp);
	if(NULL == tmp)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_push\n");
		exit(-1);
	}

	tmp->link	   = NULL;
	memcpy(tmp->emData, ts_data->emData, 188);
	memcpy(tmp->emPTS, ts_data->emPTS, 8);
	//tmp->emPTS = ts_data->emPTS;

	pthread_mutex_lock(&(ts_queue->mutex));
	ts_queue->count++;
	if(NULL == ts_queue->front && NULL == ts_queue->rear)
	{
		ts_queue->front = ts_queue->rear = tmp;
	}
	else
	{
		ts_queue->rear->link = tmp;
		ts_queue->rear       = tmp;
	}
	pthread_mutex_unlock(&(ts_queue->mutex));

	return SUCCESS;
}


int ts_queue_getfront(TS_HEAD_NODE_T ts_queue, TS_FRAME_T ts_data)
{
	//assert(ts_queue && ts_data);
	
	
	if(NULL == ts_queue || NULL == ts_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_getfront\n");
		exit(-1);
	}

	if(ts_queue->count <= 0)
	{
		return ERROR;
	}

	memcpy(ts_data->emData, ts_queue->front->emData, 188);
	memcpy(ts_data->emPTS, ts_queue->front->emPTS, 8);
	//ts_data->emPTS = ts_queue->front->emPTS;

	return SUCCESS;
}

int ts_queue_pop(TS_HEAD_NODE_T ts_queue)
{
	//assert(ts_queue);
	if(NULL == ts_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_pop\n");
		exit(-1);
	}

	if(ts_queue->count <= 0)
	{
		return ERROR;
	}

	pthread_mutex_lock(&(ts_queue->mutex));
	if(1 == ts_queue->count)
	{
		free(ts_queue->front);
		ts_queue->front = ts_queue->rear = NULL;

		--ts_queue->count;
	}
	else
	{
		TS_FRAME_T tmp = ts_queue->front;
		ts_queue->front = ts_queue->front->link;

		free(tmp);
		tmp = NULL;

		--ts_queue->count;
	}

	pthread_mutex_unlock(&(ts_queue->mutex));

	return SUCCESS;
}

int ts_queue_getrear(TS_HEAD_NODE_T ts_queue, TS_FRAME_T ts_data)
{
	//assert(ts_queue && ts_data);
	if(NULL == ts_queue || NULL == ts_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_getrear\n");
		exit(-1);
	}

	if(ts_queue->count <= 0)
	{
		return ERROR;
	}

	memcpy(ts_data->emData, ts_queue->rear->emData, 188);

	memcpy(ts_data->emPTS, ts_queue->rear->emPTS, 8);
	//ts_data->emPTS = ts_queue->rear->emPTS;

	return SUCCESS;
}


int ts_queue_destroy(TS_HEAD_NODE_T *ts_queue)
{
	//assert((*ts_queue) && ts_queue);
	if(NULL == *ts_queue || NULL == ts_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: ts_queue_getrear\n");
		exit(-1);
	}

	ts_queue_clear(*ts_queue);

	pthread_mutex_destroy(&((*ts_queue)->mutex));
	free(*ts_queue);
	*ts_queue = NULL;

	return SUCCESS;
}





















