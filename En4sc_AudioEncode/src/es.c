#include "es.h"
#include "appGlobal.h"

#include <stdio.h>

/*
   函数名: Es_init

   作用； 初始化es队列

   输入:  

   输出:

   返回值: 	ERROR - 出错，SUCCESS - 成功

   作者:  linsheng.pan

*/
int  Es_init(void)
{
	int i = 0;

	for(i=0; i < MAX_AUDIO_NUM_4; ++i)
	{
		Es_init_frame(i);
	}
	return 0;
}


int Es_init_frame(int port)
{
	return  es_queue_init(&g_EsHeadNode[port], ES_MAX_BUFFER_NUM);
}


/*
   函数名: Es_get_one_frame

   作用； 获取一个es帧

   输入:  port - 编码器路数，eslen - es帧长度

   输出: pstESFrame - es帧

   返回值: 	ERROR - 出错，SUCCESS - 成功

   作者:  linsheng.pan

*/
int	Es_get_one_frame(int port, ES_FRAME_T  pstESFrame, int eslen)
{
	printf("##star func -- %s\n", __func__);

	if((port >= MAX_AUDIO_NUM_4) || (NULL == pstESFrame))
	{
		return ERROR;
	}

	//没有数据
	if(es_queue_size(g_EsHeadNode[port]) < 1)
	{
		return ERROR;
	}

	es_queue_getfront(g_EsHeadNode[port], pstESFrame);

	if((unsigned int)eslen != pstESFrame->emDataLen)
	{
		es_queue_pop(g_EsHeadNode[port]);
		return ERROR;
	}

	es_queue_pop(g_EsHeadNode[port]);

	return SUCCESS;
}

/*
   函数名: Es_save_frame

   作用； 存储一个es帧

   输入:  port - 编码器路数，pstESFrame - es帧

   输出: 

   返回值: 	ERROR - 出错，SUCCESS - 成功

   作者:  linsheng.pan

*/
int Es_save_frame(int port, ES_FRAME_T  pstESFrame)
{
	//查看是否满，如果满，删除掉最前一个 
	if( es_queue_size(g_EsHeadNode[port]) == (g_EsHeadNode[port])->max_size )
	{
		//删除最前一个
		es_queue_pop(g_EsHeadNode[port]);
		printf("######### ====== ES Frame is full ===============##############\n");
	}

	return es_queue_push(g_EsHeadNode[port], pstESFrame);
}
