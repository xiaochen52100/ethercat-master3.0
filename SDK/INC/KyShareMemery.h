#ifndef KYSHAREMEMERY_H
#define KYSHAREMEMERY_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "KyDefines.h"
#include "KyLog.h"

#define SHM_FAILURE (-1)
#define SHM_SUCCESS (0)

Int32_t Ky_shmNew(Uint8_t Id, size_t size);
void *Ky_shmGetAddr(Int32_t shmid);
Int32_t Ky_shmDel(Int32_t shmid);
Int32_t Ky_shmDetach(void *shmaddr);
Int32_t Ky_shmRead(void *buf, void *shmaddr, size_t size);
Int32_t Ky_shmWrite(void *shmaddr, void *buf, size_t size);
Int32_t Ky_shmDataInit(void *shmaddr, size_t size);


#endif
