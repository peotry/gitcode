#include "comm.h"

#include "ProcessMutex.h"
#include "sem.h"

int g_Rcvmsgid = -1;
//int g_Sndmsgid = -1;

AudioEncoderInfo *g_AudioEncoderInfoPtr = NULL;

#define MEMKEY    (10086)



int  Comm_share_mem_init(void)
{
	printf("Audio_Encoder>>>>>>>>>>>>>>>>>>>>>>>>============= %d\n", (int)sizeof(AudioEncoderInfo));
	//int shmid = shmget(MEMKEY, sizeof(AudioEncoderInfo), 0666 | IPC_CREAT);
	int shmid = shmget(MEMKEY, 364, 0666 | IPC_CREAT);
	if(shmid < 0)
	{
		perror("shmget error");
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"shmget error\n");
		exit(-1);
	}

	g_AudioEncoderInfoPtr = (AudioEncoderInfo *)shmat(shmid, 0, 0);

	if(g_AudioEncoderInfoPtr < 0)
	{
		perror("shmat error");
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"shmat error\n");
		exit(-1);
	}

	return 0;
}


int Comm_msg_queue_init(void)
{
	g_Rcvmsgid = msgget(REVMSGKEY, IPC_CREAT | 0666);
	ASSERT_APP((g_Rcvmsgid !=  -1),"[%s]%d: Err: Can't create msg_queue!\n", __func__, __LINE__);

	return 0;
}


int Comm_wait_msg(void)
{
	msg_info_t 	rcvmsgs;
	int resRcv = 0;
	resRcv = msgrcv(g_Rcvmsgid, &rcvmsgs, 8, 0, 0);
	if(resRcv < 0)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "msgrcv error\n");
		perror("msgrcv error");
	}

	return rcvmsgs.addr;
}


int Comm_modify_encoder_status(int port, U32 state)
{
	AudioEncoderInfo * pParams = g_AudioEncoderInfoPtr;

	//ProcessMutex_wr_lock(g_LockFileFd);
	Sem_wait(g_LockPacketFd);
	pParams->AudioRegister[port].EncoderStatusReg.value = state;
	Sem_post(g_LockPacketFd);
	//ProcessMutex_file_unlock(g_LockFileFd);

	return 0;
}


int Comm_get_encoder_status(int port)
{
	int state = 0;

	AudioEncoderInfo * pParams = g_AudioEncoderInfoPtr;

	//ProcessMutex_wr_lock(g_LockFileFd);
	Sem_wait(g_LockPacketFd);
	state = pParams->AudioRegister[port].EncoderStatusReg.value;
	Sem_post(g_LockPacketFd);
	//ProcessMutex_file_unlock(g_LockFileFd);

	return state;
}


AudioEncoderInfo * Comm_get_shm_head_ptr(void)
{
	return g_AudioEncoderInfoPtr;
}


void   Comm_print_params(int port)
{

	printf("###====== port: %d =======######\n", port);
	printf("  EncoderStatus = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].EncoderStatusReg.value);
	printf("  AudioTsPID = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].AudioTsPIDReg.value);
	printf("  AudioType = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].AudioTypeReg.value);
	printf("  AudioBitRate = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].AudioBitRateReg.value);
	printf("  SampleFrequency = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].SampleFrequencyReg.value);
	printf("  ChannelLayout = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].ChannelLayoutReg.value);
	printf("  AudioVolume = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].AudioVolumeReg.value);
	printf("  EncodeDelay = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].EncodeDelayReg.value);
	printf("  PcrPidReg = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].PcrPidReg.value);
	printf("  VideoEncodeType = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].VideoEncodeTypeReg.value);
	printf("  VideoEncodeRate = %u\n", g_AudioEncoderInfoPtr->AudioRegister[port].VideoEncodeRateReg.value);

}



