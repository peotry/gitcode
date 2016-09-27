/*************************************************************************
  > File Name: es_queue.c
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

#include "es_queue.h"
#include "log.h"


ES_HEAD_NODE_T g_EsHeadNode[4];

int es_queue_init(ES_HEAD_NODE_T *es_queue, int size)
{
	//assert(size > 0);
	if(size <= 0)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_init: size <=0\n");
		exit(-1);
	}
	*es_queue = (ES_HEAD_NODE_T) malloc(sizeof(struct ES_HEAD_NODE_T));
	//assert(NULL != *es_queue);
	if(NULL == *es_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_init call malloc \n");
		exit(-1);
	}

	(*es_queue)->count = 0;
	(*es_queue)->max_size = size;
	(*es_queue)->front = NULL;
	(*es_queue)->rear = NULL;

	int ret = pthread_mutex_init(&(*es_queue)->mutex, NULL);
	//assert(0 == ret);
	
	if(0 != ret)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_init call pthread_mutex_init \n");
		exit(-1);
	}
	

	return SUCCESS;
}


int es_queue_size(ES_HEAD_NODE_T es_queue)
{
	//assert(es_queue);
	if(NULL == es_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_size \n");
		exit(-1);
	}

	return es_queue->count;
}


int es_queue_clear(ES_HEAD_NODE_T es_queue)
{
	//assert(es_queue);
	if(NULL == es_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_clear \n");
		exit(-1);
	}

	ES_FRAME_T  tmp = es_queue->front;

	pthread_mutex_lock(&(es_queue->mutex));

	while(es_queue->count > 0)
	{
		es_queue->front = es_queue->front->link;
		free(tmp->emData);
		tmp->emData = NULL;
		free(tmp);
		tmp = NULL;
		tmp = es_queue->front;
		es_queue->count--;
	}

	es_queue->front = es_queue->rear = NULL;
	es_queue->count = 0;

	pthread_mutex_unlock(&(es_queue->mutex));

	return SUCCESS;
}


int es_queue_push(ES_HEAD_NODE_T es_queue, ES_FRAME_T es_data)
{
	//assert(es_queue && es_data);
	if(NULL == es_queue || NULL == es_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_push \n");
		exit(-1);
	}

	if(es_queue->count >=  es_queue->max_size)
	{
		return ERROR;
	}

	pthread_mutex_lock(&(es_queue->mutex));
	ES_FRAME_T tmp;
	tmp = (ES_FRAME_T)malloc(sizeof(*es_data));
	//assert(tmp);
	if(NULL == tmp)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_push call malloc\n");
		exit(-1);
	}
	
	tmp->emData = (unsigned char *)malloc(es_data->emDataLen);
	//assert(tmp->emData);
	if(NULL == tmp->emData)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_push call malloc\n");
		exit(-1);
	}

	tmp->link	   = NULL;
	tmp->emDataLen = es_data->emDataLen;
	memcpy(tmp->emData, es_data->emData,es_data->emDataLen);
	memcpy(tmp->emPTS, es_data->emPTS, 8);

	//pthread_mutex_lock(&(es_queue->mutex));
	es_queue->count++;
	if(NULL == es_queue->front && NULL == es_queue->rear)
	{
		es_queue->front = es_queue->rear = tmp;
	}
	else
	{
		es_queue->rear->link = tmp;
		es_queue->rear       = tmp;
	}
	pthread_mutex_unlock(&(es_queue->mutex));

	return SUCCESS;
}


int es_queue_getfront(ES_HEAD_NODE_T es_queue, ES_FRAME_T es_data)
{
	//assert(es_queue && es_data);
	if(NULL == es_queue || NULL == es_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_push \n");
		exit(-1);
	}

	if(es_queue->count <= 0)
	{
		return ERROR;
	}

	es_data->emDataLen = es_queue->front->emDataLen;
	memcpy(es_data->emData, es_queue->front->emData, es_data->emDataLen);
	memcpy(es_data->emPTS, es_queue->front->emPTS, 8);

	return SUCCESS;
}

int es_queue_pop(ES_HEAD_NODE_T es_queue)
{
	//assert(es_queue);
	if(NULL == es_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_pop \n");
		exit(-1);
	}

	if(es_queue->count <= 0)
	{
		return ERROR;
	}

	pthread_mutex_lock(&(es_queue->mutex));
	if(1 == es_queue->count)
	{
		free(es_queue->front->emData);
		es_queue->front->emData = NULL;

		free(es_queue->front);
		es_queue->front = es_queue->rear = NULL;

		--es_queue->count;
	}
	else
	{
		ES_FRAME_T tmp = es_queue->front;
		es_queue->front = es_queue->front->link;

		free(tmp->emData);
		tmp->emData = NULL;

		free(tmp);
		tmp = NULL;

		--es_queue->count;
	}

	pthread_mutex_unlock(&(es_queue->mutex));

	return SUCCESS;
}

int es_queue_getrear(ES_HEAD_NODE_T es_queue, ES_FRAME_T es_data)
{
	//assert(es_queue && es_data);
	if(NULL == es_queue || NULL == es_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_getrear \n");
		exit(-1);
	}

	if(es_queue->count <= 0)
	{
		return ERROR;
	}

	es_data->emDataLen = es_queue->rear->emDataLen;
	memcpy(es_data->emData, es_queue->rear->emData, es_data->emDataLen);

	memcpy(es_data->emPTS, es_queue->rear->emPTS, 8);

	return SUCCESS;
}


int es_queue_destroy(ES_HEAD_NODE_T *es_queue)
{
	//assert((*es_queue) && es_queue);
	if(NULL == *es_queue || NULL == es_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: es_queue_destroy \n");
		exit(-1);
	}

	es_queue_clear(*es_queue);

	pthread_mutex_destroy(&((*es_queue)->mutex));
	free(*es_queue);
	*es_queue = NULL;

	return SUCCESS;
}





















