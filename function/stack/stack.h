/*
 * =====================================================================================
 *
 *       Filename:  stack.h
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

#ifndef _STACK_H
#define _STACK_H
#include <pthread.h>


struct stack_node_st
{
	void *data;
	struct stack_node_st * link;
};

struct stack_head_node_st
{
	int count;
	int maxsize;
	pthread_mutex_t mutex;
	struct stack_node_st *base;
	struct stack_node_st *top;
	int  em_size;
};

typedef struct stack_head_node_st STACK_HEAD_NODE;
typedef struct stack_node_st STACK_NODE;

int Stack_init(STACK_HEAD_NODE **stack_head_ptr, int maxsize, int em_size);
int Stack_size(STACK_HEAD_NODE *stack_head_ptr);
int Stack_push(STACK_HEAD_NODE *stack_head_ptr, void *element);
int Stack_pop(STACK_HEAD_NODE *stack_head_ptr);
int Stack_gettop(STACK_HEAD_NODE *stack_head_ptr, void *element);
int Stack_clear(STACK_HEAD_NODE *stack_head_ptr);
int Stack_destroy(STACK_HEAD_NODE **stack_head_ptr);


#endif
