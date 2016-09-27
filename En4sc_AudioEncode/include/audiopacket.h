#ifndef _AUDIO_PACKET_H
#define _AUDIO_PACKET_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/time.h> 

#define MAX_PACKET_CACHE_NUM    40
#define FRAME_NUM_PER_PES       2

#define READ_LOCK_TIMEOUT       4
#define MAX_AUDIO_FRAME_SIZE    (0x2000) 

#define SPI_DEV_NAME 		"/dev/spidev32766.0"

#define SPI_FIFO_NAME       "/run/spi_fifo"


int packet_get_pes(unsigned char * pesbuffer, int audio_index, unsigned int * u32arrPts);

int packet_get_ts(int port);



#endif


