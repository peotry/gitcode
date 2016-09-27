/**********************************************************************
* Copyright(c)2013, HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ��FPGA.h
* Description ��driver of FPGA
*
* Author ��liwei   Reviewer :
* Date   ��2013-09-03
*
**********************************************************************/

#ifndef ALTERA_SOCFPGA_HELLOWORLD_LINUX_GNU_0_FPGA_H_
#define ALTERA_SOCFPGA_HELLOWORLD_LINUX_GNU_0_FPGA_H_

#include "appGlobal.h"
#include <sys/sem.h>

//#define MEMORY_DEV_NAME  "/dev/cpu_comm_interface_0"
#define MEMORY_DEV_NAME  "/dev/mem"

#define CHIP_TYPE_REG_0              0x713
#define CHIP_TYPE_REG_1              0x712

//֪ͨ�߼�H46λ�üĴ���
#define ENCODER_H46_POSITION                  (0x715)

#define CHIP_TYPE_86391_0            (0)
#define CHIP_TYPE_86391_1            (1)
#define CHIP_TYPE_86391_2            (2)
#define CHIP_TYPE_86391_3            (3)
#define CHIP_TYPE_2970_0             (4)
#define CHIP_TYPE_2970_1             (5)
#define CHIP_TYPE_2970_2             (6)
#define CHIP_TYPE_2970_3             (7)
#define CHIP_TYPE_5358               (8)
#define CHIP_TYPE_ALL                (9)

//�߼��Ĵ�������
#define GS2970_SPI_SWITCH_REG_ADDR          0x120

//DDR ����BUFFER���أ�MB86391+MB86391����ϣ�����Ҫ����
#define DDR_BUFFER_SWITCH_0                   (0x332)
#define DDR_BUFFER_SWITCH_1                   (0x333)

#define ASI_REG_Tsin0Status          0x610   //read-only bit0:locked, bit1:188/204
#define ASI_REG_Tsin0TotalRate       0x612   //read-only
#define ASI_REG_Tsin0EffectiveRate   0x611   //read-only

#define TSIN0_VIDEO_RATE			 0x608	
#define TSIN0_AUDIO_RATE			 0x609
#define TSIN1_VIDEO_RATE			 0x60a
#define TSIN1_AUDIO_RATE			 0x60b
#define TSIN2_VIDEO_RATE			 0x620
#define TSIN2_AUDIO_RATE			 0x60d
#define TSIN3_VIDEO_RATE			 0x621
#define TSIN3_AUDIO_RATE			 0x60f

//����ͳ��
#define PID_REG_BASE                          (0x600)
#define RATE_REG_BASE                         (0x608)
//CH0
#define VIDEO_PID_REG_CH0                     (0x600)
#define AUDIO_PID_REG_CH0                     (0x601)

#define VIDEO_RATE_REG_CH0                    (0x608)
#define AUDIO_RATE_REG_CH0                    (0x609)
//CH1
#define VIDEO_PID_REG_CH1                     (0x602)
#define AUDIO_PID_REG_CH1                     (0x603)

#define VIDEO_RATE_REG_CH1                    (0x60A)
#define AUDIO_RATE_REG_CH1                    (0x60B)
//CH2
#define VIDEO_PID_REG_CH2                     (0x604)
#define AUDIO_PID_REG_CH2                     (0x605)

#define AUDIO_RATE_REG_CH2                    (0x60D)
//CH3
#define VIDEO_PID_REG_CH3                     (0x606)
#define AUDIO_PID_REG_CH3                     (0x607)

#define AUDIO_RATE_REG_CH3                    (0x60F)

#define av_audio_ak5358_pdn_cs       (0x714)

#define ENCODER0_AUDIO_SOURCE                 (0x716)
#define ENCODER1_AUDIO_SOURCE                 (0x717)
#define ENCODER2_AUDIO_SOURCE                 (0x718)
#define ENCODER3_AUDIO_SOURCE                 (0x719)

//���magnum��AC3����Ƶ����Ҫ�Ӷ�Ӧ��Ŀ��PCR����ȡ����Ҫ֪ͨ�߼���Ӧ��PCR_PID��ֵ
#define AUDIO_PCR_PID_0                       (0x71A)
#define ADUIO_PCR_PID_1                       (0x71B)

//��Ƶ������PID�Ĵ�������2 & 3 λ�ã�ֻ����Ƶ��ͨ������Ҫд��PID
#define AUDIO_TS_FILTER_CH2                   (0x720)
#define AUDIO_TS_FILTER_CH3                   (0x721)

//MB86391��H46���ụ̀ܽ����86391�Ĺ���Ƶ�ʵ�������H46����Ҫ
//�����Ҫ֪ͨ�߼����ǲ�������Ӱ� 1Ϊ89391,0ΪH46
#define ENCODE_CLOCK_SWITCH_REG               (0x728)

//AC3 TS�� ����buffer�Ŀ���
#define TS_SWITCH_REG_BASE                    (0x729)
#define TS_SWITCH_REG_CH0                     (0x729)
#define TS_SWITCH_REG_CH1                     (0x72A)
#define TS_SWITCH_REG_CH2                     (0x72B)
#define TS_SWITCH_REG_CH3                     (0x72C)

