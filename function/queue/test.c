/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/25/2016 02:48:09 AM
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

#include "queue.h"

int main(int argc, char **argv)
{
	struct node
	{
		int num1;
		int num2;
	};

	struct node st_node = {0,0};
	int i = 0;

	QUEUE_HEAD_NODE * queue_head_ptr = NULL;

	Queue_init(&queue_head_ptr, 5, sizeof(struct node));

	for(i = 0; i< 6; i++)
	{
		st_node.num1 = i;
		Queue_push(queue_head_ptr, (void *)(&st_node));
	}

	Queue_clear(queue_head_ptr);
	struct node st_tmp;
	for(i = 0; i < 5; i++)
	{
		if(0 == Queue_getfront(queue_head_ptr, &st_tmp))
		{

			printf("num1 = %d\n", st_tmp.num1);
		}
		Queue_pop(queue_head_ptr);
	}

	return 0;
}
