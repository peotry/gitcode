/*
 * =====================================================================================
 *
 *       Filename:  stack.c
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

#include "stack.h"


int Stack_init(STACK_HEAD_NODE **stack_head_ptr, int maxsize, int em_size)
{
	if(maxsize <= 0 || em_size <= 0)
	{
		exit(-1);
	}

	*stack_head_ptr = (STACK_HEAD_NODE *)malloc(sizeof(STACK_HEAD_NODE));
	if(NULL == *stack_head_ptr)
	{
		exit(-1);
	}

	(*stack_head_ptr)->count = 0;
	(*stack_head_ptr)->maxsize = maxsize;
	(*stack_head_ptr)->base = NULL;
	(*stack_head_ptr)->top = NULL;
	(*stack_head_ptr)->em_size = em_size;

	int ret = pthread_mutex_init(&((*stack_head_ptr)->mutex), NULL);
	if(ret < 0)
	{
		exit(-1);
	}

	return 0;
}


int Stack_size(STACK_HEAD_NODE *stack_head_ptr)
{
	if(NULL == stack_head_ptr)
	{
		exit(-1);
	}

	return stack_head_ptr->count;
}


int Stack_push(STACK_HEAD_NODE *stack_head_ptr, void *element)
{
	if(NULL == stack_head_ptr || NULL == element)
	{
		exit(-1);
	}
	
	if(stack_head_ptr->count >= stack_head_ptr->maxsize)
	{
		return -1;
	}

	STACK_NODE *stack_node = NULL;
	stack_node = (STACK_NODE *)malloc(sizeof(STACK_NODE));
	if(NULL == stack_node)
	{
		exit(-1);
	}

	stack_node->link = NULL;
	stack_node->data = (void *)malloc(stack_head_ptr->em_size);
	if(NULL == stack_node->data)
	{
		free(stack_node);
		exit(-1);
	}

	memcpy(stack_node->data, element, stack_head_ptr->em_size);

	pthread_mutex_lock(&(stack_head_ptr->mutex));
	if(NULL == stack_head_ptr->base && NULL == stack_head_ptr->top)
	{
		stack_head_ptr->base = stack_head_ptr->top = stack_node;
	}
	else
	{
		stack_node->link = stack_head_ptr->top;
		stack_head_ptr->top = stack_node;
	}

	++(stack_head_ptr->count);
	pthread_mutex_unlock(&(stack_head_ptr->mutex));

	return 0;
}


int Stack_pop(STACK_HEAD_NODE *stack_head_ptr)
{
	if(NULL == stack_head_ptr)
	{
		exit(-1);
	}
	
	if(stack_head_ptr->count <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(stack_head_ptr->mutex));
	if(1 == stack_head_ptr->count)
	{
		free(stack_head_ptr->base->data);
		free(stack_head_ptr->base);
		stack_head_ptr->base = stack_head_ptr->top = NULL;
	}
	else
	{
		STACK_NODE * tmp = stack_head_ptr->top;
		stack_head_ptr->top = stack_head_ptr->top->link;

		free(tmp->data);
		free(tmp);
		tmp = NULL;
	}
	--(stack_head_ptr->count);

	pthread_mutex_unlock(&(stack_head_ptr->mutex));

	return 0;
}


int Stack_gettop(STACK_HEAD_NODE *stack_head_ptr, void *element)
{
	if(NULL == stack_head_ptr || NULL == element)
	{
		exit(-1);
	}

	if(stack_head_ptr->count <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(stack_head_ptr->mutex));
	memcpy(element, stack_head_ptr->top->data, stack_head_ptr->em_size);
	pthread_mutex_unlock(&(stack_head_ptr->mutex));

	return 0;
}


int Stack_clear(STACK_HEAD_NODE *stack_head_ptr)
{
	if(NULL == stack_head_ptr)
	{
		exit(-1);
	}

	STACK_NODE *tmp = NULL;

	pthread_mutex_lock(&(stack_head_ptr->mutex));
	while(stack_head_ptr->count)
	{
		tmp = stack_head_ptr->top;
		stack_head_ptr->top = stack_head_ptr->top->link;

		free(tmp->data);
		free(tmp);

		--(stack_head_ptr->count);
	}
	pthread_mutex_unlock(&(stack_head_ptr->mutex));

	return 0;
}


int Stack_destroy(STACK_HEAD_NODE **stack_head_ptr)
{
	if(NULL == *stack_head_ptr || NULL == stack_head_ptr)
	{
		exit(-1);
	}

	Stack_clear(*stack_head_ptr);

	pthread_mutex_destroy(&((*stack_head_ptr)->mutex));
	free(*stack_head_ptr);
	*stack_head_ptr = NULL;

	return 0;
}


