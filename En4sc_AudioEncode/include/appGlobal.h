#ifndef _GLOBAL_APP_H
#define _GLOBAL_APP_H

#include <stdint.h>





#define DEBUG_MARK   {printf("[%s]%d == Mark ==\n", __func__, __LINE__);}
  
#define DEBUG_SWITCH 0
#define APTS_DEBUG 0

#define SNDMSGKEY  (2048)
#define REVMSGKEY  (1024)

#define SUCCESS (0)
#define ERROR   (-1)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define offsetofs(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetofs(type,member) );})




//****************** PCM Logic Register ********************//
#define I2S_BIT_MODE					0x77D

//pingpang_len (4 group addr)
#define PING_PANG_LEN_REG_ADDR_0          0x770
#define PING_PANG_LEN_REG_ADDR_1          0x771
#define PING_PANG_LEN_REG_ADDR_2          0x772
#define PING_PANG_LEN_REG_ADDR_3          0x773


//PTS TIMEBASE
#define PCM_PTS_PING_LOW_0_REG_ADDR     0x780
#define PCM_PTS_PING_HIGH_0_REG_ADDR    0x781
#define PCM_PTS_PONG_LOW_0_REG_ADDR     0x782
#define PCM_PTS_PONG_HIGH_0_REG_ADDR    0x783
#define PCM_PTS_PING_LOW_1_REG_ADDR     0x784
#define PCM_PTS_PING_HIGH_1_REG_ADDR    0x785
#define PCM_PTS_PONG_LOW_1_REG_ADDR     0x786
#define PCM_PTS_PONG_HIGH_1_REG_ADDR    0x787
#define PCM_PTS_PING_LOW_2_REG_ADDR     0x788
#define PCM_PTS_PING_HIGH_2_REG_ADDR    0x789
#define PCM_PTS_PONG_LOW_2_REG_ADDR     0x78A
#define PCM_PTS_PONG_HIGH_2_REG_ADDR    0x78B
#define PCM_PTS_PING_LOW_3_REG_ADDR     0x78C
#define PCM_PTS_PING_HIGH_3_REG_ADDR    0x78D
#define PCM_PTS_PONG_LOW_3_REG_ADDR     0x78E
#define PCM_PTS_PONG_HIGH_3_REG_ADDR    0x78F


//Read PCM Data Reg
//Read the data from the reg
#define PCM_READ_DATA_REG_ADDR_0		0x790
#define PCM_READ_DATA_REG_ADDR_1		0x791
#define PCM_READ_DATA_REG_ADDR_2		0x792
#define PCM_READ_DATA_REG_ADDR_3		0x793

//Write the data addr
#define PCM_READ_ADDR_REG_0				0x794
#define PCM_READ_ADDR_REG_1				0x795
#define PCM_READ_ADDR_REG_2				0x796
#define PCM_READ_ADDR_REG_3				0x797


//PCM Data Flag
#define PCM_REQ_FLAG_REG_ADDR_0         0x7A0
#define PCM_REQ_FLAG_REG_ADDR_1         0x7A1
#define PCM_REQ_FLAG_REG_ADDR_2         0x7A2
#define PCM_REQ_FLAG_REG_ADDR_3         0x7A3


#define RD_PCM_ACK_REG_ADDR_0           0x7B0 
#define RD_PCM_ACK_REG_ADDR_1           0x7B1 
#define RD_PCM_ACK_REG_ADDR_2           0x7B2 
#define RD_PCM_ACK_REG_ADDR_3           0x7B3 



//Write the pcr pid
#define PID_PCR_REG_ADDR_0              0x71A
#define PID_PCR_REG_ADDR_1              0x71B
#define PID_PCR_REG_ADDR_2              0x70C
#define PID_PCR_REG_ADDR_3              0x70D


//Read the pcr time base
#define VIDEO_TIMEBASE_HIGH_REG_ADDR_0   0x7B4
#define VIDEO_TIMEBASE_LOW_REG_ADDR_0    0x7B5
#define VIDEO_TIMEBASE_HIGH_REG_ADDR_1   0x7B6
#define VIDEO_TIMEBASE_LOW_REG_ADDR_1    0x7B7
#define VIDEO_TIMEBASE_HIGH_REG_ADDR_2   0x7B8
#define VIDEO_TIMEBASE_LOW_REG_ADDR_2    0x7B9
#define VIDEO_TIMEBASE_HIGH_REG_ADDR_3   0x7BA
#define VIDEO_TIMEBASE_LOW_REG_ADDR_3    0x7BB



//PCM  Enable
#define PCM_ENABLE_REG					0xC0

//select audio channel
#define SELECT_AUDIO_REG_0              0x716
#define SELECT_AUDIO_REG_1				0x717
#define SELECT_AUDIO_REG_2				0x718
#define SELECT_AUDIO_REG_3 				0x719

//audio scale
#define SCALE_FACTOR00_REG					0x760
#define SCALE_FACTOR01_REG					0x761
#define SCALE_FACTOR02_REG					0x762


#define SCALE_FACTOR10_REG					0x763
#define SCALE_FACTOR11_REG					0x764
#define SCALE_FACTOR12_REG					0x765


#define SCALE_FACTOR20_REG					0x766
#define SCALE_FACTOR21_REG					0x767
#define SCALE_FACTOR22_REG					0x768

#define SCALE_FACTOR30_REG					0x769
#define SCALE_FACTOR31_REG					0x76a
#define SCALE_FACTOR32_REG					0x76b

//ts channel
#define COMMAND_DATA_WR_DATA_BUS           0x18A
#define COMMAND_DATA_SYNC                  0x18B



#define AUDIO_TEST_MODE                    0xc5






//************************* PCM Logic Register End ********************// 


#define AUDIO_DEBUG     (0)

#define FMPEG    (1)

#define PACKET_LEN            (196)
#define PING_PANG_SIZE        (8192)


#define MAX_AUDIO_NUM_4     (4)


//type
typedef unsigned char                 U8;
typedef unsigned short                U16;
typedef unsigned int                U32;
typedef unsigned long long                U64;

typedef signed char             S8;
typedef signed short            S16;
typedef signed int              S32;
typedef long long               S64;


#endif


