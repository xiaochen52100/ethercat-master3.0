#ifndef KYSEMMEMERY_H
#define KYSEMMEMERY_H

#include "KySem.h"
#include "KyShareMemery.h"
#include "KyDefines.h"
#include "KyLog.h"

typedef struct KySemInfo{
    Int32_t semId;
    Int32_t semCount;

    Int32_t shmId;
    Int32_t memSize;
    void *addr;
}KySemInfo_t;

Int32_t Ky_semMemNew(KySemInfo_t *sem, Uint8_t Id, Int32_t semValue, size_t size, Int32_t number);
Int32_t Ky_semMemRead(KySemInfo_t *sem, void *buf, size_t size, Int32_t wait);
Int32_t Ky_semMemWrite(KySemInfo_t *sem, void *buf, size_t size, Int32_t wait);
Int32_t Ky_semMemLen(KySemInfo_t *sem, Int32_t wait);
Int32_t Ky_semMemClear(KySemInfo_t *sem, Int32_t wait);
#endif
