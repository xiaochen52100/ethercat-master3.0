#ifndef KYSEM_H
#define KYSEM_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include "KyLog.h"
#include "KyDefines.h"

union   semun
{
    int   val;
    struct   semid_ds   *buf;
    unsigned   short   int   *array;
    struct   seminfo   *__buf;
};

#define SEM_FAILURE   (-2)
#define SEM_EXIST     (-1)
#define SEM_SUCCESS   (0)

Int32_t Ky_semNew(Uint8_t Id, Int32_t initValue);
Int32_t Ky_semGet(Uint8_t Id);
Int32_t Ky_semp(Int32_t semid, Int8_t sem_n, Int32_t wait);
Int32_t Ky_semv(Int32_t semid, Int8_t sem_n, Int32_t wait);
Int32_t Ky_semdel(Int32_t semid);


#endif
