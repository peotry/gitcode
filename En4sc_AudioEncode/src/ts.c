#include "ts.h"
#include "appGlobal.h"
#include "log.h"

#include <stdio.h>

/*
   函数名: Ts_init

   作用； 初始化ts队列

   输入: 

   输出: 

   返回值: 	ERROR - 出错，SUCCESS - 成功

   作者:  linsheng.pan

*/
int  Ts_init(void)
{
	int i = 0;

	for(i=0; i < MAX_AUDIO_NUM_4; ++i)
	{
		Ts_init_frame(i);
	}
	return 0;
}



int Ts_init_frame(int port)
{
	return  ts_queue_init(&g_TsHeadNode[port], TS_MAX_BUFFER_NUM);
}


/*
   函数名: Ts_save_frame

   作用； 存储ts包到ts队列

   输入: port - 编码器路数，pstTsFrame - ts帧

   输出: 

   返回值: 	ERROR - 出错，SUCCESS - 成功

   作者:  linsheng.pan

*/
int Ts_save_frame(int port, TS_FRAME_T  pstTsFrame)
{

	//查看是否满，如果满，删除掉最前一个 
	if( ts_queue_size(g_TsHeadNode[port]) == (g_TsHeadNode[port])->max_size )
	{
		//删除最前一个
		ts_queue_pop(g_TsHeadNode[port]);
		printf("####### ===== TS Frame is full ==== ###############\n");
	}

	return  ts_queue_push(g_TsHeadNode[port], pstTsFrame);

}



int Ts_get_one_frame(int port, TS_FRAME_T  pstTsFrame)
{
	return ts_queue_getfront(g_TsHeadNode[port], pstTsFrame);
}

int	Ts_del_one_frame(int port )
{
	return ts_queue_pop(g_TsHeadNode[port]);

}




