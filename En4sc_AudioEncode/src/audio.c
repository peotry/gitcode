#include "audio.h"
#include "ProcessMutex.h"
#include "sem.h"
#include "appGlobal.h"
#include "comm.h"
#include "FPGA.h"
#include "pcm.h"
#include "log.h"
#include "pcm_queue.h"
#include "es_queue.h"
#include "es.h"
#include "ts_queue.h"
#include "ts.h"
#include "audiopacket.h"

#include "twolame.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/frame.h"


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <twolame.h>
#include <math.h>
#include <sys/time.h>
#include <sched.h>
#include <sys/eventfd.h>
#include <stdint.h>

pthread_mutex_t pcm_mutex = PTHREAD_MUTEX_INITIALIZER;


int g_AudioFd[4] = {-1,-1,-1,-1};
int g_AudioDelay[4] = {0};

int Audio_clear_buffer(int port)
{
	//printf("##Clear Buffer##############################\n");
	pcm_queue_clear(g_PcmHeadNode[port]);
	es_queue_clear(g_EsHeadNode[port]);
	ts_queue_clear(g_TsHeadNode[port]);

	return SUCCESS;
}

int Audio_init_eventfd(void)
{
	int i = 0;
	for(i = 0; i < MAX_AUDIO_NUM_4; ++i)
	{
		g_AudioFd[i] = eventfd(0, 0);
	}

	return SUCCESS;
}

int Audio_rcv_pcm(int port)
{
	int ret = eventfd_write(g_AudioFd[port], 1);
	
	return ret;
}


int Audio_wait_pcm(int port)
{
	eventfd_t val;
	int ret = eventfd_read(g_AudioFd[port] , &val);

	return ret;
}


int Audio_init_ffmpeg(void)
{
	//ffmpeg initialize
	av_register_all();
	
	return SUCCESS;
}


int create_audio_encode_task(int *data)
{
	int res = 0;  
	pthread_attr_t thread_attr;
	pthread_t comm_thread;


	res = pthread_attr_init(&thread_attr);
	if(0 != res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_ENCODE, "[%s]%d: thread attribute creation failed\n", __func__, __LINE__);
		return ERROR;
	}

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(0 != res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_ENCODE, "[%s]%d: thread attribute setting failed\n", __func__, __LINE__);
		return ERROR;
	}



	res = pthread_create(&comm_thread, &thread_attr, Audio_encoder_task,(void *)data);
	if(-1 == res)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_ENCODE, "[%s]%d: Comm thread create failed!\r\n", __func__, __LINE__);
		return ERROR;
	}
	else
	{
		printf("create thread--%d\n", *data);
		log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_ENCODE, "[%s]%d: Comm thread create successfully!\r\n", __func__, __LINE__);
	}



	(void)pthread_attr_destroy(&thread_attr);

	return SUCCESS;

}


