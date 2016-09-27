#include "pcm.h"
#include "appGlobal.h"
#include "FPGA.h"
#include "log.h"
#include "comm.h"
#include "audio.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sched.h>
#include <sys/eventfd.h>


int Pcm_create_task(void)
{
	int res = 0;  
	pthread_attr_t thread_attr;
	pthread_t pcm_thread;


	res = pthread_attr_init(&thread_attr);
	if(0 != res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: thread attribute creation failed\n", __func__, __LINE__);
		return ERROR;
	}

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(0 != res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: thread attribute setting failed\n", __func__, __LINE__);
		return ERROR;
	}


	res = pthread_create(&pcm_thread, &thread_attr, Pcm_capture_task, NULL);
	if(-1 == res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: Comm thread create failed!\r\n", __func__, __LINE__);
		return ERROR;
	}
	else
	{
		log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_AUDIO, "[%s]%d: Comm thread create successfully!\r\n", __func__, __LINE__);
	}


	(void)pthread_attr_destroy(&thread_attr);

	return SUCCESS;
}



void * Pcm_capture_task(void *arg)
{
	int i = 0;
	int port = 0;
	//struct pcm_frame stPcmFrame;
	
	//int count[4] = {0};
	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	Audio_init_eventfd();

	printf("======> Pcm Capture Task <======= \n");
	if(NULL == arg)
	{
		printf("======> thread args = NULL <===== \n");
	}

	while(1)
	{
		for( port = 0; port < MAX_AUDIO_NUM_4; ++port)
		{

			for( i = 0; i < MAX_AUDIO_NUM_4; ++i)
			{	
				if(RUNNING == pParams->AudioEncoderState[i].state)
				{
					pthread_mutex_lock(&pcm_mutex);
					Pcm_get_data(i);	
					pthread_mutex_unlock(&pcm_mutex);
				}
			}
			if(RUNNING == pParams->AudioEncoderState[port].state)
			{
				if((g_PcmHeadNode[port])->count >= 2 || STOP == pParams->AudioRegister[port].EncoderStatusReg.value)
				{
					pthread_mutex_lock(&pcm_mutex);

					Audio_rcv_pcm(port);

					sched_yield();
				}
			}

		}		

	}
}




int Pcm_get_data(int port)
{
	//int framelen = 0;
	AudioEncoderInfo * pParams = NULL; 
	pParams = Comm_get_shm_head_ptr();
	int frame_len = 0;

	if(ENCODER_M22_MPEG1_Layer2 == pParams->AudioRegister[port].AudioTypeReg.value)
	{
		frame_len = 1152;
	}
	else
	{
		frame_len = 1024;
	}
	frame_len = 1152;

	return	Pcm_save_frame(port, frame_len);

}


int Pcm_init(void)
{
	int i = 0;

	for(i = 0; i < 4; ++i)
	{
		Pcm_init_frame(i);
	}

	return SUCCESS;
}

int Pcm_init_frame(int port)
{

	return  pcm_queue_init(&g_PcmHeadNode[port], PCM_MAX_BUFFER_NUM);

}


