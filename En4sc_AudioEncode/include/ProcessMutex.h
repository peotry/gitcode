#ifndef _PROCESS_MUTEX_H
#define _PROCESS_MUTEX_H

extern int g_LockFileFd;

void ProcessMutex_init(void);
void ProcessMutex_lock(void);
void ProcessMutex_unlock(void);

int ProcessMutex_file_init(char *pathname);
int ProcessMutex_wr_lock(int fd);
int ProcessMutex_rd_lock(int fd);
int ProcessMutex_file_unlock(int fd);



#endif
