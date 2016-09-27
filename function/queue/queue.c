/*
 * =====================================================================================
 *
 *       Filename:  queue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/25/2016 12:53:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"


int Queue_init(QUEUE_HEAD_NODE **queue_head_ptr, int maxsize, int em_size)
{
	if(maxsize <= 0 || em_size <= 0)
	{
		exit(-1);
	}

	*queue_head_ptr = (QUEUE_HEAD_NODE *)malloc(sizeof(QUEUE_HEAD_NODE));
	if(NULL == *queue_head_ptr)
	{
		exit(-1);
	}

	(*queue_head_ptr)->count = 0;
	(*queue_head_ptr)->maxsize = maxsize;
	(*queue_head_ptr)->front = NULL;
	(*queue_head_ptr)->rear = NULL;
	(*queue_head_ptr)->em_size = em_size;

	int ret = pthread_mutex_init(&((*queue_head_ptr)->mutex), NULL);
	if(ret < 0)
	{
		exit(-1);
	}

	return 0;
}


int Queue_size(QUEUE_HEAD_NODE *queue_head_ptr)
{
	if(NULL == queue_head_ptr)
	{
		exit(-1);
	}

	return queue_head_ptr->count;
}


int Queue_push(QUEUE_HEAD_NODE *queue_head_ptr, void *element)
{
	if(NULL == queue_head_ptr || NULL == element)
	{
		exit(-1);
	}
	
	if(queue_head_ptr->count >= queue_head_ptr->maxsize)
	{
		return -1;
	}

	QUEUE_NODE *queue_node = NULL;
	queue_node = (QUEUE_NODE *)malloc(sizeof(QUEUE_NODE));
	if(NULL == queue_node)
	{
		exit(-1);
	}

	queue_node->link = NULL;
	queue_node->data = (void *)malloc(queue_head_ptr->em_size);
	if(NULL == queue_node->data)
	{
		free(queue_node);
		exit(-1);
	}

	memcpy(queue_node->data, element, queue_head_ptr->em_size);

	pthread_mutex_lock(&(queue_head_ptr->mutex));
	if(NULL == queue_head_ptr->front && NULL == queue_head_ptr->rear)
	{
		queue_head_ptr->front = queue_head_ptr->rear = queue_node;
	}
	else
	{
		queue_head_ptr->rear->link = queue_node;
		queue_head_ptr->rear = queue_node;
	}

	++(queue_head_ptr->count);
	pthread_mutex_unlock(&(queue_head_ptr->mutex));

	return 0;
}


int Queue_pop(QUEUE_HEAD_NODE *queue_head_ptr)
{
	if(NULL == queue_head_ptr)
	{
		exit(-1);
	}
	
	if(queue_head_ptr->count <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(queue_head_ptr->mutex));
	if(1 == queue_head_ptr->count)
	{
		free(queue_head_ptr->front->data);
		free(queue_head_ptr->front);
		queue_head_ptr->front = queue_head_ptr->rear = NULL;
	}
	else
	{
		QUEUE_NODE * tmp = queue_head_ptr->front;
		queue_head_ptr->front = queue_head_ptr->front->link;

		free(tmp->data);
		free(tmp);
		tmp = NULL;
	}
	--(queue_head_ptr->count);

	pthread_mutex_unlock(&(queue_head_ptr->mutex));

	return 0;
}


int Queue_getfront(QUEUE_HEAD_NODE *queue_head_ptr, void *element)
{
	if(NULL == queue_head_ptr || NULL == element)
	{
		exit(-1);
	}

	if(queue_head_ptr->count <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(queue_head_ptr->mutex));
	memcpy(element, queue_head_ptr->front->data, queue_head_ptr->em_size);
	pthread_mutex_unlock(&(queue_head_ptr->mutex));

	return 0;
}


int Queue_getrear(QUEUE_HEAD_NODE *queue_head_ptr, void *element)
{
	if(NULL == queue_head_ptr || NULL == element)
	{
		exit(-1);
	}

	if(queue_head_ptr->count <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(queue_head_ptr->mutex));
	memcpy(element, queue_head_ptr->rear->data, queue_head_ptr->em_size);
	pthread_mutex_unlock(&(queue_head_ptr->mutex));

	return 0;
}

int Queue_clear(QUEUE_HEAD_NODE *queue_head_ptr)
{
	if(NULL == queue_head_ptr)
	{
		exit(-1);
	}

	QUEUE_NODE *tmp = NULL;

	pthread_mutex_lock(&(queue_head_ptr->mutex));
	while(queue_head_ptr->count)
	{
		tmp = queue_head_ptr->front;
		queue_head_ptr->front = queue_head_ptr->front->link;

		free(tmp->data);
		free(tmp);

		--(queue_head_ptr->count);
	}
	pthread_mutex_unlock(&(queue_head_ptr->mutex));

	return 0;
}


int Queue_destroy(QUEUE_HEAD_NODE **queue_head_ptr)
{
	if(NULL == *queue_head_ptr || NULL == queue_head_ptr)
	{
		exit(-1);
	}

	Queue_clear(*queue_head_ptr);

	pthread_mutex_destroy(&((*queue_head_ptr)->mutex));
	free(*queue_head_ptr);
	*queue_head_ptr = NULL;

	return 0;
}