int Audio_twolame_configure(twolame_options **encopts, unsigned char **pcm_buffer, unsigned char **mp2buffer,int port)
{
	AudioEncoderInfo * pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	if ( NULL ==  (*pcm_buffer = (unsigned char *) malloc(AUDIO_BUF_SIZE)) )
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"pcmaudio memory allocation failed\n");
		fprintf(stderr, "[%s]%d: Audio [%d] Error: pcmaudio memory allocation failed\n", __func__, __LINE__, port);
		exit(-1);
	}

	if ( NULL == (*mp2buffer = (unsigned char *)malloc(MP2_BUF_SIZE)) )
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"mp2buffer memory allocation failed\n");
		fprintf(stderr, "[%s]%d: Audio [%d] Error: mp2buffer memory allocation failed\n", __func__, __LINE__, port);
		exit(-1);
	}


	*encopts = twolame_init();
	if (*encopts == NULL) 
	{	
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"initializing libtwolame encoder failed\n");
		fprintf(stderr, "[%s]%d: Audio [%d] Error: initializing libtwolame encoder failed.\n", __func__, __LINE__, port);
		exit(-1);
	}

	int s32SampleRate = 48000;
	int s32BitRate = 128;

	Audio_get_sample_rate(port, &s32SampleRate);
	Audio_get_bit_rate(port, &s32BitRate);


	twolame_set_version(*encopts, TWOLAME_MPEG1);
	twolame_set_in_samplerate(*encopts, s32SampleRate);
	twolame_set_out_samplerate(*encopts, s32SampleRate);
	//test
	//twolame_set_in_samplerate(*encopts, 48000);
	//twolame_set_out_samplerate(*encopts, 48000);

	twolame_set_scale(*encopts, 1.0);
	twolame_set_psymodel(*encopts,0); 
	//twolame_set_psymodel(*encopts, 4); 
	twolame_set_bitrate(*encopts, s32BitRate);
	//test
	//twolame_set_bitrate(*encopts, 128);
	//twolame_set_bitrate(*encopts, 320);
	//twolame_set_bitrate(*encopts, 192);
	if(SingleChannel == pParams->AudioRegister[port].ChannelLayoutReg.value)
	{
		twolame_set_mode(*encopts, TWOLAME_MONO);
		twolame_set_num_channels(*encopts,1);
	}
	else
	{
		twolame_set_mode(*encopts, TWOLAME_STEREO);
		twolame_set_num_channels(*encopts,2);
	}
	//twolame_set_quick_mode(encopts, 1);
	//twolame_set_quick_count(encopts, 10);


	if (twolame_init_params(*encopts) != 0) 
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"configuring libtwolame encoder failed\n");
		fprintf(stderr, "[%s]%d: Audio [%d] Error: configuring libtwolame encoder failed.\n", __func__, __LINE__, port);
		exit(-1);
	}


	//twolame_print_config(encopts);

	return SUCCESS;
}


int Audio_single_channel_config(unsigned char  **sw_pcm_in_buffer, int in_size, unsigned char **sw_pcm_out_buffer,
		int out_size, struct SwrContext **au_convert_ctx, int port)
{

	int i = 0;
	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();
	//U32 emSampleFrequency = pParams->AudioRegister[port].SampleFrequencyReg.value;
	int s32SampleRate = 48000;
	Audio_get_sample_rate(port,&s32SampleRate);

	int samples_read = 1152 * 1;
	int readsize = samples_read * 2 * 2; 
	if(SingleChannel == pParams->AudioRegister[port].ChannelLayoutReg.value)
	{        
		//BUFFER
		for(i = 0; i < 32; ++i)
		{
			sw_pcm_in_buffer[i] = av_malloc(readsize);
			if(NULL == sw_pcm_in_buffer[i])
			{
				printf(" =====> Pcm buffer malloc failed! <==== \n");
				log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: Audio-%d Pcm buffer malloc failed!\n", __func__, __LINE__, port);
				return -1;
			}
			sw_pcm_out_buffer[i] = av_malloc(readsize);
			if(NULL == sw_pcm_out_buffer[i])
			{
				printf(" =====> Pcm buffer malloc failed! <==== \n");
				log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: Audio-%d Pcm buffer malloc failed!\n", __func__, __LINE__, port);
				return -1;
			}
		}
		*au_convert_ctx = swr_alloc();
		*au_convert_ctx = swr_alloc_set_opts(*au_convert_ctx,
				AV_CH_LAYOUT_MONO, 
				AV_SAMPLE_FMT_S16, 
				s32SampleRate,
				AV_CH_LAYOUT_STEREO,
				AV_SAMPLE_FMT_S16 , 
				s32SampleRate,
				0, 
				NULL);
		if(NULL == *au_convert_ctx)
		{
			log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d: Audio-%d Seting PCM-Convert failed!\n", __func__, __LINE__, port);
			return -1;
		}
		else
		{
			printf("======> Seting PCM-Convert successfully! <=====\n");
			log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_AUDIO, "[%s]%d: Audio-%d Seting PCM-Convert successfully!\n", __func__, __LINE__, port);
		}                        
		swr_init(*au_convert_ctx);
	}
	return SUCCESS;
}



