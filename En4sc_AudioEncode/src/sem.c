/*************************************************************************
  > File Name: sem.c
  > Author: LGmark
  > Mail: life_is_legend@163.com 
  > Created Time: Sat 10 Sep 2016 12:14:39 AM PDT
 ************************************************************************/

#include "sem.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int g_LockPacketFd = -1;
int g_LockRegFd = -1;

union semun{
	int val;
	struct sem_ds *buf;
	unsigned short *array;

};

int Sem_get(key_t key, int nsems, int flag)
{
	int semid = 0;

	semid = semget(key, nsems, flag);
	if(-1 == semid)
	{
		perror("Sem_get error");
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"Sem_get error\n");
		exit(-1);
	}

	return semid;
}


int Sem_init(int semid)
{
	union semun sem_union;

	sem_union.val = 1;
	if(-1 == semctl(semid , 0, SETVAL, sem_union))
	{
		perror("Sem_init error");
		return -1;
	}

	return 0;
}


int Sem_del(int semid)
{
	union semun sem_union;

	if(-1 == semctl(semid, 0, IPC_RMID, sem_union))
	{
		perror("Sem_del error");
		return -1;
	}

	return 0;
}


int Sem_wait(int semid)
{
	struct sembuf sem_buf;

	sem_buf.sem_num = 0;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = SEM_UNDO;
	if(-1 == semop(semid, &sem_buf, 1))
	{
		perror("Sem_wait error");
		return -1;
	}

	return 0;
}


int Sem_post(int semid)
{
	struct sembuf sem_buf;

	sem_buf.sem_num = 0;
	sem_buf.sem_op = 1;
	sem_buf.sem_flg = SEM_UNDO;

	if(-1 == semop(semid, &sem_buf, 1))
	{
		perror("Sem_post error");
		return -1;
	}

	return 0;
}



