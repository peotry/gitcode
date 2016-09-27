/*****************************************************************************
 * Copyright(c)2013, HuiZhou WELLAV Technology Co.,Ltd.
 * All rights reserved.
 *
 * FileName ��FPGA.c
 * Description ��driver of FPGA
 *
 * Author ��liwei   Reviewer :
 * Date   ��2013-09-03
 *
 *****************************************************************************/

#ifndef FPGA_C_
#define FPGA_C_

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "appGlobal.h"
#include "ProcessMutex.h"
#include "sem.h"
#include "FPGA.h"

#include "log.h"

#include <stdint.h>

#define HW_REGS_BASE ( ALT_STM_OFST )
#ifdef RELEASE_VER
#define __func__ "FPGA"
#endif

static void *s_pLogicAddress;
void *s_pGPIOAddress;
volatile void * s_pAudioAddress ;
static int g_sem_id;

/*****************************************************************************
Function:     Reg_Map
Description:  map address
Input:        none
Output:       none
Return:       none
Author:       liwei
modify by yaoliang.sun in 2014-07-04
 *****************************************************************************/
void Reg_Map()
{
//	U32 i = 0;
	S32 memfd = 0;
//	void *h2p_lw_led_addr;

	//memfd = open(MEMORY_DEV_NAME, O_RDWR | O_SYNC);
	memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if(memfd == -1)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't open /dev/mem.\n");
		return;
	}
	else
	{
		log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "#### open /dev/mem successfully.###\n");
	}

	/*
	   s_pAudioAddress = mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, (__off_t)AXI_AUDIO_ADDR);
	   if(s_pAudioAddress == (void *) -1)
	   {
	   log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "#### Can't map the memory to audio space.###\n");
	   return;
	   }
	   else
	   {
	   log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "#### Map the memory to audio space successfully.###\n");  
	   }
	   */

	//s_pLogicAddress = mmap(0, HPS_REG_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, (__off_t)HPS_REG_BASE);
	s_pLogicAddress = mmap(0, 0x3C000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, (__off_t)0xC0000000);
	if(s_pLogicAddress == (void *) -1)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't map the memory to user space.\n");
		return;
	}
	printf("s_pLogicAddress is 0x%p\n",s_pLogicAddress);

#if 0
	//s_pGPIOAddress = mmap(0, 0x200000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, (__off_t)ALT_LWFPGASLVS_OFST);
	s_pGPIOAddress = mmap(0, 0x3c000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, (__off_t)0xc0000000);
	if(s_pGPIOAddress == (void *) -1)
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't map the memory to user space.\n");
		return;
	}
	printf("s_pGPIOAddress is 0x%x\n",s_pGPIOAddress);

	//h2p_lw_led_addr=(s_pGPIOAddress + ( 0x100000 ) +( 0x794*4 ));
	h2p_lw_led_addr=(s_pLogicAddress + ( 0 ) +( 0x794*4 ));
	//while(1)
	{ 
		for(i = 0 ; i<1000000 ; ++i)
		{        
			*(uint32_t *)h2p_lw_led_addr = i;
			//printf("h2p_lw_led_addr set 1 \r\n");
			//usleep(500000);        
			//*(uint32_t *)h2p_lw_led_addr = 0xa;
			//printf("h2p_lw_led_addr set 2 \r\n");
		}
		//usleep(500000);

	}
#endif
}

#if 1
/*****************************************************************************
Function:     Reg_Read
Description:  read register
Input:        u32Address     --- address of register
Output:       pu32Data       --- value of register
Return:       none
Author:       huangxun
 *****************************************************************************/
void REG_Read(U32 u32Address, U32 *pu32Data)
{
	ProcessMutex_lock();

	U32 *h2f_component_reg_rd_addr;
	h2f_component_reg_rd_addr = ((U32 *)s_pLogicAddress + 0x0 + u32Address);
	*pu32Data = *h2f_component_reg_rd_addr;

	ProcessMutex_unlock();
}

/*****************************************************************************
Function:     Reg_Write
Description:  write register
Input:        u32Address      --- address of register
u32Data         --- value of register
Output:       none
Return:       none
Author:       huangxun
 *****************************************************************************/