#if FMPEG
int audio_FlushEncoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
	int ret;
	int got_frame;
	AVPacket enc_pkt;
	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
				CODEC_CAP_DELAY))
		return 0;
	while (1) {
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_audio2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
				NULL, &got_frame);
		av_frame_free(NULL);
		if (ret < 0)
			break;
		if (!got_frame){
			ret=0;
			break;
		}
		log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_AUDIO, "Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",enc_pkt.size);

		ret = av_write_frame(fmt_ctx, &enc_pkt);
		if (ret < 0)
			break;
	}
	return ret;
}
#endif


void * Audio_encoder_task(void *arg)
{
	int readsize = 0;

	int ret=0;
	int pcm_len = 0;

	int  i = 1;


#if FMPEG
	unsigned char * sw_pcm_in_buffer[32]= {NULL};
	unsigned char * sw_pcm_out_buffer[32]= {NULL};
	struct SwrContext *au_convert_ctx = NULL;
	unsigned int sw_len = 0;
#endif

	struct timeval start_time, end_time;

	int	port = 0;
	U32 emChannelLayout = Stereo; 
	//U32 emSampleFrequency = 0;
	AudioEncoderInfo *pParams = NULL;

	pParams = Comm_get_shm_head_ptr();

	port = *((int *)arg);

	Audio_wr_pcr_pid(port);


	pParams->AudioEncoderState[port].isStop = false;
	pParams->AudioEncoderState[port].isRunning = true;
	pParams->AudioEncoderState[port].state  = RUNNING;

	Comm_modify_encoder_status(port,NONE);

	printf("#####================thread:%d -- start encode=============###########\n", port);

	g_AudioDelay[port] = Audio_get_delay(port);

	printf("=================> Audio Delay = %d  <=========================\n", g_AudioDelay[port]);

	//if(0x1 == audio_index)
	//	av_log_set_level(AV_LOG_QUIET);


	//control thread exit
	while(false == pParams->AudioEncoderState[port].isStop  && 
			true == pParams->AudioEncoderState[port].isRunning)

	{
		//pcm configure
		Pcm_configure(port);

		emChannelLayout = pParams->AudioRegister[port].ChannelLayoutReg.value;

		if(ENCODER_M22_MPEG1_Layer2 == pParams->AudioRegister[port].AudioTypeReg.value)
		{
			twolame_options *encopts = NULL;
			unsigned char *pcm_buffer = NULL;
			unsigned char *mp2buffer = NULL;


			unsigned char 	*pcm_input =  NULL;
			int 	samples_read = 0;
			int 	mp2fill_size = 0;
			//int 	framelen = 1152;

			samples_read = 1152 * 1;
			readsize = samples_read * 2 * 2;   

			//twolame configure
			Audio_twolame_configure(&encopts, &pcm_buffer, &mp2buffer, port);


			struct pcm_frame stPcmFrame1;
			//stPcmFrame1.emData = pcm_buffer;

			//struct pcm_frame stPcmFrame2;
			//stPcmFrame2.emData = pcm_buffer + 1152 * 2 *2;

			//es 
			struct es_frame stEsFrame;
			stEsFrame.emData = mp2buffer;




			// single channle config
#if FMPEG
			Audio_single_channel_config(sw_pcm_in_buffer, 32, sw_pcm_out_buffer, 32, &au_convert_ctx,port);
#endif

			while (false == pParams->AudioEncoderState[port].isStop && 
					true == pParams->AudioEncoderState[port].isRunning) 
			{

				Audio_wait_pcm(port);
				//printf("=====> Recieve Pcm Data  ====>\n");
				if(STOP == pParams->AudioRegister[port].EncoderStatusReg.value)
				{
					pParams->AudioEncoderState[port].isStop = true;
					pParams->AudioEncoderState[port].isRunning = false;
					pthread_mutex_unlock(&pcm_mutex); 
					continue;
				}


				if((g_PcmHeadNode[port])->count > 40)
				{
					printf("###port:%d recieve msg, queue num = %d ===#########\n", port, (g_PcmHeadNode[port])->count);
				}


				//pcm_len = 1152;

				//int	   pes_len = 0;

				if(SingleChannel == emChannelLayout)
				{
					pcm_input = (unsigned char *)sw_pcm_in_buffer[0];
				}
				else
				{
					pcm_input = (unsigned char *)pcm_buffer;
				}

				stPcmFrame1.emData = pcm_input;

				if(pcm_queue_size(g_PcmHeadNode[port]) < 1)
				{
					//printf("==============###### ERROR No Pcm Data\n");
					pthread_mutex_unlock(&pcm_mutex); 
					continue;
				}

				Pcm_get_one_frame(port, &stPcmFrame1, pcm_len);


				if(SingleChannel == emChannelLayout)
				{
					sw_len = swr_convert(au_convert_ctx, sw_pcm_out_buffer, readsize,(const unsigned char **)(sw_pcm_in_buffer) , 2 * samples_read); 
					memcpy((void *)pcm_buffer, (void *)sw_pcm_out_buffer[0], sw_len);
				}




				gettimeofday(&start_time, NULL);

				mp2fill_size = twolame_encode_buffer_interleaved(
						encopts, 
						(const short int *)pcm_buffer,
						1152, 
						(unsigned char *)mp2buffer,
						MP2_BUF_SIZE);

				//printf("====> Encoder Success   =======#\n");


				if (mp2fill_size <= 0)
				{
					fprintf(stderr, "[%s]%d: Auido-%d error while encoding audio: %d\n", __func__, __LINE__, port, mp2fill_size);
					break;
				}
				else
				{
					//printf("### ==> es len = %d ##################\n", mp2fill_size);
					stEsFrame.emDataLen = mp2fill_size;
					stEsFrame.emData    = mp2buffer;
					//printf("### Get Pcm PTS\n");
					memcpy(stEsFrame.emPTS, stPcmFrame1.emPTS, 8);
					ret = Es_save_frame(port, &stEsFrame);
					if(ret != 0)
					{
						log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "[%s]%d Audio [%d] save frame failed [%d]!\n", __func__, __LINE__, port, ret);
					}


					packet_get_ts(port);


					Audio_muxer(port);		

				}


				gettimeofday(&end_time, NULL);

				//printf("port:%d -- encode time:%d\n", port,(end_time.tv_usec + 1000000 - start_time.tv_usec) % 1000000);
				if(((end_time.tv_usec + 1000000 - start_time.tv_usec) % 1000000) > 4000)
				{
					printf("port: %d -- out of time: %d ########################=================================================############################\n\n",port, (int)((end_time.tv_usec + 1000000 - start_time.tv_usec) % 1000000));
				}		

				pthread_mutex_unlock(&pcm_mutex);    	
				sched_yield();

			}


			twolame_close(&encopts);

			if(NULL != pcm_buffer)
			{
				free(pcm_buffer);
				pcm_buffer = NULL;
			}

			if(NULL != mp2buffer)
			{
				free(mp2buffer);
				mp2buffer = NULL;
			}


#if FMPEG
			if(NULL != au_convert_ctx)
			{
				av_free(au_convert_ctx);
				au_convert_ctx = NULL;
			}
#endif
		}


#if FMPEG
		for(i = 0; i < 32; ++i)
		{
			if(NULL != sw_pcm_in_buffer[i])
			{
				av_free(sw_pcm_in_buffer[i]);
				sw_pcm_in_buffer[i] = NULL;
			}

			if(NULL != sw_pcm_out_buffer[i])
			{
				av_free(sw_pcm_out_buffer[i]);
				sw_pcm_out_buffer[i] = NULL;
			}
		}
#endif

		Audio_clear_buffer(port);

	}

