#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "ProcessMutex.h"
#include "log.h"

static pthread_mutex_t *mptr;

int g_LockFileFd = -1;

int ProcessMutex_file_init(char * pathname)
{
	int fd = -1;

	fd = open(pathname, O_RDWR | O_CREAT, 0666);
	if(fd < 0)
	{
		perror("open error");
		exit(-1);
	}

	return fd;
}


int ProcessMutex_wr_lock(int fd)
{
	if(fd < 0)
	{
		return -1;
	}

	struct flock stLock;
	stLock.l_type = F_WRLCK;
	stLock.l_whence = SEEK_SET;
	stLock.l_start = 0;
	stLock.l_len   = 1;

	if(fcntl(fd, F_SETLKW, &stLock) < 0)
	{
		exit(-1);
	}

	return 0;
}

int ProcessMutex_rd_lock(int fd)
{
	if(fd < 0)
	{
		return -1;
	}

	struct flock stLock;
	stLock.l_type = F_RDLCK;
	stLock.l_whence = SEEK_SET;
	stLock.l_start = 0;
	stLock.l_len   = 1;

	if(fcntl(fd, F_SETLKW, &stLock) < 0)
	{
		return -1;
	}

	return 0;
}



int  ProcessMutex_file_unlock(int fd)
{
	if(fd < 0)
	{
		return -1;
	}

	struct flock stLock;
	stLock.l_type = F_UNLCK;
	stLock.l_whence = SEEK_SET;
	stLock.l_start = 0;
	stLock.l_len = 1;

	if(fcntl(fd, F_SETLKW, &stLock) < 0)
	{
		return -1;
	}

	return 0;
}




//������λ�ڹ����ڴ����������ڲ�ͬ����֮������������û�����������ΪPTHREAD_PROCESS_SHARED������ʼ����������
void ProcessMutex_init(void)
{
	pthread_mutexattr_t mattr;
	//fd=open("./mm.c", O_RDWR, 0);
	//if(fd < 0)
	//{
	//	printf("error\n");
	//}
	//mptr=mmap(0, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	//close(fd);
	int shmid = shmget((key_t)1234, sizeof(pthread_mutex_t), IPC_CREAT | 0666);
	if(shmid < 0)
	{
		perror("shmget error");
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"shmget error\n");
		exit(-1);
	}

	mptr = (pthread_mutex_t *)shmat(shmid, 0, 0);
	if(mptr < 0)
	{
		perror("shmat error");
		log_printf(LOG_LEVEL_ERROR, LOG_MODULE_AUDIO,"shmat error\n");
		exit(-1);
	}

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mptr, &mattr);

}


void ProcessMutex_lock(void)
{
	pthread_mutex_lock(mptr);
}

void ProcessMutex_unlock(void)
{
	pthread_mutex_unlock(mptr);
}
