#include "audiopacket.h"
#include "appGlobal.h"
#include "audio.h"
#include "FPGA.h"
#include "log.h"
#include "es_queue.h"
#include "comm.h"
#include "ts.h"

extern 

/*
   Name:			packet_get_pes

   Description:		packet_get_pes() packet ES to PES, and get the PTS

   Return Value:      

   Author:                LGmark

*/
int packet_get_pes(unsigned char * pesbuffer, int port, unsigned int *u32arrPts)
{
	if(port >= MAX_AUDIO_NUM_4)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d Audio [%d] error parameters!\n", __func__, __LINE__, port);
		return ERROR;
	}

	//No Es data
	if(g_EsHeadNode[port]->count < FRAME_NUM_PER_PES)
	{
		return ERROR;
	}

	int 	i = 0;
	U64 u64PTS = 0;
	int 	pesoffset = 14;
	struct  es_frame 	stEsFrame;
	unsigned char	es_buffer[9216]= {0};
	unsigned char local_ptsbuffer[8] = {0};
	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();
	stEsFrame.emData = (unsigned char *)es_buffer;

	for(i = 0; i < FRAME_NUM_PER_PES; ++i)
	{
		es_queue_getfront(g_EsHeadNode[port], &stEsFrame);
		es_queue_pop(g_EsHeadNode[port]);

		memcpy(pesbuffer + pesoffset, stEsFrame.emData, stEsFrame.emDataLen);
		pesoffset += stEsFrame.emDataLen;
		if(0 == i)
		{
			memcpy(&u64PTS, stEsFrame.emPTS, 8);
		}

	}


	//delay
	//static int count[4] = {0};
	int delay = 0;
	int modify_pts = 860 * 90;
	static int last_delay = 0;
	int cur_delay = 0;
	static U64  last_pts[4][3] = {{0,0,0}};
	static U64 interval[4][2] = {{0,0}};

	//delay = (pParams->AudioRegister[port].EncodeDelayReg.value - 2000) * 90;
	delay = pParams->AudioRegister[port].EncodeDelayReg.value - 2000;

	cur_delay = 860 + delay + g_AudioDelay[port];
	if(last_delay != cur_delay)
	{
		printf("==========> new delay = %d <============\n", cur_delay);
		last_delay = cur_delay;
	}

	//memcpy(&u64PTS, stEsFrame.emPTS, 8);

	//u64PTS = u64PTS	+ delay + modify_pts + g_AudioDelay[port] * 90;
	u64PTS = u64PTS	+ cur_delay * 90;

	memcpy(local_ptsbuffer, &u64PTS, 8);

	memcpy(u32arrPts, &u64PTS, 8);

	last_pts[port][0] = last_pts[port][1];
	last_pts[port][1] = last_pts[port][2];
	last_pts[port][2] = u64PTS;

	interval[port][0] = last_pts[port][1] - last_pts[port][0];
	interval[port][1] = last_pts[port][2] - last_pts[port][1];

	#if 0
	if((interval[port][1] > interval[port][0] + 90 * 2) || (interval[port][0] > interval[port][1] + 90 * 2))
	{
		printf("=========> interval[0] = %llu   interval[1] = %llu  <============\n", interval[port][0], interval[port][1]);
		printf("=========> last_pts[0] = %llu   last_pts[1] = %llu  last_pts[2] = %llu <==========\n", last_pts[port][0], last_pts[port][1],last_pts[port][2]);
	}
	
	#endif
	


#if 0
	U64 * pts = 0;
	int delay = 0;
	unsigned int * pu32ts = 0;
	static unsigned int lastlowpts[4] = {0};
	int   maxptsoffset = 0;
	int   curoffset = 0;
	static int   ptserror[4] = {0};
	static int   ptsmodifycnt[4] = {0};
	int   modifypts = 0;
	int   ndelay = 0;

	/* ��ӡPTS */
	if(ENCODER_M22_MPEG1_Layer2 == pParams->AudioRegister[port].AudioTypeReg.value)
	{
		maxptsoffset = FRAME_NUM_PER_PES * 2160;    
	}
	//else if(AUDIO_TYPE_DRA == pParams->AudioRegister[port].AudioTypeReg.value)
	//{
	//    maxptsoffset = FRAME_NUM_PER_PES * 1920;  
	//}
	pu32ts = (unsigned int *)local_ptsbuffer;
	curoffset = *(pu32ts) - lastlowpts[port];
	if(abs(curoffset - maxptsoffset) >= 2000)
	{
		ptserror[port] += 1;;
		if(0x1 == port)
		{
			//printf("[%s]%d: Port-%d pts jump [%d]=> [%d] total count [%d]\n!\n", __func__, __LINE__, audio_index, lastlowpts[audio_index], *(pu32ts), ptserror[audio_index]);
		}
	}
	else if(abs(curoffset - maxptsoffset) >= PTS_MAX_JIFFTER)
	{
		modifypts = maxptsoffset - curoffset;
		ptsmodifycnt[port] += 1;;
		if(0x1 == port)
		{
			//printf("[%s]%d: Port-%d pts jump [%d]=> [%d] total modifycount [%d] modify pts [%d]\n!\n", __func__, __LINE__, audio_index, lastlowpts[audio_index], *(pu32ts), ptsmodifycnt[audio_index], modifypts);
		}     
	}


	lastlowpts[port] = *(pu32ts);

	/* ��Ƶ�ӳ� */
	delay = pParams->AudioRegister[port].EncodeDelayReg.value * 90 + modifypts;
	pts =  (U64 *)(&(local_ptsbuffer[0]));
	if((delay < 0) && (*pts < abs(delay)))
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: Negtive pts modify, [%d] vs [%d]!\n", __func__, __LINE__, (int)(*pts), abs(delay));
		ndelay = (int)(*pts) + delay;
		*pts += ( 0x1FFFFFFFF + ndelay );
	}
	else
	{
		*pts += delay;
	}

	//��ȡpts��ֵ
	memcpy(pu64Pts, local_ptsbuffer, 8);