#if AUDIO_DEBUG
	printf("##Thread:%d Exit!!=================#####################\n", port);
#endif

	Comm_modify_encoder_status(port, NONE);
	pParams->AudioEncoderState[port].state = STOP;

	return 0;

}


int Audio_muxer(int port)
{
	U64  u64Pcr = 0;
	U64  u64Pts = 0;
	U32  u32arrPcr[2] = {0,0};
	//AudioEncoderInfo *pParams = NULL; 
	//pParams = Comm_get_shm_head_ptr();

	struct ts_frame stTsFrame;
	int ret = 0;
	U32   u32FrameTime_3 = 3 * (24 * 90);
	//static int count_ts = 0;
	//count_ts++;
	//U64   *pu64Pts = NULL;
	//U64   *pu64Pcr = NULL;

#if AUDIO_DEBUG
	static int count[4] = {0};
#endif
	for(port = 0; port < 4; port++)
	{
		if( ts_queue_size(g_TsHeadNode[port]) < 1)
		{
			continue;
		}

		REG_Read(VIDEO_TIMEBASE_LOW_REG_ADDR_0 + 2 * port, &u32arrPcr[0]);
		REG_Read(VIDEO_TIMEBASE_HIGH_REG_ADDR_0 + 2 *port, &u32arrPcr[1]);

		memcpy(&u64Pcr, u32arrPcr, 8);
#if AUDIO_DEBUG
		count[port]++;
#endif

		while(1)
		{
			ret = Ts_get_one_frame(port, &stTsFrame);
			if(-1 == ret)
			{
				break;
			}

			memcpy(&u64Pts,stTsFrame.emPTS,8);

#if AUDIO_DEBUG
			if(count[port] > 60)
			{
				printf("===>pcr = %llu\n===>pts = %llu \n", u64Pcr, u64Pts);

				count[port] = 0;
			}
#endif

			// 0 - 3 frame time
			if((u64Pts > u64Pcr) && (u64Pts < (u64Pcr + u32FrameTime_3)))
			{

				Audio_send_ts_packet(&stTsFrame, port);

				Ts_del_one_frame(port);

			}
			else if(u64Pts <= u64Pcr)
			{
				// out of time
				Ts_del_one_frame(port);
			}
			else if(u64Pts >= (u64Pcr + u32FrameTime_3))
			{
				if(u64Pts > (u64Pcr + 4000 * 90))
				{
					//unexpect situation
					Audio_send_ts_packet(&stTsFrame, port);
					Ts_del_one_frame(port);

#if AUDIO_DEBUG
					printf("====> Send Ts Data <=====\n");
					printf("===>pcr = %llu\n===>pts = %llu \n", u64Pcr, u64Pts);
#endif
				}
				else
				{
					break;
				}
			}
		}

	}

	return SUCCESS;

}