void REG_Write(U32 u32Address, U32 u32Data)
{
	//U8  u8MaxTrytimes = 3;
//	U32 u32Readback = 0x00;

	ProcessMutex_lock();

	U32 *h2f_component_reg_wr_addr;
	h2f_component_reg_wr_addr = ((U32 *)s_pLogicAddress + 0x0 + u32Address);
	*h2f_component_reg_wr_addr = u32Data;

	ProcessMutex_unlock();
	/*
	   do
	   {
	   IOWR(NIOSII_CPU_COM_INTERFACE_BASE, u32Address | WRITE_REG_COMMAND, u32Data);
	   u32Readback = IORD(NIOSII_CPU_COM_INTERFACE_BASE, u32Address | READ_REG_COMMAND);
	   if(u32Readback == u32Data)
	   {
	   return;
	   }
	   else
	   {
#if 0
LogPrint(LOG_TYPE_ERR,"[REG_Write] Failure%d(addr:0x%x, wr:0x%x, rb:0x%x)\r\n",
(3 - u8MaxTrytimes), u32RegAddress, u32DataValue, u32Readback);
#endif
usleep(10);
}

u8MaxTrytimes--;
}while(u8MaxTrytimes > 0);
*/
#if 0
	LogPrint(LOG_TYPE_INF, "[REG_Write] Write reg(addr: 0x%x, value: 0x%x) failure!! \r\n", u32RegAddress, u32DataValue);
#endif
	}

#endif
#endif

/*******************************add by yaoliang.sun start***********************/
#define REG_MAX_TRYTIMES                (5)
void REG_WriteWithReadBack(U32 u32RegAddress, U32  u32DataValue)
{
	U32         u32Readback = 0x00;
	int         nMaxTrytimes = REG_MAX_TRYTIMES;

	do
	{
		// write 
		//AXI_CPU_LOGIC_INF_mWriteReg((U32)s_pLogicAddress, u32RegAddress<<4, u32DataValue);

		// read back and check
		//u32Readback = AXI_CPU_LOGIC_INF_mReadReg((U32)s_pLogicAddress, u32RegAddress<<4);

		if (u32Readback == u32DataValue)
		{
			//break;
			return;
		}
		else
		{
			//log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,"[REG_WriteWithReadBack] Failure%d(addr:0x%x, wr:0x%x, rb:0x%x)\r\n", 
			//                (5 - nMaxTrytimes), u32RegAddress, u32DataValue, u32Readback);
			usleep(10000);
		}

		nMaxTrytimes--;

	} while(nMaxTrytimes > 0);
}
/*******************************add by yaoliang.sun end  ***********************/

int REG_InitLogicSwitch(void)
{
	/* ���� */
	union semun sem_union;

	/* ��ʼֵ */
	sem_union.val = 1;

	/* ���KEY */
	g_sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);    

	/* ��ʼ���ź��� */
	if(-1 == semctl(g_sem_id, 0, SETVAL, sem_union))
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s]%d: Init semphore is failed!\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int REG_LockLogicSwitch(void)
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;

	if(-1 == semop(g_sem_id, &sem_b, 1))
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s]%d: Lock logic failed!\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}


int REG_FreeLogicSwitch(void)
{

	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	if(-1 == semop(g_sem_id, &sem_b, 1))
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s]%d: Free logic failed!\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int REG_DelLogicSwitch(void)
{
	union semun sem_union;

	if(-1 == semctl(g_sem_id, 0, IPC_RMID, sem_union))
	{
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s]%d: Delete logic failed!\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

#define RESET_CHIP_DELAY_US     50000
int REG_ResetChip(U8 CHIPType)
{
	// VARS
	U8  u8RstRegValue = 0;

	log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "[REG_ResetChip] **** Reset chip type: 0x%X  ****\r\n", CHIPType);
	//
	switch(CHIPType)
	{
		case CHIP_TYPE_2970_0:
			{
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = u8RstRegValue & 0xEF; // b1110 1111
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}
		case CHIP_TYPE_2970_1:
			{
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = u8RstRegValue & 0xDF; // b1101 1111
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}
		case CHIP_TYPE_2970_2:
			{
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = u8RstRegValue & 0xBF; // b1011 1111
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}
		case CHIP_TYPE_2970_3:
			{
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = u8RstRegValue & 0x7F; // b0111 1111
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}
		case CHIP_TYPE_5358:
			{
				u8RstRegValue = 0xFF;
				REG_Write(av_audio_ak5358_pdn_cs, 1);
				usleep(RESET_CHIP_DELAY_US);
				REG_Write(av_audio_ak5358_pdn_cs, 0);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(av_audio_ak5358_pdn_cs, 1);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}
		case CHIP_TYPE_ALL:
			{
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0; // b0111 1111
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				u8RstRegValue = 0xFF;
				REG_Write(CHIP_RESET_REG, u8RstRegValue);
				usleep(RESET_CHIP_DELAY_US);
				break;
			}

		default:
			{
				break;
			}
	}
	return 0;
}


