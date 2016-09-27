#ifndef COMM_H
#define COMM_H
#include <stdint.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <pthread.h>

#include "log.h"
#include "audio.h"
#include "appGlobal.h"


#define AUDIO_ENCODER_NUM_4     4

extern int g_Rcvmsgid;
//extern int g_Sndmsgid;



typedef struct msg_st
{
	long	msgtype;
	U32		addr;
	U32		value;
}msg_info_t;


typedef struct Reg
{
	U32  addr;
	U32  value;
}Register;

typedef struct audio_register_st
{
	   
	Register 	EncoderStatusReg; //  stop , start , running
	Register 	AudioTsPIDReg;    // 
	Register 	AudioTypeReg;     //
	Register 	AudioBitRateReg;  //
	Register 	SampleFrequencyReg; //
	Register 	ChannelLayoutReg;  //
	Register 	AudioVolumeReg;   //
	Register    EncodeDelayReg;
	Register    PcrPidReg;
	Register    VideoEncodeTypeReg;
	Register    VideoEncodeRateReg;
}audio_register_info_t;


typedef enum
{
    ENCODER_M22_OFF,
    ENCODER_M22_AC3,
    ENCODER_M22_MPEG1_Layer2,
    ENCODER_M22_MPEG2_AAC,
    ENCODER_M22_MPEG4_AAC,
    ENCODER_M22_MPEG2_HE_AAC_V2,
	ENCODER_M22_MPEG4_HE_AAC_V2,
}ENCODER_M22_AudioStreamType;
typedef enum
{
    TRANS_VIDEO_TYPE_H264,
    TRANS_VIDEO_TYPE_MPEG2,
}ENCODER_MULTIAUDIO_VideoEncodeType;


typedef enum{
    AUDIO_SAMPLE_32K,
    AUDIO_SAMPLE_441K,
    AUDIO_SAMPLE_48K,
}AudioSampleType_e;

typedef enum
{
    Stereo,
    JointStereo,
    DualChannel,
    SingleChannel,
}AudioChannelLayout_e;

 
typedef enum
{
    ENCODER_HDHDMI_32K,
    ENCODER_HDHDMI_48K,
    ENCODER_HDHDMI_56K,
    ENCODER_HDHDMI_64K,
    ENCODER_HDHDMI_80K,
    ENCODER_HDHDMI_96K,
    ENCODER_HDHDMI_112K,
    ENCODER_HDHDMI_128K,
    ENCODER_HDHDMI_160K,
    ENCODER_HDHDMI_192K,
    ENCODER_HDHDMI_224K,
    ENCODER_HDHDMI_256K,
    ENCODER_HDHDMI_320K,
    ENCODER_HDHDMI_384K,
    ENCODER_HDHDMI_448K,
}ENCODER_HDHDMI_AudioEncodeRate;


typedef enum
{
    WRITE = 1,
    READ = 2,
}Cmd_Type_t;


typedef enum
{
    STOP = 0,
    START = 1,
    RUNNING  = 2,
    NONE = 0xFF,
}Audio_State_t;


typedef struct audio_encoder_state_st
{
	char isStop;
	char isRunning;
	char  state; //0-stop , 1- run , 2 - running
}audio_encoder_state_info_t;


typedef struct audio_encoder_info_st
{
	audio_encoder_state_info_t AudioEncoderState[4];
	audio_register_info_t  AudioRegister[4];
}AudioEncoderInfo;


int     Comm_share_mem_init(void);

void    Comm_print_params(int port);

int 	Comm_msg_queue_init(void);
int 	Comm_wait_msg(void);
int 	Comm_modify_encoder_status(int port, U32 state);
int 	Comm_get_encoder_status(int port);
AudioEncoderInfo * Comm_get_shm_head_ptr(void);









#endif