int Audio_send_ts_packet(TS_FRAME_T pstTsFrame, int port)
{
	unsigned char 	ts_packet_buffer[PACKET_LEN];

	memset(ts_packet_buffer, 0x0, 8);

	//port | 0x100; the sync add in the lower driver
	ts_packet_buffer[0] = port + 4; 
	memcpy(&ts_packet_buffer[8], pstTsFrame->emData, 188);

	Audio_send_data_logic(ts_packet_buffer);

	return SUCCESS;

}


int Audio_send_data_logic(unsigned char *data)
{
	int  i = 0;
	U32  u32Data = 0;

	//ProcessMutex_wr_lock(g_LockFileFd);
	Sem_wait(g_LockPacketFd);

	for( i = 0; i < PACKET_LEN; i++)
	{
		if(0 == i)
		{
			REG_Write(COMMAND_DATA_SYNC, 0x01);
			REG_Read(COMMAND_DATA_SYNC, &u32Data);
			//printf("#### === COMMAND_DATA_SYNC = %d\n", u32Data);
		}
		REG_Write(COMMAND_DATA_WR_DATA_BUS, data[i]);
		REG_Read(COMMAND_DATA_WR_DATA_BUS, &u32Data);
		if(data[i] != u32Data)
		{
			printf("Logic Reg Error i = %d, write =%d , read = %d \n", i, data[i],u32Data);
		}

#if 0
		if(11 == index)
		{

			value_cc = (0x0f & data[11]);
			if( ((count_cc + 1) % 0x10) != value_cc )
			{
				printf("#### === CC Error ===== ==================================###########\n");
			}

			value_cc = (0x0f & u32Data);
			if( ((count_cc + 1) % 0x10) != value_cc )
			{
				printf("#### === CC Error ===== ==================================###########\n");
			}
			count_cc = value_cc;
		}
		//if(index < 10)
		//printf("#### === COMMAND_DATA_WR_DATA_BUS = %d\n", u32Data);

#endif
		if(0 == i)
		{
			REG_Write(COMMAND_DATA_SYNC, 0x00);
			REG_Read(COMMAND_DATA_SYNC, &u32Data);
			//printf("#### === COMMAND_DATA_SYNC = %d\n", u32Data);
		}	
	}

	Sem_post(g_LockPacketFd);
	//ProcessMutex_file_unlock(g_LockFileFd);


	return SUCCESS;

}