int   Pcm_save_frame(int port, int framelen)
{

	struct pcm_frame  stPcmFrame;
	U16   buffer[PING_PANG_SIZE];
	static U16 last_buffer[100];
	int 	offset = 0;
	U32 	u32arrPTS[2] = {0};
	int 	value = 0;
	int     i = 0;
	//static  U64 last_pts[4] = {0};
	static U64  last_pts[4][3] = {{0,0,0}};
	static U64  interval[4][2] = {{0,0}};
	static  U64 u32flag[4][2] = {{0,0}};
	static char    read_ping[4][2] = {{0,0}};

#if AUDIO_DEBUG
	static U32 u32LastPts[4] = {0};
#endif
	//AudioEncoderInfo * pParams = NULL;
	//pParams = Comm_get_shm_head_ptr();


	U32 u32Data = 0;
	REG_Read(PCM_REQ_FLAG_REG_ADDR_0 + port , &u32Data);
	value = (u32Data & 0x01);
	if(0 == value)
	{
		//no pcm data
		return -1;
	}
	//printf("****************************** read data\n");


	u32flag[port][1] = u32Data;
	value = (u32Data & 0x02);
	if(0x0 == value)
	{
		offset = 0;
		read_ping[port][1] = 0;
		//printf("##PORT:%d -- PING DATA**************\n", audio_index);
		REG_Read(PCM_PTS_PING_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
		REG_Read(PCM_PTS_PING_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));
		//printf("pts[0] = %u pts[1] = %u\n", u32arrPTS[0], u32arrPTS[1]);
	}
	else
	{
		read_ping[port][1] = 1;
		//printf("##PORT:%d -- PANG DATA*************\n", audio_index);
		offset = 2048;
		REG_Read(PCM_PTS_PONG_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
		REG_Read(PCM_PTS_PONG_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));  
		//printf("pts[0] = %u pts[1] = %u\n", u32arrPTS[0], u32arrPTS[1]);
	}
	u32arrPTS[1] &= 0x1;
#if AUDIO_DEBUG
	u32LastPts[port] = u32arrPTS[0];
#endif
	U64   u64Pts = 0;
	memcpy(stPcmFrame.emPTS, u32arrPTS, 8);
	memcpy(&u64Pts, u32arrPTS, 8);

	//ack for read pcm
	REG_Write(RD_PCM_ACK_REG_ADDR_0 + port, 0);
	REG_Write(RD_PCM_ACK_REG_ADDR_0 + port ,0x01);
	REG_Write(RD_PCM_ACK_REG_ADDR_0 + port ,0);

	if(u32flag[port][0] == u32flag[port][1])
	{
		printf("\nu32flag[0] = %llu\n",u32flag[port][0]);
		printf("\nu32flag[1] = %llu\n",u32flag[port][1]);
		//sleep(120);

		if(u32flag[port][0])
		{
			u32flag[port][1] = 0;
			offset = 2048;
			REG_Read(PCM_PTS_PING_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
			REG_Read(PCM_PTS_PING_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));
		}
		else
		{
			u32flag[port][1] = 1;
			offset = 0;
			REG_Read(PCM_PTS_PONG_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
			REG_Read(PCM_PTS_PONG_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));
		}

		u32arrPTS[1] &= 0x1;
		memcpy(stPcmFrame.emPTS, u32arrPTS, 8);
		memcpy(&u64Pts, u32arrPTS, 8);
	}

	


	#if 0
	if((u64Pts > 30 * 90 + last_pts[port]) || (u64Pts < 10 * 90 + last_pts[port]))
	{
		if(read_ping[port][0] == read_ping[port][1])
		{
			//printf("\nu32flag[0] = %llu\n",u32flag[port][0]);
			//printf("\nu32flag[1] = %llu\n",u32flag[port][1]);
			//sleep(120);
			
			if(read_ping[port][0])
			{
				read_ping[port][1] = 0;
				offset = 2048;
				REG_Read(PCM_PTS_PING_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
				REG_Read(PCM_PTS_PING_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));
			}
			else
			{
				read_ping[port][1] = 1;
				offset = 0;
				REG_Read(PCM_PTS_PONG_LOW_0_REG_ADDR + 4 * port, &(u32arrPTS[0]));
				REG_Read(PCM_PTS_PONG_HIGH_0_REG_ADDR + 4 * port, &(u32arrPTS[1]));
			}

			u32arrPTS[1] &= 0x1;
			memcpy(stPcmFrame.emPTS, u32arrPTS, 8);
			memcpy(&u64Pts, u32arrPTS, 8);
		}
	}
	#endif

	#if 1
	U64   u64Pcr = 0;
	U32   u32arrPcr[2] = {0};
	static int interval_video = 0;
	
	REG_Read(VIDEO_TIMEBASE_LOW_REG_ADDR_0 + 2 * port, &u32arrPcr[0]);
	REG_Read(VIDEO_TIMEBASE_HIGH_REG_ADDR_0 + 2 *port, &u32arrPcr[1]);

	memcpy(&u64Pcr, u32arrPcr, 8);
	
	if( ((u64Pcr - u64Pts) > 90 + interval_video) || ((u64Pcr - u64Pts) + 90 < interval_video))
	{
		printf("===> u64Pcr - u64Pts = %llu , interval_video = %u <======\n",u64Pcr - u64Pts, interval_video);
		interval_video = u64Pcr - u64Pts;
	}
	#endif

#if AUDIO_DEBUG


	int error_count = 0;
	static int s_last_num[4] = {0,0,0,0};
	static U16 s_prev_num[4] = {0};
	static U16 s_back_num[4] = {0};
#endif

	for(i = 0; i < framelen; i++)
	{
		REG_Write(PCM_READ_ADDR_REG_0 + port , offset + i);
		REG_Read(PCM_READ_DATA_REG_ADDR_0 + port, &u32Data);

		//buffer[i*2] = u32Data % 65536;
		//buffer[i*2 + 1] = u32Data / 65536;

		buffer[i*2] = u32Data & 0xffff;
		buffer[i*2 + 1] = u32Data >> 16;

#if AUDIO_DEBUG
		s_prev_num[port] = u32Data % 65536;
		s_back_num[port] = u32Data / 65536;
		if((s_prev_num[port] + 1) % 100 == s_back_num[port])
		{
			if((s_last_num[port] + 1) % 100 == s_prev_num[port])
			{
				;
				//continue;
			}
			else
			{
				error_count++;
				if(error_count < 5)
				{
					printf("1-prev: %u -- back: %u\n", s_prev_num[port], s_back_num[port]);
					printf("**********************port:%d --byte:%d -- error:should:%u -- result:%u\n",port,i,(s_last_num[port]+1) % 100, s_prev_num[port]);
				}
			}
		}
		else
		{
			error_count++;
			if(error_count < 5)
			{
				printf("2-prev: %u -- back: %u\n", s_prev_num[port], s_back_num[port]);
				printf("**********************port:%d --byte:%d -- error:should:%u -- result:%u\n",port,i,(s_prev_num[port]+1) % 100, s_back_num[port]);
			}
		}

		s_last_num[port] = s_back_num[port];
#endif

	}

#if 0
	if(error_count > 0)
	{
		printf("#==========================================================#\n");
	}
#endif

	last_pts[port][0] = last_pts[port][1];
	last_pts[port][1] = last_pts[port][2];
	last_pts[port][2] = u64Pts;

	interval[port][0] = last_pts[port][1] - last_pts[port][0];
	interval[port][1] = last_pts[port][2] - last_pts[port][1];

	if((interval[port][1] > interval[port][0] + 90) || (interval[port][0] > interval[port][1] + 90))
	{
		printf("=========> interval[0] = %llu   interval[1] = %llu  <============\n", interval[port][0], interval[port][1]);
		printf("=========> last_pts[0] = %llu   last_pts[1] = %llu  last_pts[2] = %llu <==========\n\n", last_pts[port][0], last_pts[port][1],last_pts[port][2]);
	}

	if(interval[port][1] > 25 * 90)
	{
		printf(" =======> last_pts[1] = %llu  last_pts[2] = %llu <=======\n",last_pts[port][1],last_pts[port][2]);
	}
	

	u32flag[port][0] = u32flag[port][1];
	read_ping[port][0] = read_ping[port][1];

#if 0
	if((u64Pts > 30 * 90 + last_pts[port]) || (u64Pts < 10 * 90 + last_pts[port]))
	{
		printf("\nxxxxxxxx===> Logic Pts: last_pts = %llu   cur_pts = %llu  <==xxxxxx\n", last_pts[port], u64Pts);

		printf("\n=============== last_buffer ==================\n");
		for(i = 0; i < 50; i++)
		{
			printf("0x%x ", last_buffer[i]);
			if(9 == (i % 10 ))
			{
				printf("\n");
			}
		}
		printf("\n ============= cur buffer ============\n");
		for( i = 0; i < 50; i++ )
		{
			printf("0x%x ", buffer[i]);
			if(9 == (i % 10))
			{
				printf("\n");
			}
		}
		if((u32flag[port][0] & 0x01))
		{
			printf("\nu32flag[0] = %llu\n",u32flag[port][0]);
			if((u32flag[port][0] & 0x02))
			{
				printf("======> Last Read PONG\n");
			}
			else
			{
				printf("=====> Last Read PING\n");
			}
		}
		if((u32flag[port][1] &0x01))
		{
			printf("\nu32flag[1] = %llu\n",u32flag[port][1]);
			if((u32flag[port][1] & 0x02))
			{
				printf("======> Current Read PONG\n");
			}
			else
			{
				printf("=====> Current Read PING\n");
			}

		}

	}
	last_pts[port] = u64Pts;
	memcpy(last_buffer, buffer, 100);
	u32flag[port][0] = u32flag[port][1];
	read_ping[port][0] = read_ping[port][1];
#endif
	

	if( pcm_queue_size(g_PcmHeadNode[port]) == (g_PcmHeadNode[port])->max_size)
	{
		pcm_queue_pop(g_PcmHeadNode[port]);	

	}

	stPcmFrame.emDataLen = framelen * 4;
	stPcmFrame.emData = (unsigned char *)buffer;
	stPcmFrame.link = NULL;

	//printf("====> Get Pcm =====>\n");
	pcm_queue_push(g_PcmHeadNode[port], &stPcmFrame);


	return 0;
}