#define CHIP_RESET_REG               (0x731)
#define BUFFER_REG                   (0x732)
#define SDIControl_Reg               (0x733)   //bit(0--1):

//����MB86391��I2S��λ���ӣ�������Ƶ��С,д���ֵ X = 64 * scale
//����PCMֵ��С0.5����д���ֵΪ X = 64*0.5 = 32
#define I2S_SCALE_REG_BASE                     0x734
#define I2S_SCALE_REG_CH0                      0x734   //A1  - PORT1
#define I2S_SCALE_REG_CH1                      0x736   //B1  - PORT0
#define I2S_SCALE_REG_CH2                      0x735   //A2  - PORT3
#define I2S_SCALE_REG_CH3                      0x737   //B2  - PORT2

// 20150115 �߼�SDT���²巽ʽ�޸�
// ����Ƶ��·�Ĳ���SDT��дRAM
#define SDT_RAM0_WREN                          0x750
#define SDT_RAM0_ADDR                          0x751
#define SDT_RAM0_WDAT                          0x752
#define SDT_RAM1_WREN                          0x753
#define SDT_RAM1_ADDR                          0x754
#define SDT_RAM1_WDAT                          0x755

// Mugnum����SDT��дRAM
#define MAGNUM_SDT_RAM_WREN                    0x756
#define MAGNUM_SDT_RAM_ADDR                    0x757
#define MAGNUM_SDT_RAM_WDAT                    0x758
 

//��Ƶbuffer�ӳٿ��ƼĴ���
#define AUDIO_BUFFER_REG_BASE                 (0x700)
#define AUDIO_BUFFER_REG_CHO                  (0x700)
#define AUDIO_BUFFER_REG_CH1                  (0x701)
#define AUDIO_BUFFER_REG_CH2                  (0x702)
#define AUDIO_BUFFER_REG_CH3                  (0x703)

#define REG_COMM_CMD_ADD                        (0x12B)       //6bit , W+R
#define REG_COMM_CMD_FINISH                     (0x12C)       //1bit , W+R
#define REG_COMM_CMD_EXIST                      (0x12E)       //1bit , R      interrupt
#define REG_COMM_CMD_DELETE                     (0x23)       //8bit , W+R    don't use
#define REG_COMM_CMD_RDATA                      (0x12D)       //32bit, R

#define command_data                            (0x18A)
#define command_sync                            (0x18B)
#define command_valid                           (0x18C)
#define REG_COMM_SLOT_ID            			(0x18D)
#define AVS_RESET_REG				(0x909)
#define CS_CHIP_REG                 (0x90A)

#define LED_PIN_RED         52             /* red led pin   : MIO 52 */
#define LED_PIN_GREEN       53             /* green led pin : MIO 53 */

//logic reset reg
#define REG_LOGIC_RESET              0x100
//logic ver reg
#define REG_LOGIC_VERSION            0xFF4
//logic time reg
#define REG_LOGIC_TIME               0xFF5
#define REG_HARDWARE_VERSION         0x103

#define NIOSII_CPU_COM_INTERFACE_BASE   0x0
#define HPS_REG_SPAN					0x3C000000
#define HPS_REG_BASE					0xC0000000

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short * array;
};


/*****************************************************************************
  Function:     Reg_Map
  Description:  map address
  Input:        none
  Output:       none
  Return:       none
  Author:       liwei
*****************************************************************************/
void Reg_Map(void);

/*****************************************************************************
  Function:     FPGA_LED_RED
  Description:  Make led red
  Input:        none
  Output:       none
  Return:       none
  Author:       dadi.zeng
*****************************************************************************/
void FPGA_LED_RED(void);

/*****************************************************************************
  Function:     FPGA_LED_GREEN
  Description:  Make led green
  Input:        none
  Output:       none
  Return:       none
  Author:       dadi.zeng
*****************************************************************************/
void FPGA_LED_GREEN(void);

/*****************************************************************************
  Function:     FPGA_LED_CLOSE
  Description:  Close led
  Input:        none
  Output:       none
  Return:       none
  Author:       dadi.zeng
*****************************************************************************/
void FPGA_LED_CLOSE(void);

/************************add by yaoliang.sun start***************************/
#define BACK_IP_ADDESS              0x50
#define BACK_IP_DATA1               0x51
#define BACK_IP_DATA2               0x56
#define BACK_IP_EN                  0x52
#define STREAM_OUT_EN               0x54        //bit0:main,bit1:backup
#define PACKET_NUM                  0x45
void REG_Read(U32 u32RegAddress, U32 *pu32DataValue);
void REG_Write(U32 u32RegAddress, U32  u32DataValue);
void REG_WriteWithReadBack(U32 u32RegAddress, U32  u32DataValue);

int  REG_InitLogicSwitch(void);
int  REG_LockLogicSwitch(void);
int  REG_FreeLogicSwitch(void);
int  REG_DelLogicSwitch(void);

/************************add by yaoliang.sun end  ***************************/

#endif /* ALTERA_SOCFPGA_HELLOWORLD_LINUX_GNU_0_FPGA_H_ */