int Audio_get_sample_rate(int port, int *ps32SampleRate)
{
	AudioEncoderInfo * pParams = NULL; 
	U32 value = 0;

	pParams = Comm_get_shm_head_ptr();
	value = pParams->AudioRegister[port].SampleFrequencyReg.value;

	switch(value)
	{
		case AUDIO_SAMPLE_32K:
			{
				*ps32SampleRate = 32000;
				break;
			}

		case AUDIO_SAMPLE_441K:
			{
				*ps32SampleRate = 44100;
				break;
			}

		case AUDIO_SAMPLE_48K:
			{
				*ps32SampleRate = 48000;
				break;
			}

		default:
			{
				*ps32SampleRate = 48000;
			}
	}

	return SUCCESS;
}


int Audio_get_bit_rate(int port, int *ps32BitRate)
{
	AudioEncoderInfo * pParams = NULL; 
	U32 value = 0;

	pParams = Comm_get_shm_head_ptr();
	value = pParams->AudioRegister[port].AudioBitRateReg.value;

	switch(value)
	{
		case ENCODER_HDHDMI_64K:
			{
				*ps32BitRate = 64;
				break;
			}

		case ENCODER_HDHDMI_80K:
			{
				*ps32BitRate = 80;
				break;
			}

		case ENCODER_HDHDMI_96K:
			{
				*ps32BitRate = 96;
				break;
			}

		case ENCODER_HDHDMI_112K:
			{
				*ps32BitRate = 112;
				break;
			}

		case ENCODER_HDHDMI_128K:
			{
				*ps32BitRate = 128;
				break;
			}

		case ENCODER_HDHDMI_160K:
			{
				*ps32BitRate = 160;
				break;
			}

		case ENCODER_HDHDMI_192K:
			{
				*ps32BitRate = 192;
				break;
			}

		case ENCODER_HDHDMI_224K:
			{
				*ps32BitRate = 224;
				break;
			}

		case ENCODER_HDHDMI_256K:
			{
				*ps32BitRate = 256;
				break;
			}

		case ENCODER_HDHDMI_320K:
			{
				*ps32BitRate = 320;
				break;
			}

		case ENCODER_HDHDMI_384K:
			{
				*ps32BitRate = 384;
				break;
			}

		default:
			{
				*ps32BitRate = 128;
				break;
			}
	}

	return SUCCESS;
}