int	Pcm_get_one_frame(int port, PCM_FRAME_T  pstPCMFrame, int pcmlen)
{
	//printf("##star func -- %s\n", __func__);

	if((port >= MAX_AUDIO_NUM_4) || (NULL == pstPCMFrame))
	{
		return ERROR;
	}

	if(pcm_queue_size(g_PcmHeadNode[port]) < 1)
	{
		return ERROR;
	}

	pcm_queue_getfront(g_PcmHeadNode[port], pstPCMFrame);

	if((unsigned int)pcmlen != pstPCMFrame->emDataLen)
	{
		pcm_queue_pop(g_PcmHeadNode[port]);
		return ERROR;
	}


	pcm_queue_pop(g_PcmHeadNode[port]);

	return SUCCESS;

}


int Pcm_configure(int port)
{
	//printf("##port:%d -- start func -- %s\n",port,__func__);
	U32  u32FrameLen = 0x0;
	AudioEncoderInfo * pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	//0 - 16bit , 1 - 32 bit  
	REG_Write(I2S_BIT_MODE, 0);

	if(ENCODER_M22_MPEG1_Layer2 == pParams->AudioRegister[port].AudioTypeReg.value)
	{
		u32FrameLen = 1152;
	}
	else
	{
		u32FrameLen = 1024;
	}

	u32FrameLen = 1152;
	REG_Write(PING_PANG_LEN_REG_ADDR_0 +port, u32FrameLen -1);

	//PCM  enable
	REG_Write(PCM_ENABLE_REG, 0x0F);

	//clear pcm
	pcm_queue_clear(g_PcmHeadNode[port]);

	return SUCCESS;
}





