/*************************************************************************
	> File Name: sem.h
	> Author: LGmark
	> Mail: life_is_legend@163.com 
	> Created Time: Sat 10 Sep 2016 12:05:15 AM PDT
 ************************************************************************/

#ifndef SEM_H_
#define SEM_H_
#include <sys/sem.h>

#define SEM_PACKET_KEY  (8008)
#define SEM_REG_KEY     (808)

extern int g_LockPacketFd;
extern int g_LockRegFd;


int Sem_get(key_t key, int nsems, int flag);
int Sem_init(int semid);
int Sem_del(int semid);
int Sem_wait(int semid);
int Sem_post(int semid);

#endif