int Audio_wr_pcr_pid(int port)
{
	U32 u32PcrPid = 0;
	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	if(0 == port)
	{
		REG_Write(PID_PCR_REG_ADDR_0 , pParams->AudioRegister[port].PcrPidReg.value);
		REG_Read( PID_PCR_REG_ADDR_0 , &u32PcrPid);
		//printf("===>Read PCR_PID = %u  <===\n", u32PcrPid);
	}
	else if( 1 == port)
	{
		REG_Write(PID_PCR_REG_ADDR_1, pParams->AudioRegister[port].PcrPidReg.value);
		REG_Read( PID_PCR_REG_ADDR_1 , &u32PcrPid);
		//printf("===>Read PCR_PID = %u  <===\n", u32PcrPid);
	}
	else if(2 == port)
	{
		REG_Write(PID_PCR_REG_ADDR_2, pParams->AudioRegister[port].PcrPidReg.value);	
		REG_Read( PID_PCR_REG_ADDR_2 , &u32PcrPid);
		//printf("===>Read PCR_PID = %u  <===\n", u32PcrPid);
	}
	else if(3 == port)
	{
		REG_Write(PID_PCR_REG_ADDR_3, pParams->AudioRegister[port].PcrPidReg.value);
		REG_Read( PID_PCR_REG_ADDR_3 , &u32PcrPid);
		//printf("===>Read PCR_PID = %u  <===\n", u32PcrPid);
	}

	return SUCCESS;
}



int Audio_mutex_init(void)
{
#if 0
	ProcessMutex_init();

	g_LockFileFd = ProcessMutex_file_init(".lockfile");
	ASSERT_APP((g_LockFileFd > 0),"[%s]%d: Err: Can't create communication task!\n", __func__, __LINE__);
#endif

	ProcessMutex_init();

	g_LockPacketFd = Sem_get(SEM_PACKET_KEY, 1, 0666 | IPC_CREAT);
	ASSERT_APP((g_LockPacketFd >= 0),"[%s]%d: Err: Sem_get error!\n", __func__, __LINE__);

	Sem_init(g_LockPacketFd);
	return 0;
}


int Audio_buffer_init(void)
{
	Pcm_init();

	Es_init();

	Ts_init();

	return 0;
}


int  Audio_init_encoder_status(void)
{
	int i = 0;
	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	for(i = 0; i < MAX_AUDIO_NUM_4; i++)
	{
		pParams->AudioEncoderState[i].state = STOP;
		pParams->AudioEncoderState[i].isRunning = false;
		pParams->AudioEncoderState[i].isStop = true;

	}

	return SUCCESS;
}


int Audio_get_delay(int port)
{
	int  video_encode_type = TRANS_VIDEO_TYPE_MPEG2;
	int  audio_bit_rate = ENCODER_HDHDMI_128K;
	int  video_encode_rate = 4000;
	int  delay = 0;
	AudioEncoderInfo *pParams = NULL;

	if(port < 0 || port > 3)
	{
		printf("=====> port Error! <=====\n");
		return 0;
	}

	pParams = Comm_get_shm_head_ptr();

	video_encode_type = pParams->AudioRegister[port].VideoEncodeTypeReg.value;
	audio_bit_rate = pParams->AudioRegister[port].AudioBitRateReg.value;
	video_encode_rate = pParams->AudioRegister[port].VideoEncodeRateReg.value;

	switch(video_encode_type)
	{
		case TRANS_VIDEO_TYPE_MPEG2:
			{
				delay = Audio_get_mpeg2_delay(audio_bit_rate,video_encode_rate);
				break;
			}

		case TRANS_VIDEO_TYPE_H264:
			{
				delay = Audio_get_h264_delay(audio_bit_rate,video_encode_rate);
				break;
			}

		default:
			{
				delay = Audio_get_mpeg2_delay(audio_bit_rate,video_encode_rate);
				break;
			}
	}

	return delay;

}


int Audio_get_h264_delay(int audio_bit_rate, int video_encode_rate)
{
	int delay = 0;

	switch(audio_bit_rate)
	{
		case ENCODER_HDHDMI_128K:
			{
				delay = 600;
				break;
			}

		case ENCODER_HDHDMI_192K:
			{
				delay = 575;
				break;
			}

		default:
			{
				delay = 600;
				break;
			}
	}

	return delay;
}


