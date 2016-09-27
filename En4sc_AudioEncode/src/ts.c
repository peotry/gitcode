#include "ts.h"
#include "appGlobal.h"
#include "log.h"

#include <stdio.h>

/*
   ������: Ts_init

   ���ã� ��ʼ��ts����

   ����: 

   ���: 

   ����ֵ: 	ERROR - ����SUCCESS - �ɹ�

   ����:  linsheng.pan

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
   ������: Ts_save_frame

   ���ã� �洢ts����ts����

   ����: port - ������·����pstTsFrame - ts֡

   ���: 

   ����ֵ: 	ERROR - ����SUCCESS - �ɹ�

   ����:  linsheng.pan

*/
int Ts_save_frame(int port, TS_FRAME_T  pstTsFrame)
{

	//�鿴�Ƿ������������ɾ������ǰһ�� 
	if( ts_queue_size(g_TsHeadNode[port]) == (g_TsHeadNode[port])->max_size )
	{
		//ɾ����ǰһ��
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