#endif


	pesbuffer[0] = 0x00;
	pesbuffer[1] = 0x00;
	pesbuffer[2] = 0x01;


	if(ENCODER_M22_MPEG1_Layer2 == pParams->AudioRegister[port].AudioTypeReg.value)
	{
		pesbuffer[3] = 0xC0;
	}
	else
	{
		pesbuffer[3] = 0xBD;
	}

	//PES length
	pesbuffer[4] = ((pesoffset - 6) >> 8) & 0xff;
	pesbuffer[5] = (pesoffset - 6) & 0xff;

	//flags
	pesbuffer[6] = 0x81;// 1000 0001
	pesbuffer[7] = 0x80;// 1000 0000

	//PES header data length
	pesbuffer[8] = 0x05;

	//buffer[9] = 0010 + PTS[32..30] + marker_bit(1 bit)
	//PTS[32..30]
	pesbuffer[9] =  (0x20) | ((local_ptsbuffer[4] & 0x1) << 3) | ((local_ptsbuffer[3] & 0xC0) >> 5) | 0x1;
	//buffer[9] =  (0x20 << 4) | ((local_ptsbuffer[4] & 0x1) << 3) | ((local_ptsbuffer[3] & 0xC0) >> 5) | 0x1;

	//buffer[10..11] = PTS[29..15] + marker_bit(1 bit) 
	//PTS[29..15]
	pesbuffer[10] = ((local_ptsbuffer[3] & 0x3F) << 2) | ((local_ptsbuffer[2] & 0xC0) >> 6);
	pesbuffer[11] = ((local_ptsbuffer[2] & 0x3F) << 2) | ((local_ptsbuffer[1] & 0x80) >> 6) |  0x1;

	//buffer[12..13] = PTS[14..0] + marker_bit(1 bit) 
	//PTS[14..0]
	pesbuffer[12] = ((local_ptsbuffer[1] & 0x7F) << 1) | ((local_ptsbuffer[0] & 0x80) >> 7);
	pesbuffer[13] = ((local_ptsbuffer[0] & 0x7F) << 1) | 0x1;

	//pesbuffer[14] = 0xFF;
	// pesbuffer[15] = 0xFC;   

	return pesoffset;
}



/*
Name:			packet_get_ts

Description:		packet_get_ts() packet PES to TS

Return Value:      

Author:                LGmark

*/
int packet_get_ts(int port)
{
	unsigned char  ts_buffer[188];
	U32   audio_pid = 0;
	U8    ts_pid[4];
	unsigned char  pes_buffer[PING_PANG_SIZE];

	int   pes_len = 0;
//	static int count[4] = {0};
	struct ts_frame stTsFrame;
	unsigned int arrPts[2] = {0};
	static U8 count_ts[4] = {0};

	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();
	audio_pid = pParams->AudioRegister[port].AudioTsPIDReg.value;

	memcpy(ts_pid, &audio_pid, sizeof(U32));

	//printf("====> start packet_get_ts func ===> \n");
	pes_len = packet_get_pes(pes_buffer, port, arrPts);
	if(pes_len < 0)
	{
		return ERROR;
	}

	memcpy(stTsFrame.emPTS, arrPts, 8);

	U8    i = 0;
	unsigned int   pes_offset = 0;
	unsigned int   len = 0;
	while(pes_offset < pes_len)
	{
		// 1 byte
		ts_buffer[0] = 0x47;
		// 2-3 byte
		//transport_error_indicator -- 1 bit
		//payload_unit_start_indicator -- 1 bit
		//transport_priority -- 1 bit
		//PID --- 13 bit
		ts_buffer[1] = (0x1F & ts_pid[1]);
		if(0 == i)
		{
			//the first ts packet set  payload_unit_start_indicator bit to 1
			ts_buffer[1] |= 0x40;
		}

		//audio pid
		ts_buffer[2] = ts_pid[0];

		// 4 byte
		//transport_scrambling_control -- 2 bit
		//adaptation_field_control -- 2 bit
		//continuity_counter -- 4 bit
		ts_buffer[3] = (0x00 | count_ts[port]); //


		//ts data
		if(pes_offset + 184 <= pes_len)
		{
			ts_buffer[3] |= 0x10;
			memcpy(&ts_buffer[4], pes_buffer + pes_offset, 184);
			pes_offset += 184;

		}
		else
		{

			ts_buffer[3] |= 0x30;
			len = 184 - (pes_len % 184) -1;// (ts flag byte +0xFF)

			ts_buffer[4] = len;  //fill byte
			ts_buffer[5] = 0x00;//ts flag

			memset(&ts_buffer[6], 0xFF, len -1);
			memcpy(&ts_buffer[5 + len], pes_buffer + pes_offset, 183 - len);
			pes_offset += (183 - len);
		}

		memcpy(stTsFrame.emData, ts_buffer, 188);


		Ts_save_frame(port, &stTsFrame);

		i++;
		count_ts[port]++;
		if(count_ts[port] > 0x0f)
		{
			count_ts[port] = 0;
		}

	}

	//printf("===> end packet_get_ts func ===> \n");

	return SUCCESS;
}