int Audio_get_mpeg2_delay(int audio_bit_rate, int video_encode_rate)
{
	int delay = 0;

	switch(audio_bit_rate)
	{
		case ENCODER_HDHDMI_128K:
			{
				if(video_encode_rate < 2000)
				{
					delay = 465;
				}
				else if((video_encode_rate >= 2000) && (video_encode_rate <3000))
				{
					delay = 465 - ((video_encode_rate - 2000)/100) * 31;
				}
				else if((video_encode_rate >= 3000) && (video_encode_rate <4000))
				{
					delay = 155 - ((video_encode_rate - 3000)/100) * 15;
				}
				else if((video_encode_rate >= 4000) && (video_encode_rate <6000))
				{
					delay = 10 - ((video_encode_rate - 4000)/100) * 7;
				}
				else if((video_encode_rate >= 6000) && (video_encode_rate <8000))
				{
					delay = -135 - ((video_encode_rate - 6000)/100) * 4;
				}
				else if((video_encode_rate >= 8000) && (video_encode_rate <12000))
				{
					delay = -210 - ((video_encode_rate - 8000)/100)/2;
				}
				else
				{
					delay = -235;
				}

				break;
			}

		case ENCODER_HDHDMI_192K:
			{
				if(video_encode_rate < 2000)
				{
					delay = 465;
				}
				else if((video_encode_rate >= 2000) && (video_encode_rate <3000))
				{
					delay = 465 - ((video_encode_rate - 2000)/100) * 31;
				}
				else if((video_encode_rate >= 3000) && (video_encode_rate <4000))
				{
					delay = 155 - ((video_encode_rate - 3000)/100) * 15;
				}
				else if((video_encode_rate >= 4000) && (video_encode_rate <6000))
				{
					delay = 0 - ((video_encode_rate - 4000)/100) * 7;
				}
				else if((video_encode_rate >= 6000) && (video_encode_rate <8000))
				{
					delay = -140 - ((video_encode_rate - 6000)/100) * 5;
				}
				else if((video_encode_rate >= 8000) && (video_encode_rate <12000))
				{
					delay = -240 - (((video_encode_rate - 8000)/100)*3)/2;
				}
				else
				{
					delay = -300;
				}
				break;
			}

		case ENCODER_HDHDMI_256K:
			{

				if(video_encode_rate < 2000)
				{
					delay = 465;
				}
				else if((video_encode_rate >= 2000) && (video_encode_rate <3000))
				{
					delay = 465 - ((video_encode_rate - 2000)/100) * 31;
				}
				else if((video_encode_rate >= 3000) && (video_encode_rate <4000))
				{
					delay = 155 - ((video_encode_rate - 3000)/100) * 15;
				}
				else if((video_encode_rate >= 4000) && (video_encode_rate <6000))
				{
					delay = 10 - ((video_encode_rate - 4000)/100) * 7;
				}
				else if((video_encode_rate >= 6000) && (video_encode_rate <8000))
				{
					delay = -135 - ((video_encode_rate - 6000)/100) * 4;
				}
				else if((video_encode_rate >= 8000) && (video_encode_rate <10000))
				{
					delay = -210 - ((video_encode_rate - 8000)/100)/2;
				}
				else if((video_encode_rate >= 10000) && (video_encode_rate <12000))
				{
					delay = -250 - ((video_encode_rate - 10000)/100)/2;
				}
				else
				{
					delay = -265;
				}
				break;
			}

		default:
			{

				if(video_encode_rate < 2000)
				{
					delay = 465;
				}
				else if((video_encode_rate >= 2000) && (video_encode_rate <4000))
				{
					delay = 465 - ((video_encode_rate - 2000)/100) * 23;
				}
				else if((video_encode_rate >= 4000) && (video_encode_rate <6000))
				{
					delay = 10 - ((video_encode_rate - 4000)/100) * 7;
				}
				else if((video_encode_rate >= 6000) && (video_encode_rate <8000))
				{
					delay = -135 - ((video_encode_rate - 6000)/100) * 4;
				}
				else if((video_encode_rate >= 8000) && (video_encode_rate <12000))
				{
					delay = -210 - ((video_encode_rate - 8000)/100)/2;
				}
				else
				{
					delay = -235;
				}
				break;
			}
	}

	return delay;
}
