/*************************************************************************
  > File Name: pcm_queue.c
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

#include "log.h"
#include "pcm_queue.h"

#define SUCCESS (0)
#define ERROR	(-1)

PCM_HEAD_NODE_T g_PcmHeadNode[4];

int pcm_queue_init(PCM_HEAD_NODE_T *pcm_queue, int size)
{
	//assert(size > 0);
	if(size <= 0)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_init: size <=0\n");
		exit(-1);
	}
	
	*pcm_queue = (PCM_HEAD_NODE_T) malloc(sizeof(struct PCM_HEAD_NODE_T));
	//assert(*pcm_queue);
	if(NULL == *pcm_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_init call malloc\n");
		exit(-1);
	}

	(*pcm_queue)->count = 0;
	(*pcm_queue)->max_size = size;
	(*pcm_queue)->front = NULL;
	(*pcm_queue)->rear = NULL;

	int ret = pthread_mutex_init(&(*pcm_queue)->mutex, NULL);
	//assert(0 == ret);
	if(0 != ret)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_init call pthread_mutex_init\n");
		exit(-1);
	}

	return SUCCESS;
}


int pcm_queue_size(PCM_HEAD_NODE_T pcm_queue)
{
	//assert(pcm_queue);
	if(NULL == pcm_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_size \n");
		exit(-1);
	}

	return pcm_queue->count;
}


int pcm_queue_clear(PCM_HEAD_NODE_T pcm_queue)
{
	//assert(pcm_queue);
	if(NULL == pcm_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_clear \n");
		exit(-1);
	}

	PCM_FRAME_T  tmp = pcm_queue->front;

	pthread_mutex_lock(&(pcm_queue->mutex));

	while(pcm_queue->count > 0)
	{
		pcm_queue->front = pcm_queue->front->link;
		free(tmp->emData);
		tmp->emData = NULL;
		free(tmp);
		tmp = NULL;
		tmp = pcm_queue->front;
		pcm_queue->count--;
	}

	pcm_queue->front = pcm_queue->rear = NULL;
	pcm_queue->count = 0;

	pthread_mutex_unlock(&(pcm_queue->mutex));

	return SUCCESS;
}


int pcm_queue_push(PCM_HEAD_NODE_T pcm_queue, PCM_FRAME_T pcm_data)
{
	//assert(pcm_queue && pcm_data);
	if(NULL == pcm_queue || NULL == pcm_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_push \n");
		exit(-1);
	}

	if(pcm_queue->count >=  pcm_queue->max_size)
	{
		return ERROR;
	}

	PCM_FRAME_T tmp;
	tmp = (PCM_FRAME_T)malloc(sizeof(*pcm_data));
	//assert(tmp);
	if(NULL == tmp)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_push \n");
		exit(-1);
	}

	
	tmp->emData = (unsigned char *)malloc(pcm_data->emDataLen);
	//assert(tmp->emData);
	if(NULL == tmp->emData)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_push \n");
		exit(-1);
	}

	tmp->link	   = NULL;
	tmp->emDataLen = pcm_data->emDataLen;
	memcpy(tmp->emData, pcm_data->emData, tmp->emDataLen);
	memcpy(tmp->emPTS, pcm_data->emPTS, 8);

	pthread_mutex_lock(&(pcm_queue->mutex));
	pcm_queue->count++;
	if(NULL == pcm_queue->front && NULL == pcm_queue->rear)
	{
		pcm_queue->front = pcm_queue->rear = tmp;
	}
	else
	{
		pcm_queue->rear->link = tmp;
		pcm_queue->rear       = tmp;
	}
	pthread_mutex_unlock(&(pcm_queue->mutex));

	return SUCCESS;
}


int pcm_queue_getfront(PCM_HEAD_NODE_T pcm_queue, PCM_FRAME_T pcm_data)
{
	//assert(pcm_queue && pcm_data);
	
	if(NULL == pcm_queue || NULL == pcm_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_getfront \n");
		exit(-1);
	}

	if(pcm_queue->count <= 0)
	{
		return ERROR;
	}

	pcm_data->emDataLen = pcm_queue->front->emDataLen;
	memcpy(pcm_data->emData, pcm_queue->front->emData, pcm_data->emDataLen);
	memcpy(pcm_data->emPTS, pcm_queue->front->emPTS, 8);

	return SUCCESS;
}

int pcm_queue_pop(PCM_HEAD_NODE_T pcm_queue)
{
	//assert(pcm_queue);
	if(NULL == pcm_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_pop \n");
		exit(-1);
	}

	if(pcm_queue->count <= 0)
	{
		return ERROR;
	}

	pthread_mutex_lock(&(pcm_queue->mutex));
	if(1 == pcm_queue->count)
	{
		free(pcm_queue->front->emData);
		pcm_queue->front->emData = NULL;

		free(pcm_queue->front);
		pcm_queue->front = pcm_queue->rear = NULL;

		--pcm_queue->count;
	}
	else
	{
		PCM_FRAME_T tmp = pcm_queue->front;
		pcm_queue->front = pcm_queue->front->link;

		free(tmp->emData);
		tmp->emData = NULL;

		free(tmp);
		tmp = NULL;

		--pcm_queue->count;
	}

	pthread_mutex_unlock(&(pcm_queue->mutex));

	return SUCCESS;
}

int pcm_queue_getrear(PCM_HEAD_NODE_T pcm_queue, PCM_FRAME_T pcm_data)
{
	//assert(pcm_queue && pcm_data);
	
	if(NULL == pcm_queue || NULL == pcm_data)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_getrear \n");
		exit(-1);
	}

	if(pcm_queue->count <= 0)
	{
		return ERROR;
	}

	pcm_data->emDataLen = pcm_queue->rear->emDataLen;
	memcpy(pcm_data->emData, pcm_queue->rear->emData, pcm_data->emDataLen);

	memcpy(pcm_data->emPTS, pcm_queue->rear->emPTS, 8);

	return SUCCESS;
}


int pcm_queue_destroy(PCM_HEAD_NODE_T *pcm_queue)
{
	//assert((*pcm_queue) && pcm_queue);
	if(NULL == *pcm_queue || NULL == pcm_queue)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "Error: pcm_queue_getrear \n");
		exit(-1);
	}
	pcm_queue_clear(*pcm_queue);

	pthread_mutex_destroy(&((*pcm_queue)->mutex));
	free(*pcm_queue);
	*pcm_queue = NULL;

	return SUCCESS;
}





















