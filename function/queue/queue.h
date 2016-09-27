/*
 * =====================================================================================
 *
 *       Filename:  queue.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/24/2016 03:41:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  LGmark 
 *           Mail:  life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _QUEUE_H
#define _QUEUE_H
#include <pthread.h>


struct queue_node_st
{
	void *data;
	struct queue_node_st * link;
};

struct queue_head_node_st
{
	int count;
	int maxsize;
	pthread_mutex_t mutex;
	struct queue_node_st *front;
	struct queue_node_st *rear;
	int  em_size;
};

typedef struct queue_head_node_st QUEUE_HEAD_NODE;
typedef struct queue_node_st QUEUE_NODE;

int Queue_init(QUEUE_HEAD_NODE **queue_head_ptr, int maxsize, int em_size);
int Queue_size(QUEUE_HEAD_NODE *queue_head_ptr);
int Queue_push(QUEUE_HEAD_NODE *queue_head_ptr, void *element);
int Queue_pop(QUEUE_HEAD_NODE *queue_head_ptr);
int Queue_getfront(QUEUE_HEAD_NODE *queue_head_ptr, void *element);
int Queue_getrear(QUEUE_HEAD_NODE *queue_head_ptr, void *element);
int Queue_clear(QUEUE_HEAD_NODE *queue_head_ptr);
int Queue_destroy(QUEUE_HEAD_NODE **queue_head_ptr);


#endif
