#include <stdio.h>
#include "twolame.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include "FPGA.h"
#include "appGlobal.h"
#include "audiopacket.h"
#include "audio.h"
#include "pcm.h"
#include "log.h"

#include "comm.h"
#include "es.h"
#include "ProcessMutex.h"
#include "sem.h"


#define ECODER_TASK 0
#define SEND_TASK   1
#define PCM_TASK    1
#define PACKET_TS 0


/***********************************************************************
 *
 *  void comm_sig_func()
 *
 *  Function:
 *
 *      Receive information which from mainboard and parse it
 *
 *  Returns:
 *      None
 *
 ***********************************************************************/
void comm_sig_func(int number)
{
	pid_t pid_ctrl = -1;

	printf("## get signo = %d", number);

	pid_ctrl = getppid();

	kill(pid_ctrl, SIGUSR1);

}

/***********************************************************************
 *
 *  int comm_fasync()
 *
 *  Function:
 *
 *      Bind process to signal which from irq.
 *      Running the sig-function when irq generate the signal
 *
 *  Returns:
 *      None
 *
 ***********************************************************************/
int comm_fasync()
{
	signal(SIGUSR1, comm_sig_func);

	return 0;
}



void close_event_fd(void)
{
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		if(g_AudioFd[i] > 0)
		{
			close(g_AudioFd[i]);
		}
	}
}


void  close_msg_queue(void )
{
	if(g_Rcvmsgid > 0)
	{
		printf("Close msg queue ===========\n");
		msgctl(g_Rcvmsgid, IPC_RMID, 0);
		g_Rcvmsgid = -1;
	}


}


void process_atexit(void)
{
	printf("######## process atexit #############\n");
	//atexit(close_msg_queue);
	atexit(close_event_fd);
}


void handle_signal(int signo)
{
	switch(signo)
	{
		case SIGABRT:
			{
				printf("process is killed by SIGABRT\n");
				break;
			}
		case SIGINT:
			{
				printf("process is killed by SIGINT\n");
				break;
			}
		case SIGQUIT:
			{
				printf("process is killed by SIGQUIT\n");
				break;
			}
		case SIGSEGV:
			{
				printf("process is killed by SIGSEGV\n");
				break;
			}
		case SIGHUP:
			{
				printf("process is killed by SIGHUP\n");
				break;
			}
		case SIGILL:
			{
				printf("process is killed by SIGILL\n");
				break;
			}
		case SIGTRAP:
			{
				printf("process is killed by SIGTRAP\n");
				break;
			}
		default:
			{
				printf("process is killed by SIGNO:%d\n", signo);
				break;
			}
	}

	printf("process is killed by SIGNO:%d\n", signo);
	log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO, "process is killed by SIGNO:%d\n", signo);
	exit(-1);
}

void capture_signal(void)
{
	signal(SIGHUP, handle_signal);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGILL, handle_signal);
	signal(SIGABRT, handle_signal);
	signal(SIGBUS, handle_signal);
	signal(SIGSEGV, handle_signal);
	signal(SIGPIPE, handle_signal);
	signal(SIGALRM, handle_signal);
	signal(SIGTERM, handle_signal);

}



int main(int argc, char** argv) 
{
	Reg_Map();

	log_InitCtrlHandler();

	log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_AUDIO, "\n========== Audio encoder ===========\n");

	comm_fasync();

	//REG_Write(,U32 u32Data)

	//init mutex
	Audio_mutex_init();

	Audio_buffer_init();

	Audio_init_ffmpeg();

	// buffer_init();
	process_atexit();

	capture_signal();

	//init audio share memory
	Comm_share_mem_init();

	//init
	Comm_msg_queue_init();

	//init audio encoder state
	Audio_init_encoder_status();

	Pcm_create_task();

#if 0
	for(i = 0; i < MAX_AUDIO_NUM_4; ++i)
		//for(i = 0; i < 4; ++i)
	{
		//������Ƶ�����߳�
		res = create_audio_encode_task(&index[i]);
		ASSERT_APP((res == SUCCESS),"[%s]%d: Err: Can't create audio encode task!\n", __func__, __LINE__);
	}
#endif


	int port = 0;
	int encoder_state = STOP;
	int  audio_index[AUDIO_ENCODER_NUM_4] = {0,1,2,3};

	AudioEncoderInfo *pParams = NULL;
	pParams = Comm_get_shm_head_ptr();

	while(1)
	{
		printf("======> Wait Audio Ctrl Msg =====>\n");
		port = Comm_wait_msg();
		printf("Get Message port : %d\n",port);

		if(port < 0 || port > 3)
		{
			continue;
		}

		encoder_state = Comm_get_encoder_status(port);

		if(STOP == encoder_state)
		{
			pParams->AudioEncoderState[port].isStop = true;
			pParams->AudioEncoderState[port].isRunning = false;
			printf("==>Stop AudioEncoder : %d\n", port);
		}

		if(START == encoder_state)
		{
			Comm_print_params(port);
			if(STOP == pParams->AudioEncoderState[port].state)
			{
				create_audio_encode_task(&audio_index[port]);
				printf("==>Start AudioEncoder : %d\n", port);
			}
		}

	}
	//log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_AUDIO, "[%s]%d: Exit audio encoder!\n", __func__, __LINE__);

	return 0;
}
