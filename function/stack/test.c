#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

int main(int argc, char **argv)
{
	struct node{
		int num1;
		int num2;
	};
	struct node st_node = {0};
	
	STACK_HEAD_NODE * stack_head_ptr = NULL;
	int i = 0;
	
	Stack_init(&stack_head_ptr, 5, sizeof(struct node));
	
	for(i = 0; i < 5; i++)
	{
		st_node.num1 = i;
		Stack_push(stack_head_ptr, &st_node);
	}

	Stack_clear(stack_head_ptr);
	
	struct node tmp;
	for(i = 0; i < 6; i++)
	{
		if(0 == Stack_gettop(stack_head_ptr,&tmp))
		{
			printf("num1 = %d\n", tmp.num1);
		}
		Stack_pop(stack_head_ptr);
	}
	
	return 0;
}
