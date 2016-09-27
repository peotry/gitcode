#include "es.h"
#include "appGlobal.h"

#include <stdio.h>

/*
   ������: Es_init

   ���ã� ��ʼ��es����

   ����:  

   ���:

   ����ֵ: 	ERROR - ����SUCCESS - �ɹ�

   ����:  linsheng.pan

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
   ������: Es_get_one_frame

   ���ã� ��ȡһ��es֡

   ����:  port - ������·����eslen - es֡����

   ���: pstESFrame - es֡

   ����ֵ: 	ERROR - ����SUCCESS - �ɹ�

   ����:  linsheng.pan

*/
int	Es_get_one_frame(int port, ES_FRAME_T  pstESFrame, int eslen)
{
	printf("##star func -- %s\n", __func__);

	if((port >= MAX_AUDIO_NUM_4) || (NULL == pstESFrame))
	{
		return ERROR;
	}

	//û������
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
   ������: Es_save_frame

   ���ã� �洢һ��es֡

   ����:  port - ������·����pstESFrame - es֡

   ���: 

   ����ֵ: 	ERROR - ����SUCCESS - �ɹ�

   ����:  linsheng.pan

*/
int Es_save_frame(int port, ES_FRAME_T  pstESFrame)
{
	//�鿴�Ƿ������������ɾ������ǰһ�� 
	if( es_queue_size(g_EsHeadNode[port]) == (g_EsHeadNode[port])->max_size )
	{
		//ɾ����ǰһ��
		es_queue_pop(g_EsHeadNode[port]);
		printf("######### ====== ES Frame is full ===============##############\n");
	}

	return es_queue_push(g_EsHeadNode[port], pstESFrame);
}
